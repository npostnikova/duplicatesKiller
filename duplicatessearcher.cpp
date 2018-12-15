#include "duplicatessearcher.h"
#include <thread>
#include <cmath>
#include <cstdlib>
#include <trie.h>
#include <QFile>
#include <QApplication>
#include "duplicatesevent.h"
#include "mythreadreader.h"
#include "mythreadhelper.h"
#include <QThread>
#include <mutex>
#include <map>
#include <utility>
#include "errorevent.h"
#include <QThreadPool>
#include <QFuture>
#include <QtConcurrent/QtConcurrentMap>
#include <QtConcurrent/QtConcurrentRun>

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))


DuplicatesSearcher::DuplicatesSearcher(QString const& dir, QWidget * duplicatesReceiver, QWidget * errorsReceiver) :
    directory(dir), duplicatesReceiver(duplicatesReceiver), errorsReceiver(errorsReceiver) {}

DuplicatesSearcher::~DuplicatesSearcher() {}

size_t DuplicatesSearcher::addSizeBlock(std::vector<std::vector<QString>>& vec, std::vector<QString> const& block) {
    if (block.size() > 1) {
        vec.push_back(block);
        return 1;
    }
    return 0;
}

int DuplicatesSearcher::getBlocks(std::map<size_t, std::vector<QString>> const& sizes,
               std::vector<std::vector<QString>>& smallNames,
               std::vector<std::vector<QString>>& bigNames) {
    auto it = sizes.begin();
    int progress1 = 0;
    int progress2 = 0;
    while (it != sizes.end() && it->first < MAX_FILE_SIZE) {
        progress1 += addSizeBlock(smallNames, it->second);
        it++;
    }
    while (it != sizes.end()) {
        progress2 += addSizeBlock(bigNames, it->second);
        it++;
    }
    return progress1 + progress2 * 20;
}

void DuplicatesSearcher::checkFilesEquality(QDir directory, DuplicatesSearcher* searcher) {
    emit searcher->sendMessage("Scanning the directory...");
    std::map<size_t, std::vector<QString>> sizes;

    searcher->divideFilesBySize(directory, sizes);

    std::vector<std::vector<QString>> smallFileNames;
    std::vector<std::vector<QString>> bigFileNames;

    emit searcher->setProgress(getBlocks(sizes, smallFileNames, bigFileNames));

    if (searcher->stopped) return;


    emit searcher->sendMessage("Searching for duplicates...");

    searcher->runBigFiles(bigFileNames);
    searcher->divideSmallFilesOnThreads(&smallFileNames);
}

std::vector<vector<size_t>> getSmallSizeDupl(std::vector<QByteArray> const& content) {
    if (content.size() <= 1) return {};
    Trie trie(content);
    trie.build();
    return trie.getDuplicates();
}

void DuplicatesSearcher::sendError(Message const& error) {
    ErrorEvent * event = new ErrorEvent(error);
    QApplication::postEvent(errorsReceiver, event);
}

std::vector<QByteArray> DuplicatesSearcher::getSmallSizeContent(std::vector<QString>& names) {
    std::vector<QByteArray> content;
    std::vector<QString> newNames; // if permisson is denied etc
    content.reserve(names.size());

    for (auto it = names.begin(); it != names.end() && !isCanceled(); it++) {
        QFile file;
        file.setFileName(*it);
        if (file.exists() && file.open(QFile::ReadOnly) ) {
            auto fileContent = file.readAll();
            if (fileContent.isEmpty() && file.size() != 0) {
                sendError({*it, Message::ERROR, "Cannot read the file"});
            } else {
                content.push_back(fileContent);
                newNames.push_back(*it);
            }
            file.close();
        } else {
            sendError({*it, Message::ERROR, "Cannot open the file"});
        }
    }
    std::swap(names, newNames);
    return content;
}

void sendDupls(QWidget * duplReceiver, std::vector<std::vector<QString>> const& duplicates) {
    if (!duplicates.empty()) {
        DuplicatesEvent * event = new DuplicatesEvent(duplicates);
        QApplication::postEvent(duplReceiver, event);
    }
}

void DuplicatesSearcher::runBlocksInThread(DuplicatesSearcher * searcher, std::vector<std::vector<QString>> names) {
    std::vector<std::vector<QString>> duplicates;
    for (auto& name : names) {
        auto duplIndexes = getSmallSizeDupl(searcher->getSmallSizeContent(name));

        for (auto& d : duplIndexes) {
            if (searcher->isCanceled()) return;
            if (d.size() <= 1) continue;

            std::vector<QString> duplNames(d.size());
            for (size_t i = 0; i < d.size(); i++) {
                duplNames[i] = name[d[i]];
            }

            duplicates.push_back(duplNames);
            if (duplicates.size() > 10) {
                sendDupls(searcher->duplicatesReceiver, duplicates);
                duplicates.clear();
            }
        }
        searcher->curProgress += 1;
        emit searcher->updateProgress(searcher->curProgress);
    }
    sendDupls(searcher->duplicatesReceiver, duplicates);
}


void DuplicatesSearcher::divideSmallFilesOnThreads(std::vector<std::vector<QString>>* names) {
    if (names->empty())
        return;

    size_t threadsNumber = 3 < names->size() ? 3 : names->size();
    size_t forOneThread = names->size() / threadsNumber + static_cast<size_t>(names->size() % threadsNumber != 0);

    // to make threads work +- same time
    std::random_shuffle(names->begin(), names->end());

    size_t ind = 0;
    std::vector<QFuture<void>> future(threadsNumber);
    for (size_t i = 0; i < threadsNumber; i++) {
        future[i] = QtConcurrent::run(runBlocksInThread, this, std::vector<std::vector<QString>>(names->begin() + ind, names->begin() + min(ind + forOneThread, names->size())));
        ind += forOneThread;
    }

    size_t cnt = 0;
    while (true) {
        cnt = 0;
        std::for_each(future.begin(), future.end(), [&cnt] (QFuture<void> const& f) { cnt += (f.isFinished()) ? 1 : 0; });
        if (cnt == threadsNumber) {
            break;
        }
    }
}

void DuplicatesSearcher::dealWithBigFiles(std::vector<QString> const& names) {
    std::vector<quint64> threadNo(names.size(), 0);

    MyThreadHelper helper(threadNo);
    helper.threadsNum = names.size();
    auto&threadsNum = helper.threadsNum;
    if (isCanceled())
        return;
    auto MEM = (MyThreadReader*)std::malloc(names.size() * sizeof(MyThreadReader));

    auto& fileReaders = helper.fileReaders;
    fileReaders.reserve(names.size());

    for (size_t i = 0; i < threadsNum; i++) {
        fileReaders.push_back(new (&MEM[i]) MyThreadReader(i, names[i], &helper));
        QObject::connect(fileReaders[i], SIGNAL(eof()), &helper, SLOT(setDone()));
        QObject::connect(fileReaders[i], SIGNAL(hash(QByteArray, size_t)), &helper, SLOT(receiveHash(QByteArray, size_t)));
        QObject::connect(fileReaders[i], SIGNAL(failure(Message, size_t)), &helper, SLOT(addError(Message, size_t)));
    }
    helper.needCnt = threadsNum;
    for (size_t i = 0; i < threadsNum; i++) {
        fileReaders[i]->start();
    }

    size_t cntt = 0;
    while (!helper.isDone() && helper.needCnt > 0 && !isCanceled()) {
        helper.doneThreadsCnt = 0;
        helper.oldClassCnt = helper.classCnt;
        emit helper.readPart();

        cntt++;
        while (helper.doneThreadsCnt < helper.needCnt && !isCanceled()) {;}

        std::vector<std::vector<size_t>> needFinish(helper.classCnt - helper.oldClassCnt);
        std::vector<QString> errors;

        for (size_t i = 0; i < threadNo.size() && !isCanceled(); i++) {
            auto& t = threadNo[i];

            if (t <= helper.oldClassCnt)
                continue;
            needFinish[t - helper.oldClassCnt - 1].push_back(i);

        }

        if (!isCanceled())
        for (auto& n : needFinish) {
            if (n.size() < 2) {
                for (auto t : n) {
                    emit helper.stop(t);
                    helper.killThread(t);
                }
            }

        }
        helper.clearMap();
    }

    std::vector<std::vector<QString>> duplicates;
    std::vector<std::vector<QString>> resultDupls(helper.classCnt - helper.oldClassCnt);
    for (size_t i = 0; i < threadNo.size() && !isCanceled(); i++) {
        if (threadNo[i] > helper.oldClassCnt) {
            resultDupls[threadNo[i] - helper.oldClassCnt - 1].push_back(names[i]);
        }
    }

    for (auto& m : resultDupls) {
        if (m.size() > 1)
        duplicates.push_back(m);
    }
    if (!isCanceled()) sendDupls(duplicatesReceiver, duplicates);

    for (size_t i = 0; i < fileReaders.size();i++) {
         helper.stop(i);
         auto& reader = fileReaders[i];
         reader->quit();
         reader->wait();
         reader->~MyThreadReader();
    }
    std::free(MEM);
}


void DuplicatesSearcher::runBigFiles(std::vector<std::vector<QString>> const& blocks) {
    if (blocks.empty())
        return;

    for (auto it = blocks.begin(); it != blocks.end() && !isCanceled(); it++) {
        dealWithBigFiles(*it);
        curProgress += 20;
        emit updateProgress(curProgress);
    }
}


bool DuplicatesSearcher::isCanceled() {
    return stopped;
}


void DuplicatesSearcher::divideFilesBySize(QDir dir,
                                           std::map<size_t, std::vector<QString>>& sizes) {
    QFileInfoList dirList = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries | QDir::NoSymLinks);
    for (auto d : dirList) {
        if (isCanceled() || !d.isReadable())
            return;
        if (d.isFile()) {
            auto insertion = sizes.insert({d.size(), {d.absoluteFilePath()}});
            if (!insertion.second) {
                insertion.first->second.push_back(d.absoluteFilePath());
            }
        } else if (d.isDir()) {
            divideFilesBySize(d.filePath(), sizes);
        }
    }
}


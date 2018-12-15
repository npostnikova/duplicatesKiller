#include "mythreadhelper.h"
#include "duplicatessearcher.h"
#include "mythreadreader.h"
#include "errorevent.h"

#include <QApplication>
#include <QMutexLocker>
#include <utility>


MyThreadHelper::MyThreadHelper(std::vector<quint64>& threads,QObject * p) : threads(threads), QObject(p) {

}

bool MyThreadHelper::isDone() {
    return flag;
}

MyThreadHelper::~MyThreadHelper() {
//    for (auto& reader : fileReaders) {
//        //if (!reader->isFinished() || !reader->finished) {
//            //reader->blocked = true;
//            //if (reader->)
//            //if (!reader->isFinished()) {
//        if (!reader->isFinished()) {
//            reader->quit();
//            reader->wait();
//            //}
//        }

//            //delete reader;
//        //}
//    }

//    std::free(exp);
}

void MyThreadHelper::setDone() {
    flag = true;
}


void MyThreadHelper::clearMap() {
    classNumberMap.clear();
}

void MyThreadHelper::updateClasses(QByteArray hash, size_t threadNumber) {
    std::mutex m;
    std::lock_guard<std::mutex> lg(m);
    doneThreadsCnt++;
    if (hash.isNull()) {
        threads[threadNumber] = 0;
    }
    auto it = classNumberMap.insert({std::make_pair(threads[threadNumber], hash), classCnt + 1});
    if (it.second) {
        classCnt++;
    }
    threads[threadNumber] = it.first->second;
}

void MyThreadHelper::receiveHash(QByteArray hash, size_t threadNumber) {
    updateClasses(hash, threadNumber);
}

void MyThreadHelper::sendErrors(QObject * receiver) {
//    std::vector<std::pair<QString, QString>> errorsVec;
//    for (auto it : errors) {
//        errorsVec.push_back({it.first, it.second});
//    }
//    ErrorEvent * event = new ErrorEvent({errorsVec});
//    QApplication::postEvent(receiver, event);
}
void MyThreadHelper::addError(QString message, QString fileName, size_t fileNumber) {
    if (threads[fileNumber] != 0)
    threadsNum--;
    threads[fileNumber] = 0;
    errors.insert({message, fileName});
}

void MyThreadHelper::killThread(size_t number) {
    if (fileReaders[number] != nullptr) {
        fileReaders[number]->blocked = true;
        fileReaders[number]->quit();
        fileReaders[number]->wait();

        //delete fileReaders[number];
        //fileReaders[number] = nullptr;
        needCnt--;
    }
}


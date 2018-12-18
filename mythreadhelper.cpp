#include "mythreadhelper.h"
#include "duplicatessearcher.h"
#include "mythreadreader.h"
#include "errorevent.h"

#include <QApplication>
#include <QMutexLocker>
#include <utility>


MyThreadHelper::MyThreadHelper(std::vector<quint64>& threads,QObject * p) : QObject(p), threads(threads) {}

bool MyThreadHelper::isDone() {
    return flag;
}

MyThreadHelper::~MyThreadHelper() {}

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

void MyThreadHelper::addError(Message const& error, size_t fileNumber) {
    if (threads[fileNumber] != 0)
    threadsNum--;
    threads[fileNumber] = 0;
    errors.push_back(error);
}

void MyThreadHelper::killThread(size_t number) {
    if (fileReaders[number] != nullptr) {
        stop(number);
        //fileReaders[number]->blocked = true;
        fileReaders[number]->quit();
        fileReaders[number]->wait();

        needCnt--;
    }
}


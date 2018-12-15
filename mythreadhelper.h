#ifndef MY_THREAD_HELPER_H
#define MY_THREAD_HELPER_H
#include <QObject>
#include <map>
#include <utility>
#include <cstdlib>
#include <iostream>
#include "mythreadreader.h"


struct MyThreadHelper : QObject {

    Q_OBJECT
public:
    bool isDone();
    size_t needCnt = 0;

    void sendErrors(QObject * receiver);

    void clearMap();


    bool flag = false;
    size_t classCnt = 0; size_t doneThreadsCnt = 0;
    size_t oldClassCnt = 0;
    size_t threadsNum = 0;
                  std::vector<quint64>& threads;

    std::vector<MyThreadReader*> fileReaders;
    MyThreadHelper(std::vector<quint64>& threads, QObject * p = nullptr);
    ~MyThreadHelper();
    void killThread(size_t number);
signals:
    void readPart();
    void stop(size_t);

public slots:
    void addError(QString message, QString fileName, size_t fileNumber);

    void setDone();
    void receiveHash(QByteArray hash, size_t i);
private:
    void updateClasses(QByteArray hash, size_t threadNumber);

    std::map<QString, QString> errors;

    std::map<std::pair<quint64, QByteArray>, size_t> classNumberMap;

};

#endif // MY_THREAD_HELPER_H

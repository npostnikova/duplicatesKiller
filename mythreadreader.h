#ifndef MYTHREADREADER_H
#define MYTHREADREADER_H

#include <QObject>
#include <QFile>
#include <QThread>
#include <QCryptographicHash>
#include "duplicatessearcher.h"

class MyThreadReader : public QThread {
    Q_OBJECT
public:
    bool blocked = false;
    bool started = false;
    bool isFinished = false;
    MyThreadReader(size_t n, QString const& fileName, QObject * receiver, QObject * parent = nullptr);


    MyThreadReader(QObject* o = nullptr) : QThread (o),  number(0) {}
    ~MyThreadReader();

    void finish();

    void waiting();

    QByteArray getHash(QByteArray buffer);
    QByteArray bigFileHash();
protected:
    void run();

public slots:

    void readData();
    void stop(size_t);


signals:
    void hash(QByteArray, size_t);
    void eof();
    void failure(Message, size_t);


private:
    QString fileName;
    QFile file;
    const size_t number;
    int state = 0;
    QObject * receiver;

    const size_t BUFFER_SIZE = 1000000;
};

#endif // MYTHREADREADER_H

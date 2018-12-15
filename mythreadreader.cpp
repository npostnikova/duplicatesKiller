#include "mythreadreader.h"
#include <iostream>
//#include <QDebug>
#include<cassert>


MyThreadReader::MyThreadReader(size_t n, QString const& fileName, QObject * receiver, QObject * parent) :
    QThread (parent), fileName(fileName), file(fileName), number(n), receiver(receiver) {
    connect(receiver, SIGNAL(readPart()), this, SLOT(readData()));
    connect(receiver, SIGNAL(stop(size_t)), this, SLOT(stop(size_t)));
    state = 0;

}
MyThreadReader::~MyThreadReader() {}

void MyThreadReader::finish() {
    blocked = true;
    file.close();
    isFinished = true;

}

void MyThreadReader::waiting() {
    while (state == 0) {;}
}

QByteArray MyThreadReader::getHash(QByteArray buffer) {
    return QCryptographicHash::hash(buffer, QCryptographicHash::Algorithm::RealSha3_256);
}

void MyThreadReader::readData() {
    if (blocked) return;

    while (!started) {;}

    QByteArray data = file.read(BUFFER_SIZE);
    if (data.size() == 0) {
        emit failure({fileName, Message::ERROR, "Could not read from the file"}, number);
        return;
    }
    emit hash(getHash(data), number);

    if (file.atEnd()) {
        state = 2;
        blocked = true;
        emit eof();
    }
}

void MyThreadReader::stop(size_t n) {
    if (n == number) {
        blocked = true;
        state = 2;
    }
}

void MyThreadReader::run() {
    file.setFileName(fileName);

    if (file.exists() && file.open(QFile::ReadOnly)) {
        started = true;
    } else {
        blocked = true;
        emit failure({fileName, Message::ERROR, QString("Could not find or open the file")}, number);
    }
    waiting();

    finish();
}

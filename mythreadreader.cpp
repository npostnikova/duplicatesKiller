#include "mythreadreader.h"
#include <iostream>
//#include <QDebug>
#include<cassert>


MyThreadReader::MyThreadReader(size_t n, QString const& fileName, QObject * receiver, QObject * parent) :
    QThread (parent), number(n), fileName(fileName), file(fileName), receiver(receiver) {
    connect(receiver, SIGNAL(readPart()), this, SLOT(readData()));
    connect(receiver, SIGNAL(stop(size_t)), this, SLOT(stop(size_t)));
    if (number > 400) std::cout << "NUMBER"<< std::endl;
    state = 0;

}
MyThreadReader::~MyThreadReader() {
}



void MyThreadReader::finish() {

    blocked = true;
    if (number > 1000) {
        std::cout << "MEOWMEOWMEOW" << std::endl;
    }
    //assert(number < 1000);
    //std::cout << number << "finished" << std::endl;
    try {
        file.close();
    } catch (...) {
        //emit failure("Could not close the file", fileName, number);
    }

    finishedd = true;
    //emit finished();
    //std::cout << "im'here"  << std::endl;
}


void MyThreadReader::waitt() {
    while (state == 0) {;
        /*he he*/
    }
}

QByteArray MyThreadReader::getHash(QByteArray buffer) {
    return QCryptographicHash::hash(buffer, QCryptographicHash::Algorithm::RealSha3_256);
}

QByteArray MyThreadReader::bigFileHash() {
    //QFile file(filePath);
    //if (!file.open(QIODevice::ReadOnly))
    //        return ;
    //file.open(QFile::ReadOnly);
    QByteArray hash;

    QByteArray buffer;
    while (!file.atEnd()) {
        buffer = file.read(2000000);
        hash.append(QCryptographicHash::hash(buffer, QCryptographicHash::Algorithm::RealSha3_256));
    }
    ////
//    qint64 bufferSize = 2048 * 2048;
//    char * buffer = new char[bufferSize];
//    qint64 wasRead = 0;

//    while (!file.atEnd()) {
//        file.read(buffer, bufferSize);
//        hash.append(QCryptographicHash::hash(buffer, QCryptographicHash::Algorithm::RealSha3_256));
////        if (wasRead < bufferSize) {
////            break;
////        }
//    }
    //file.close();
    //delete [] buffer;
    return hash;
}

void MyThreadReader::readData() {
    if (blocked) return;
    while (!started) {;}
    if (enormous) {
        emit hash(bigFileHash(), number);
    } else {

    //std::cout << "read!!!" << std::endl;
    QByteArray data;
    try {
// хз что с этим делать
        data = file.read(BUFFER_SIZE);
    } catch (...) {
        emit failure("Could not read from the file", fileName, number);
        return;

    }

    emit hash(getHash(data), number);
    //std::cout << "hash" << number << std::endl;
    }
    if (file.atEnd()) {
        state = 2;
        blocked = true;
        //std::cout << number << " is done" << std:: endl;
        emit eof();
    }
 //   delete [] buffer;
}

void MyThreadReader::stop(size_t n) {
    if (n == number) {
        blocked = true;
        state = 2;
        //finish();

    }
}

void MyThreadReader::run() {

    //std::cout << "RUNRUNRUN" << std::endl;

    file.setFileName(fileName);

        if (file.exists() && file.open(QFile::ReadOnly)) {
        started = true;
        //std::cout << "opened" << number << std::endl;
        //if (file.size() > 10000000LL) enormous = true;
    } else {
            std::cout << "FAILFAILFAIL" <<std::endl;
            blocked = true;
        emit failure(QString("Could not find or open the file"), fileName, number);
    }
    waitt();

    finish();
}

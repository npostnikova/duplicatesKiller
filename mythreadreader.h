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


    // ПОФИКСИ!!!!!!!!
    bool blocked = false;
    bool started = false;
    bool finishedd = false;
    MyThreadReader(size_t n, QString const& fileName, QObject * receiver, QObject * parent = nullptr);


    MyThreadReader(QObject* o = nullptr) : QThread (o),  number(0) {}
    ~MyThreadReader();

    void finish();

    void waitt();

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
    void failure(QString, QString, size_t);
//    void finished();


private:
    QString fileName;
    QFile file;
    const size_t number;
    int state = 0;
    QObject * receiver;


    bool enormous = false;
    // 1 -- read
    // 2 -- finish

    const size_t BUFFER_SIZE = 1000000; ///!!!!!!!!!! 00000


};

#endif // MYTHREADREADER_H

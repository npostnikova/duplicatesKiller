#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include "trie.h"
#include <iostream>
#include <QDialog>
#include <vector>
#include <QThread>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/img/rocket.png"));
    MainWindow w;

    cout << QDir::currentPath().toStdString();
    w.show();
    a.thread()->setPriority(QThread::HighestPriority);
    return a.exec();

}

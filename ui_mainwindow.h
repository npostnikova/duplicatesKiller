#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QTreeView>
#include <QProgressBar>
#include <QMdiSubWindow>
#include <QStatusBar>
#include <QDialog>
#include <QtWidgets/QTreeWidget>
#include <QTreeView>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QWidget>
#include <QtWidgets/QListView>
#include <QStandardItem>
#include <QPushButton>
#include <QProcess>
#include <QWidgetAction>
#include <mytreeview.h>
#include <mylistview.h>
#include <QListWidgetItem>
#include <QProgressDialog>
#include <QLabel>
#include <QDir>


QT_BEGIN_NAMESPACE

class Ui_MainWindow {
public:
    MyTreeView * duplicatesTreeView;
    MyListView * exceptionsList;
    QSplitter * splitter;
    QMenuBar * menuBar;
    QMenu * menuFile;
    QMenu * menuHelp;

    QProgressBar * progressBar;
    QToolBar * toolBar;
    QStatusBar * statusBar;
    QLabel * statusLabel;
    
    QAction *actionScan_Directory;
    QAction *actionExit;
    QAction *actionAbout;
    QAction *actionDelete;
    QAction *actionCancel;

    enum search {
        START = 0,
        FINISH = 1
    };

    void searchAction(bool b) {
        actionDelete->setEnabled(b);
        actionScan_Directory->setEnabled(b);
        actionCancel->setDisabled(b);
        progressBar->setVisible(!b);
        progressBar->setRange(0,0);
    }


    void setupUi(QMainWindow * MainWindow) {
        MainWindow->setObjectName("Duplicates Killer");
        MainWindow->resize(1000, 600);
        duplicatesTreeView = new MyTreeView(MainWindow);

        exceptionsList = new MyListView(MainWindow);
        splitter = new QSplitter(Qt::Vertical, MainWindow);
        splitter->addWidget(duplicatesTreeView);
        splitter->addWidget(exceptionsList);
        splitter->setStretchFactor(0, 5);
        splitter->setStretchFactor(1, 2);
        MainWindow->setCentralWidget(splitter);

        progressBar = new QProgressBar(MainWindow);

        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 800, 31));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));

        MainWindow->setMenuBar(menuBar);

        actionScan_Directory = new QAction(MainWindow);
        actionExit = new QAction(MainWindow);
        actionAbout = new QAction(MainWindow);
        actionDelete = new QAction(MainWindow);
        actionCancel = new QAction(MainWindow);

        actionScan_Directory->setObjectName(QStringLiteral("actionScan_Directory"));
        actionExit->setObjectName(QStringLiteral("actionExit"));
        actionAbout->setObjectName(QStringLiteral("actionAbout"));


        actionScan_Directory->setIcon(QIcon(":/img/open_folder.png"));
        actionDelete->setIcon(QIcon(":/img/delete.png"));
        actionAbout->setIcon(QIcon(":/img/about.png"));
        actionExit->setIcon(QIcon(":/img/exit.png"));
        actionCancel->setIcon(QIcon(":/img/stop.png"));

        actionAbout->setStatusTip("About");

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuHelp->menuAction());

        menuFile->addAction(actionScan_Directory);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuHelp->addAction(actionAbout);

        toolBar = new QToolBar(MainWindow);
        //toolBar->setOrientation(Qt::Vertical);
        toolBar->setIconSize(QSize(64, 64));

        toolBar->addAction(actionScan_Directory);
        toolBar->addAction(actionDelete);
        toolBar->addAction(actionCancel);

        /*QWidget* empty = new QWidget();
        empty->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
        toolBar->addWidget(empty);*/

        MainWindow->addToolBar(Qt::LeftToolBarArea, toolBar);


        statusBar = new QStatusBar(MainWindow);
        statusLabel = new QLabel(MainWindow);
        progressBar->setVisible(true);

        statusBar->addWidget(statusLabel, 5);
        statusLabel->setText("Test directory is done");
        statusBar->addWidget(progressBar, 2);
        progressBar->setVisible(false);
        MainWindow->setStatusBar(statusBar);

        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        actionScan_Directory->setText(QApplication::translate("MainWindow", "&Scan Directory...", nullptr));
        actionExit->setText(QApplication::translate("MainWindow", "&Exit", nullptr));
        actionAbout->setText(QApplication::translate("MainWindow", "About", nullptr));
        menuFile->setTitle(QApplication::translate("MainWindow", "Fi&le", nullptr));
        menuHelp->setTitle(QApplication::translate("MainWindow", "Help", nullptr));

        QMetaObject::connectSlotsByName(MainWindow);
    }

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE



#endif // UI_MAINWINDOW_H

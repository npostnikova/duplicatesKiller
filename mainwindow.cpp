#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "duplicatessearcher.h"
#include "mytreeview.h"

#include <QCommonStyle>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QFuture>
#include <QtConcurrent/QtConcurrentMap>
#include <QtConcurrent/QtConcurrentRun>
#include <QDesktopWidget>
#include <QProcess>
#include <QTimer>
#include <QObject>
#include <QMessageBox>

#include <QFutureWatcher>
#include <iostream>


void setChildrenState(QStandardItem * item, Qt::CheckState state) {
    for (size_t i = 0; i < item->rowCount(); i++) {
        item->child(i)->setCheckState(state);
    }
}

size_t countChildState(QStandardItem * item, Qt::CheckState state) {
    size_t cnt = 0;
    for (int i = 0; i < item->rowCount(); i++) {
        if (item->child(i)->checkState() == state) {
            cnt++;
        }
    }
    return cnt;
}

void MainWindow::onClick(QStandardItem* item) {
    if (item->checkState() == Qt::Checked) {
        setChildrenState(item, Qt::Checked);
    } else if (countChildState(item, Qt::Checked) == item->rowCount()) {
        setChildrenState(item, Qt::Unchecked);
    }
}


void MainWindow::openInNotepad(const QModelIndex& index) {
    if (index.column() == 0 && ui->duplicatesTreeView->getModel()->itemFromIndex(index)->rowCount() == 0) {
        QProcess::execute("notepad.exe", {index.data().toString()});
    }
}

void MainWindow::changedChild(QStandardItem* item) {
    if (item->rowCount() != 0)
        return;

    if (item->checkState() == Qt::Unchecked &&
            item->parent()->checkState() == Qt::Checked) {
        item->parent()->setCheckState(Qt::Unchecked);
    }
    if (item->checkState() == Qt::Checked &&
            countChildState(item->parent(), Qt::Checked) == item->parent()->rowCount()) {
            item->parent()->setCheckState(Qt::Checked);
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    connect(ui->duplicatesTreeView->getModel(),
                SIGNAL(itemChanged(QStandardItem*)),
                this,
                SLOT(onClick(QStandardItem*)));

    connect(ui->duplicatesTreeView->getModel(),
                SIGNAL(itemChanged(QStandardItem*)),
                this,
                SLOT(changedChild(QStandardItem*)));

    connect(ui->duplicatesTreeView,
                &MyTreeView::doubleClicked,
                this,
                &MainWindow::openInNotepad);

    connect(ui->actionScan_Directory, &QAction::triggered, this, &MainWindow::select_directory);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::quit);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::show_about_dialog);
    connect(ui->actionCancel, &QAction::triggered, this, &MainWindow::cancelSearching);
    connect(ui->actionDelete, &QAction::triggered, this, &MainWindow::deleteDuplicates);
    scan_directory(QDir::currentPath());
}

MainWindow::~MainWindow() {}


void MainWindow::select_directory()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Directory for Scanning",
                                                    QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (dir != nullptr) {
        ui->searchAction(ui->START);
        scan_directory(dir);
        ui->searchAction(ui->FINISH);
    }
}

void MainWindow::runSearch(MainWindow * w, QString const& dir, Ui_MainWindow* ui) {
    DuplicatesSearcher duplicatesSearcher(dir, ui->duplicatesTreeView, ui->exceptionsList);
    connect(w, SIGNAL(interruptSearching()), &duplicatesSearcher, SLOT(interrupt()));
    connect(&duplicatesSearcher, SIGNAL(setProgress(int)), ui->progressBar, SLOT(setMaximum(int)));
    connect(&duplicatesSearcher, SIGNAL(updateProgress(int)), ui->progressBar, SLOT(setValue(int)));
    connect(&duplicatesSearcher, SIGNAL(sendMessage(QString)), ui->statusLabel, SLOT(setText(QString)));

    QFuture<void> future = QtConcurrent::run(DuplicatesSearcher::checkFilesEquality, dir, &duplicatesSearcher);

    while (!future.isFinished()) {
        QTimer timer;
        timer.setSingleShot(true);
        timer.start(1000);
        QCoreApplication::processEvents(QEventLoop::AllEvents | QEventLoop::WaitForMoreEvents);
    }
    if (!duplicatesSearcher.isCanceled()) {
        emit duplicatesSearcher.sendMessage("The search is done!");

        if (ui->duplicatesTreeView->getModel()->rowCount() == 0)
            duplicatesSearcher.sendError({"", Message::DONE, "The search is successfully done, no duplicates was found!"});
        else
            duplicatesSearcher.sendError({"", Message::DONE, "The search is successfully done!"});
    } else {
        emit duplicatesSearcher.sendMessage("The search was cancelled :(");
        duplicatesSearcher.sendError({"", Message::ERROR, "The search was cancelled!"});
    }
}

void MainWindow::scan_directory(QString const& dir) {
    ui->duplicatesTreeView->clean();
    ui->exceptionsList->clean();
    setWindowTitle(QString("Duplicates in directory - %1").arg(dir));

    QFuture<void> future = QtConcurrent::run(runSearch, this, dir, ui.get());
    future.waitForFinished();

}

void MainWindow::show_about_dialog() {
    QMessageBox::aboutQt(this);
}

void MainWindow::cancelSearching() {
    emit interruptSearching();
}

void MainWindow::quit() {
    cancelSearching();
    QWidget::close();
}


void MainWindow::deleteDuplicates() {

    int n = QMessageBox::warning(0,
                                 "Warning",
    "Do you really want to delete files in my program?"
                                 "\n Think twice!",

                                 QMessageBox::Yes | QMessageBox::Cancel,
                                 QMessageBox::Cancel
                                 );
    if (n == QMessageBox::Cancel) {
        return;
    }
    ui->actionDelete->setEnabled(false);
    ui->duplicatesTreeView->deleteTraverse();
    ui->actionDelete->setEnabled(true);
}

void MainWindow::deleteFile(QString name) {
    QFile file(name);
    if (file.exists()) {
        ui->exceptionsList->insertError({name, Message::DONE, "Deleted: "});
        //file.remove();
    }
}



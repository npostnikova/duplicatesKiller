#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <QStandardItem>
#include <QFuture>

#include "mytreeview.h"
#include "ui_mainwindow.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void deleteFile(QString name);

signals:
    void interruptSearching();

private slots:
    void openInNotepad(const QModelIndex& index);
    void select_directory();
    void scan_directory(QString const& dir);
    void show_about_dialog();
    void changedChild(QStandardItem* item);
    void onClick(QStandardItem* item);
    void cancelSearching();
    void deleteDuplicates();
    void quit();
private:

    static void runSearch(MainWindow * w, QString const& dir, Ui_MainWindow* ui);

    std::unique_ptr<Ui::MainWindow> ui;
    QFuture<void> * processFuture = nullptr;
};

#endif // MAINWINDOW_H

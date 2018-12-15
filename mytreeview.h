#ifndef MYTREEVIEW_H
#define MYTREEVIEW_H

#include <QTreeView>
#include <QStandardItemModel>
#include <memory>
#include "duplicatesevent.h"
#include <QApplication>

struct MyModel : public QStandardItemModel {
    MyModel(QWidget * parent);

    ~MyModel();

    void clean();

    void appendRow(QList<QStandardItem*> const& data);

    void appendRows(std::vector<std::vector<QString>> const& data);
};

struct MyTreeView : QTreeView {
    MyTreeView(QWidget * parent = nullptr);

    void customEvent(QEvent * event);
    
    static QList<QStandardItem*> buildDuplicatesItem(std::vector<QString> const& fileNames);

    void deleteTraverse();
    
    QStandardItemModel* getModel() {
        return model;
    }

    void clean();

private:
    void setUp();

    MyModel* model;

    QWidget* deleter;
};

#endif // MYTREEVIEW_H

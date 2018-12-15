#include "mytreeview.h"

MyModel::MyModel(QWidget * parent) : QStandardItemModel(parent) {}

MyModel::~MyModel() {}

void MyModel::appendRow(QList<QStandardItem*> const& data) {
    insertRow(0, data);
}

void MyModel::appendRows(std::vector<std::vector<QString>> const& data) {
   for (auto& d : data) {
        appendRow(MyTreeView::buildDuplicatesItem(d));
    }
}

void MyModel::clean() {
    clear();
}

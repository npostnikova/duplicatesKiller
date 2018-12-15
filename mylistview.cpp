#include "mylistview.h"
#include "errorevent.h"

MyListView::MyListView(QWidget * parent) : QListView(parent), model(new QStandardItemModel(this)) {
    setUp();
}


void MyListView::setUp() {
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setModel(model);
    setUniformItemSizes(true);
}


#include <iostream>
QStandardItem* makeItem(Message const& error) {
    QStandardItem * item = new QStandardItem(error.message + error.fileName);

    static std::vector<QString> icons = {":/img/cute_warn.png", ":/img/cross.png", ":/img/tick.png"};
    item->setIcon(QIcon(icons[error.messageType]));
    return item;
}



void MyListView::customEvent(QEvent * event) {
    if (static_cast<int>(event->type()) == ErrorEvent::ErrorType) {
        emit model->layoutAboutToBeChanged();
        model->insertRow(0, makeItem(static_cast<ErrorEvent*>(event)->getError()));
    }
    QWidget::customEvent(event);
}

QList<QStandardItem*> MyListView::buildErrorsItem(Message const& error) {
    insertError(error);
}

void MyListView::insertError(Message const& error) {
    model->appendRow(makeItem(error));
}

void MyListView::clean() {
    model->clear();
    setUp();
}

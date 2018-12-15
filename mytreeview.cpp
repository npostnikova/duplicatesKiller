#include "mytreeview.h"
#include <QFileInfo>
#include <QDateTime>
#include <QHeaderView>
#include "mainwindow.h"

MyTreeView::MyTreeView(QWidget * parent) : QTreeView(parent), deleter(parent), model(new MyModel(this)) {
    setUp();
    
}

void MyTreeView::setUp() {
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    model->setHorizontalHeaderLabels({"File name", "Size", "Date", "Number"});
    setModel(model);
    header()->setStretchLastSection(false);
    header()->setSectionResizeMode(0, QHeaderView::Stretch);
    setColumnWidth(1, 150);
    setColumnWidth(2, 150);
    setColumnWidth(3, 80);
    setSortingEnabled(true);
    setUniformRowHeights(true);
}


QList<QStandardItem*> prepareRow(const QString &first,
                                                const QString &second,

                                  const QString &third, const QString& num = nullptr) {
    QList<QStandardItem *> rowItems;

    QStandardItem * item = new QStandardItem;
    item->setCheckable(true);
    item->setCheckState(Qt::Unchecked);
    item->setText(first);

    rowItems << item;
    rowItems << new QStandardItem(second);
    rowItems << new QStandardItem(third);
    if (num != nullptr)
        rowItems << new QStandardItem(num);
    return rowItems;
}

QList<QStandardItem*> MyTreeView::buildDuplicatesItem(std::vector<QString> const& fileNames) {
    if (fileNames.size() <= 1) {
        throw "Wrong duplicates item request";
    }
    auto firstFile = QFileInfo(fileNames[0]);
    auto pr = prepareRow(firstFile.filePath(), QString::number(firstFile.size()), firstFile.birthTime().toString("dd.MM.yyyy"), QString::number(fileNames.size()));
    for (auto name : fileNames) {
        auto fileInfo = QFileInfo(name);
        pr.first()->appendRow(prepareRow(fileInfo.absoluteFilePath(), QString::number(fileInfo.size()), fileInfo.birthTime().toString("dd.MM.yyyy")));
    }
    return pr;
}

void MyTreeView::clean() {
    model->clean();
    setUp();
}


void MyTreeView::customEvent(QEvent * event) {
    if (static_cast<int>(event->type()) == DuplicatesEvent::DuplicatesType) {
        emit model->layoutAboutToBeChanged();
        model->appendRows(static_cast<DuplicatesEvent*>(event)->getFilesName());
    }
    QWidget::customEvent(event);
}

void MyTreeView::deleteTraverse() {
    auto item = model->invisibleRootItem();
    for (size_t i = 0; i < item->rowCount(); ) {
        auto child = item->child(i);
        for (size_t j = 0; j < child->rowCount(); ) {
            auto ch = child->child(j);
            if (ch->checkState() == Qt::Checked) {
                ch->takeColumn(0);
                ((MainWindow*)deleter)->deleteFile(ch->text());
                child->removeRow(j);
            } else {
                j++;
            }
        }
        if (!child->hasChildren()) {
            item->removeRow(i);
        } else {
            i++;
        }
    }
}

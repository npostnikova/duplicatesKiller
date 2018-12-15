#ifndef MYLISTVIEW_H
#define MYLISTVIEW_H

#include <QListView>
#include <QStandardItemModel>

struct Message {
    enum MessageType {
        WARNING = 0,
        ERROR = 1,
        DONE = 2
    };

    QString fileName;
    MessageType messageType;
    QString message;

    Message(QString name, MessageType mt, QString message = "") :
        fileName(name), messageType(mt), message(message) {}
};

struct MyListView : public QListView {
    MyListView(QWidget * parent = nullptr);

    void customEvent(QEvent * event);

    void clean();

    void setUp();
private:
    QList<QStandardItem*> buildErrorsItem(Message const& errors);

    QStandardItemModel * model;
};



#endif // MYLISTVIEW_H

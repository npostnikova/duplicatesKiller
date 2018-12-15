#ifndef ERROREVENT_H
#define ERROREVENT_H

#include <QEvent>
#include <vector>
#include <QString>
#include <utility>
#include "mylistview.h"

struct ErrorEvent : public QEvent {
    ErrorEvent(Message const& error);

    ~ErrorEvent();

    Message getError();

    enum {ErrorType = User + 2};
private:
    Message error;
};


#endif // ERROREVENT_H

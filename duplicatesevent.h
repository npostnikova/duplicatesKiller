#ifndef DUPLICATESEVENT_H
#define DUPLICATESEVENT_H

#include <QEvent>
#include <vector>
#include <QString>

struct DuplicatesEvent : public QEvent {
    DuplicatesEvent(std::vector<std::vector<QString>> const& duplicates);

    ~DuplicatesEvent();

    std::vector<std::vector<QString>> getFilesName();

    enum {DuplicatesType = User + 1};
private:

    std::vector<std::vector<QString>> duplicates;


};

#endif // DUPLICATESEVENT_H

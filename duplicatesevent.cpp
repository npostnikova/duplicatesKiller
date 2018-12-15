#include "duplicatesevent.h"

DuplicatesEvent::DuplicatesEvent(std::vector<std::vector<QString>> const& duplicates) :
    QEvent((Type)DuplicatesType), duplicates(duplicates) {}

DuplicatesEvent::~DuplicatesEvent() {}

std::vector<std::vector<QString>> DuplicatesEvent::getFilesName() {
    return duplicates;
}

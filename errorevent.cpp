#include "errorevent.h"

ErrorEvent::ErrorEvent(Message const& error) :
     QEvent((Type)ErrorType), error(error) {}

ErrorEvent::~ErrorEvent() {}

Message ErrorEvent::getError() {
    return error;
}

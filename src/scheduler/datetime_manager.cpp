module;
#include <memory>
module datetime_manager;

namespace infinity {

void DatetimeManager::setSystemTimeZone() {
   this->SystemTimeZone = std::getenv("TZ");

}

}
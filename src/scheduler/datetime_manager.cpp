module;
#include <memory>
module DatetimeManager;





class DatetimeManager {
public:
    DatetimeManager() : SystemTimeZone(nullptr), SystemDate(nullptr), UserTimeZone(nullptr), UserDate(nullptr) {}

    ~DatetimeManager() {
        delete[] UserTimeZone;
        delete[] UserDate;
    }

    DatetimeManager(const DatetimeManager& other) :
        SystemTimeZone(other.SystemTimeZone),
        SystemDate(other.SystemDate),
        UserTimeZone(nullptr),
        UserDate(nullptr) {
        if (other.UserTimeZone) {
            UserTimeZone = new char[strlen(other.UserTimeZone) + 1];
            strcpy(UserTimeZone, other.UserTimeZone);
        }
        if (other.UserDate) {
            UserDate = new char[strlen(other.UserDate) + 1];
            strcpy(UserDate, other.UserDate);
        }
    }

    DatetimeManager& operator=(const DatetimeManager& other) {
        if (this != &other) {
            SystemTimeZone = other.SystemTimeZone;
            SystemDate = other.SystemDate;
            delete[] UserTimeZone;
            UserTimeZone = nullptr;
            delete[] UserDate;
            UserDate = nullptr;
            if (other.UserTimeZone) {
                UserTimeZone = new char[strlen(other.UserTimeZone) + 1];
                strcpy(UserTimeZone, other.UserTimeZone);
            }
            if (other.UserDate) {
                UserDate = new char[strlen(other.UserDate) + 1];
                strcpy(UserDate, other.UserDate);
            }
        }
        return *this;
    }

    const char* SystemTimeZone;
    const char* SystemDate;
    char* UserTimeZone;
    char* UserDate;
};
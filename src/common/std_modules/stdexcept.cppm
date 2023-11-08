module;
#include <stdexcept>
#include <system_error>
# 4 __FILE__ 1 3 // Enter "faked" system files since std is reserved module name
export module std_lib:stdexcept;
export namespace std_lib {
    using std::runtime_error;
    using std::logic_error;
    using std::rethrow_exception;
    using std::current_exception;
    using std::invalid_argument;
    using std::error_code;
    using std::operator==;
}

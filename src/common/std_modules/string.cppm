module;
#include <string>
# 3 __FILE__ 1 3 // Enter "faked" system files since std is reserved module name
export module std:string;
export namespace std {
    using std::string;
    using std::basic_string;
    using std::string_view;
    using std::basic_string_view;

    using std::getline;
    
    using std::operator+;
    using std::operator==;
    using std::operator<;
    using std::operator>;
#if defined(__GLIBCXX_) && __GLIBCXX_ < 20200723
    using std::operator<=>;
#endif

    using std::to_string;

    inline namespace literals {
        using std::literals::operator""s;
    }
}

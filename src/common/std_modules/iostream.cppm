module;
#include <iostream>
#include <iomanip>
#include <fstream>
# 5 __FILE__ 1 3 // Enter "faked" system files since std is reserved module name
export module std_lib:iostream;
export namespace std_lib {
    using std::cout;
    using std::cerr;
    using std::endl;
    using std::printf;
    using std::boolalpha;
    using std::noboolalpha;
    using std::setw;
    using std::flush;
    using std::istream;
    using std::ostream;
    using std::basic_ostream;
    using std::iostream;
    using std::ifstream;
    using std::basic_stringstream;
    using std::stringstream;
    using std::istringstream;
    using std::hex;
    using std::ios;
    using std::ios_base;

    using std::operator<<;
    using std::operator>>;
}

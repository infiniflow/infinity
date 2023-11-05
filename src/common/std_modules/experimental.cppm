//
// Created by jinhai on 23-11-4.
//

module;
#include <experimental/source_location>
 # 3 __FILE__ 1 3 // Enter "faked" system files since std is reserved module name
// export module std:fstream;
export module std:experimental;
export namespace std {

using std::experimental::source_location;

}

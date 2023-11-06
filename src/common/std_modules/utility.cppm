module;
#include <utility>
# 3 __FILE__ 1 3 // Enter "faked" system files since std is reserved module name
export module std:utility;
export namespace std {
    using std::move;
    using std::swap;
    using std::forward;
    using std::exchange;
    using std::pair;
    using std::make_pair;

    using std::declval;

    using std::index_sequence;
    using std::make_index_sequence;
    using std::make_integer_sequence;
    using std::integer_sequence;
    using std::index_sequence_for;
}

#include "utils/microfmt.hpp"

#include <iostream>

namespace cpptrace {
namespace microfmt {
namespace detail {

    std::ostream& get_cout() {
        return std::cout;
    }

}
}
}

#include "inter_module.h"

Shared create_shared() {
    return { 123 };
}

bool check_shared(const Shared &shared) {
    return shared.value == 123;
}

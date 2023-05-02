#include "policy_enforcer.h"

namespace infinity {

bool PolicyEnforcer::AdmitQuery(SharedPtr<QueryContext> query_context) {
    return true;
}

}
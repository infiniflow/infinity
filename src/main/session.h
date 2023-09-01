//
// Created by JinHai on 2022/9/5.
//

#pragma once

#include "query_context.h"

#include "prepared_plan.h"

#include <unordered_map>

namespace infinity {

class Session {
public:
    explicit Session();

    [[nodiscard]] inline String&
    current_schema() {
        return current_schema_;
    }

    [[nodiscard]] inline const String&
    user_name() const {
        return user_name_;
    }

    [[nodiscard]] inline u64
    user_id() const {
        return user_id_;
    }

    inline UniquePtr<TransactionContext>&
    transaction() {
        return transaction_;
    };

    inline void
    SetClientInfo(const String ip_address, u16 port) {
        client_address_ = std::move(ip_address);
        client_port_ = port;
    }

private:
    // transaction context
    UniquePtr<TransactionContext> transaction_;

    // session config such as memory limitation

    // prepared plan for PREPARE and EXECUTE statement
    HashMap<String, SharedPtr<PreparedPlan>> prepared_plans_;

    // Current schema
    String current_schema_;

    // User / Tenant information
    String tenant_name_;
    String user_name_;

    u64 user_id_{0};

    String client_address_;
    u16 client_port_;
};

}

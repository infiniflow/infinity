//
// Created by JinHai on 2022/9/5.
//

module;

import stl;
import txn;

export module session;

namespace infinity {

export class Session {
public:
    explicit Session() : current_database_("default") {}

    [[nodiscard]] inline String &current_database() { return current_database_; }

    [[nodiscard]] inline const String &user_name() const { return user_name_; }

    [[nodiscard]] inline u64 user_id() const { return user_id_; }

    inline void SetClientInfo(const String &ip_address, u16 port) {
        client_address_ = ip_address;
        client_port_ = port;
    }

public:
    // Txn is session level.
    Txn *txn_{};

private:
    // Current schema
    String current_database_;

    // User / Tenant information
    String tenant_name_;
    String user_name_;

    u64 user_id_{0};

    String client_address_{};
    u16 client_port_{};
};

} // namespace infinity

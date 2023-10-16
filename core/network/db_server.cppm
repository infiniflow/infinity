
module;

import stl;
import singleton;
import boost;
import connection;

export module db_server;

namespace infinity {

export struct StartupParameter {
    SharedPtr<String> config_path{};
};

export class DBServer : public Singleton<DBServer> {
public:
    explicit DBServer(const StartupParameter &parameter);

    void Run();

    void Shutdown();

private:
    void CreateConnection();

    void StartConnection(SharedPtr<Connection> &connection);

    atomic_bool initialized{false};
    au64 running_connection_count_{0};
    AsioIOService io_service_{};
    UniquePtr<AsioAcceptor> acceptor_ptr_{};
    SharedPtr<String> config_path_{};
};

}

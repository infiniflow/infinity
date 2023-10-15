
module;

import stl;
import singleton;

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
//
//    void StartConnection(SharedPtr<Connection> &connection);
//
    atomic_bool initialized{false};
//    au64 running_connection_count_{0};
//    boost::asio::io_service io_service_{};
//    UniquePtr<boost::asio::ip::tcp::acceptor> acceptor_ptr_{};
    SharedPtr<String> config_path_{};
};

}

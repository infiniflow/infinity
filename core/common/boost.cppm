//
// Created by jinhai on 23-10-14.
//

module;

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/bind/bind.hpp>

export module boost;

namespace infinity {

export using boost_error_code = boost::system::error_code;
export using asio_ip_addr = boost::asio::ip::address;
export using asio_acceptor = boost::asio::ip::tcp::acceptor;

export asio_ip_addr asio_make_address(const std::string &str, boost::system::error_code &ec) BOOST_ASIO_NOEXCEPT {
    return boost::asio::ip::make_address(str.c_str(), ec);
}

} // namespace infinity

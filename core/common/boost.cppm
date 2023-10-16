//
// Created by jinhai on 23-10-14.
//

module;

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/bind/bind.hpp>

export module boost;

namespace infinity {

export using BoostErrorCode = boost::system::error_code;
export using AsioIpAddr = boost::asio::ip::address;
export using AsioAcceptor = boost::asio::ip::tcp::acceptor;
export using AsioIOService = boost::asio::io_service;
export using AsioEndPoint = boost::asio::ip::tcp::endpoint;
export using AsioSocket = boost::asio::ip::tcp::socket;

export AsioIpAddr asio_make_address(const std::string &str, boost::system::error_code &ec) BOOST_ASIO_NOEXCEPT {
    return boost::asio::ip::make_address(str.c_str(), ec);
}

} // namespace infinity

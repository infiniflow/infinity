// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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

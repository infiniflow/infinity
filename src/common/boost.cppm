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

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

export module infinity_core:boost;

namespace boost {
namespace system {
export using boost::system::error_code;
}
namespace asio {
export using boost::asio::io_context;
namespace ip {
export using boost::asio::ip::tcp;
export using boost::asio::ip::make_address;
export using boost::asio::ip::address;
} // namespace ip
} // namespace asio
} // namespace boost

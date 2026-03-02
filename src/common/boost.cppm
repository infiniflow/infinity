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

#define BOOST_NO_AUTO_PTR ;

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/bind/bind.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/containers_fwd.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/managed_mapped_file.hpp>
#include <boost/interprocess/offset_ptr.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_sharable_mutex.hpp>
#include <boost/thread.hpp>

export module infinity_core:boost;

export namespace boost {
using boost::bind;
using boost::dynamic_bitset;
using boost::shared_mutex;
using boost::unique_lock;
using boost::upgrade_lock;
using boost::upgrade_to_unique_lock;

namespace system {
using boost::system::error_code;
}

namespace asio {
using boost::asio::buffer;
using boost::asio::io_context;
using boost::asio::mutable_buffer;
using boost::asio::read;
using boost::asio::transfer_at_least;
using boost::asio::write;
namespace ip {
using boost::asio::ip::address;
using boost::asio::ip::make_address;
using boost::asio::ip::tcp;
} // namespace ip
namespace error {
using boost::asio::error::broken_pipe;
using boost::asio::error::connection_reset;
} // namespace error
} // namespace asio

namespace interprocess {
using boost::interprocess::offset_ptr;

using boost::interprocess::allocator;
using boost::interprocess::vector;
// export using boost::interprocess::map;

using boost::interprocess::file_mapping;
using boost::interprocess::managed_mapped_file;

using boost::interprocess::interprocess_mutex;
using boost::interprocess::interprocess_sharable_mutex;

inline constexpr auto create_only_infinity = create_only;
inline constexpr auto open_copy_on_write_infinity = open_copy_on_write;
inline constexpr auto open_only_infinity = open_only;
inline constexpr auto open_or_create_infinity = open_or_create;
inline constexpr auto open_read_only_infinity = open_read_only;

} // namespace interprocess

namespace container {
using boost::container::basic_string;
} // namespace container

} // namespace boost

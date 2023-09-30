//
// Created by JinHai on 2022/7/31.
//

#include "config.h"
#include "bin/compilation_config.h"
#include "toml.hpp"
#include "spdlog/common.h"
#include "common/types/alias/containers.h"
#include <iostream>
#include <charconv>
#include <boost/asio/ip/impl/address.ipp>

namespace infinity {

extern SharedPtr<spdlog::logger> infinity_logger;

SharedPtr<String>
Config::Init(const SharedPtr<String>& config_path) {
    SharedPtr<String> result;

    // Default general config
    String default_version = std::to_string(VERSION_MAJOR) + '.'
                             + std::to_string(VERSION_MINOR) + '.'
                             + std::to_string(VERSION_PATCH);

    String default_time_zone = "UTC";
    i32 default_time_zone_bias = 8;

    // Default system config
    u64 default_total_cpu_number = std::thread::hardware_concurrency();
    u64 default_total_memory_size = GetAvailableMem();
    u64 default_query_cpu_limit = default_total_cpu_number;
    u64 default_query_memory_limit = default_total_memory_size;

    // Default network config
    String default_listen_address = "0.0.0.0";
    u32 default_pg_port = 5432;
    u32 default_http_port = 8088;
    u32 default_sdk_port = 23817;

    // Default log config
    SharedPtr<String> default_log_filename = MakeShared<String>("infinity.log");
    SharedPtr<String> default_log_dir = MakeShared<String>("/tmp/infinity/log");
    bool default_log_to_stdout = false;
    u64 default_log_max_size = 1024lu * 1024lu * 1024lu; // 1Gib
    u64 default_log_file_rotate_count = 10;
    spdlog::level::level_enum default_log_level = spdlog::level::level_enum::trace;

    // Default storage config
    SharedPtr<String> default_data_dir = MakeShared<String>("/tmp/infinity/data");
    SharedPtr<String> default_wal_dir = MakeShared<String>("/tmp/infinity/wal");
    u64 default_row_size = 8192lu;

    // Default buffer config
    u64 default_buffer_pool_size = 4 * 1024lu * 1024lu * 1024lu; // 4Gib
    SharedPtr<String> default_temp_dir = MakeShared<String>("/tmp/infinity/temp");

    if(config_path == nullptr) {
        std::cout << "No config file is given, use default configs." << std::endl;

        // General
        {
            option_.version = default_version;
            option_.time_zone = default_time_zone;
            option_.time_zone_bias = default_time_zone_bias;
        }

        // System
        {
            option_.total_cpu_number = default_total_cpu_number;
            option_.total_memory_size = default_total_memory_size;
            option_.query_cpu_limit = default_query_cpu_limit;
            option_.query_memory_limit = default_query_memory_limit;
        }

        // Network
        {
            option_.listen_address = default_listen_address;
            option_.pg_port = default_pg_port;
            option_.http_port = default_http_port;
            option_.sdk_port = default_sdk_port;
        }

        // Log
        {
            option_.log_filename = default_log_filename;
            option_.log_dir = default_log_dir;
            option_.log_file_path = MakeShared<String>(*option_.log_dir + '/' + *option_.log_filename);
            option_.log_to_stdout = default_log_to_stdout;
            option_.log_max_size = default_log_max_size; // 1Gib
            option_.log_file_rotate_count = default_log_file_rotate_count;
            option_.log_level = default_log_level;
        }

        // Storage
        {
            option_.data_dir = MakeShared<String>(*default_data_dir);
            option_.wal_dir = MakeShared<String>(*default_wal_dir);
            option_.default_row_size = default_row_size;
        }

        // Buffer
        {
            option_.buffer_pool_size = default_buffer_pool_size; // 4Gib
            option_.temp_dir = MakeShared<String>(*default_temp_dir);
        }

    } else {
        std::cout << "Read config from: " << *config_path << std::endl;
        auto config = toml::parse_file(*config_path);

        // General
        {
            auto general_config = config["general"];

            String infinity_version = general_config["version"].value_or("invalid");
            if(default_version != infinity_version) {
                return MakeShared<String>("Unmatched version in config file.");
            }
            option_.version = infinity_version;

            String time_zone_str = general_config["timezone"].value_or("invalid");
            if(time_zone_str == "invalid") {
                result = MakeShared<String>("Timezone isn't given in config file.");
                return result;
            }

            try {
                ParseTimeZoneStr(time_zone_str, option_.time_zone, option_.time_zone_bias);
            } catch(...) {
                result = MakeShared<String>(fmt::format("Timezone can't be recognized: {}", time_zone_str));
                return result;
            }
        }

        // System
        {
            auto system_config = config["system"];
            option_.total_cpu_number = system_config["total_cpu_number"].value_or(default_total_cpu_number);

            String total_memory_size_str = system_config["total_memory_size"].value_or("8GB");
            result = ParseByteSize(total_memory_size_str, option_.total_memory_size);
            if(result != nullptr) {
                return result;
            }
            if(option_.total_memory_size > default_total_memory_size) {
                option_.total_memory_size = default_total_memory_size;
            }

            option_.query_cpu_limit = system_config["query_cpu_limit"].value_or(default_query_cpu_limit);
            option_.query_memory_limit = system_config["query_memory_limit"].value_or(default_query_memory_limit);

            String query_memory_limit_str = system_config["query_memory_limit"].value_or("4MB");
            result = ParseByteSize(query_memory_limit_str, option_.query_memory_limit);
            if(result != nullptr) {
                return result;
            }
            if(option_.query_memory_limit > default_query_memory_limit) {
                option_.query_memory_limit = default_query_memory_limit;
            }
        }

        // Network
        {
            auto network_config = config["network"];
            option_.listen_address = network_config["listen_address"].value_or(default_listen_address);

            // Validate the address format
            boost::system::error_code error;
            boost::asio::ip::make_address(option_.listen_address, error);
            if(error) {
                String err_msg = fmt::format("Not a valid IPv4 address: {}", option_.listen_address);
                result = MakeShared<String>(err_msg);
                return result;
            }

            option_.pg_port = network_config["pg_port"].value_or(default_pg_port);
            option_.http_port = network_config["http_port"].value_or(default_http_port);
            option_.sdk_port = network_config["sdk_port"].value_or(default_sdk_port);
        }

        // Log
        {
            auto log_config = config["log"];
            option_.log_filename = MakeShared<String>(log_config["log_filename"].value_or(*default_log_filename));
            option_.log_dir = MakeShared<String>(log_config["log_dir"].value_or(*default_log_dir));
            option_.log_file_path = MakeShared<String>(*option_.log_dir + '/' + *option_.log_filename);
            option_.log_to_stdout = log_config["log_to_stdout"].value_or(default_log_to_stdout);

            String log_max_size_str = log_config["log_max_size"].value_or("1GB");
            result = ParseByteSize(log_max_size_str, option_.log_max_size);
            if(result != nullptr) {
                return result;
            }

            option_.log_file_rotate_count = log_config["log_file_rotate_count"].value_or(default_log_file_rotate_count);

            String log_level = log_config["log_level"].value_or("invalid");
            if(log_level == "trace") {
                option_.log_level = spdlog::level::level_enum::trace;
            } else if(log_level == "info") {
                option_.log_level = spdlog::level::level_enum::info;
            } else if(log_level == "warning") {
                option_.log_level = spdlog::level::level_enum::warn;
            } else if(log_level == "error") {
                option_.log_level = spdlog::level::level_enum::err;
            } else if(log_level == "critical") {
                option_.log_level = spdlog::level::level_enum::critical;
            } else {
                result = MakeShared<String>("Invalid log level in config file");
                return result;
            }
        }

        // Storage
        {
            auto storage_config = config["storage"];
            option_.data_dir = MakeShared<String>(storage_config["data_dir"].value_or(*default_data_dir));
            option_.wal_dir = MakeShared<String>(storage_config["wal_dir"].value_or(*default_wal_dir));
            option_.default_row_size = storage_config["default_row_size"].value_or(default_row_size);
        }

        // Buffer
        {
            auto buffer_config = config["buffer"];
            String buffer_pool_size_str = buffer_config["buffer_pool_size"].value_or("4GB");
            result = ParseByteSize(buffer_pool_size_str, option_.buffer_pool_size);
            if(result != nullptr) {
                return result;
            }
            option_.temp_dir = MakeShared<String>(buffer_config["temp_dir"].value_or("invalid"));
        }
    }

    return result;
}

void
Config::PrintAll() const {
    if(infinity_logger != nullptr) {
        infinity::infinity_logger->info("Infinity system parameters: ");

        // General
        infinity::infinity_logger->info(" - version: " + option_.version);
        infinity::infinity_logger->info(" - timezone: " + option_.time_zone + std::to_string(option_.time_zone_bias));

        // System
        infinity::infinity_logger->info(" - total_cpu_number: " + std::to_string(option_.total_cpu_number));
        infinity::infinity_logger->info(" - total_memory_size: " + std::to_string(option_.total_memory_size));
        infinity::infinity_logger->info(" - query_cpu_limit: " + std::to_string(option_.query_cpu_limit));
        infinity::infinity_logger->info(" - query_memory_limit: " + std::to_string(option_.query_memory_limit));

        // Network
        infinity::infinity_logger->info(" - listen address: " + option_.listen_address);
        infinity::infinity_logger->info(" - postgres port: " + std::to_string(option_.pg_port));
        infinity::infinity_logger->info(" - http port: " + std::to_string(option_.http_port));
        infinity::infinity_logger->info(" - sdk port: " + std::to_string(option_.sdk_port));

        // Log
        infinity::infinity_logger->info(" - log_file_path: " + *option_.log_file_path);
        infinity::infinity_logger->info(" - log_to_stdout: " + std::to_string(option_.log_to_stdout));
        infinity::infinity_logger->info(" - log_max_size: " + std::to_string(option_.log_max_size));
        infinity::infinity_logger->info(" - log_file_rotate_count: " + std::to_string(option_.log_file_rotate_count));
        infinity::infinity_logger->info(" - log_level: " + String(spdlog::level::to_short_c_str(option_.log_level)));

        //Storage
        infinity::infinity_logger->info(" - data_dir: " + *option_.data_dir);
        infinity::infinity_logger->info(" - wal_dir: " + *option_.wal_dir);
        infinity::infinity_logger->info(" - default_row_size: " + std::to_string(option_.default_row_size));

        // Buffer
        infinity::infinity_logger->info(" - buffer_pool_size: " + std::to_string(option_.buffer_pool_size));
        infinity::infinity_logger->info(" - temp_dir: " + *option_.temp_dir);
    }
}

void
Config::ParseTimeZoneStr(const String& time_zone_str, String& parsed_time_zone, i32& parsed_time_zone_bias) {
    parsed_time_zone = time_zone_str.substr(0, 3);
    std::transform(parsed_time_zone.begin(), parsed_time_zone.end(), parsed_time_zone.begin(), ::toupper);
    parsed_time_zone_bias = std::stoi(time_zone_str.substr(3, std::string::npos));
}

SharedPtr<String>
Config::ParseByteSize(const String& byte_size_str, u64& byte_size) {

    HashMap<String, u64> byte_unit = {
            {"kb", 1024ul},
            {"mb", 1024ul * 1024ul},
            {"gb", 1024ul * 1024ul * 1024ul}
    };
    if(byte_size_str.empty()) {
        return MakeShared<String>("No byte size is given");;
    }

    u64 factor;
    auto [ptr, error_code] = std::from_chars(byte_size_str.data(),
                                             byte_size_str.data() + byte_size_str.size(),
                                             factor);
    if(error_code == std::errc()) {
        String unit = ptr;
        std::transform(unit.begin(), unit.end(), unit.begin(), ::tolower);
        auto it = byte_unit.find(unit);
        if(it != byte_unit.end()) {
            byte_size = factor * it->second;
            return nullptr;
        } else {
            return MakeShared<String>("Unrecognized byte size");
        }
    } else {
        return MakeShared<String>("Unrecognized byte size");
    }
}

u64
Config::GetAvailableMem() {
    u64 pages = sysconf(_SC_PHYS_PAGES);
    u64 page_size = sysconf(_SC_PAGE_SIZE); // Byte
    return pages * page_size;
}


}

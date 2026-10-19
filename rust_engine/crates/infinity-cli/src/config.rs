//! Engine configuration (`Config`).
//!
//! C++: `Config` (`src/main/config.cppm`). Defaults and the TOML overrides used
//! by `src/unit_test/main/config_ut.cpp`; validation returns the reference's
//! error categories (version, time zone, IP, byte size, log level, time info).

#![allow(dead_code)]

use std::net::IpAddr;
use std::path::Path;

/// C++: `ErrorCode` subset used by `Config::Init`.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum ConfigError {
    InvalidConfig,
    MismatchVersion,
    InvalidTimezone,
    InvalidIpAddr,
    InvalidByteSize,
    InvalidLogLevel,
    InvalidTimeInfo,
}

/// C++: `LogLevel`.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
pub enum LogLevel {
    Trace,
    Debug,
    #[default]
    Info,
    Warning,
    Error,
    Critical,
}

/// C++: `StorageType`.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
pub enum StorageType {
    #[default]
    Local,
    Minio,
}

/// C++: `Config`.
#[derive(Debug, Clone)]
pub struct Config {
    version: String,
    time_zone: String,
    time_zone_bias: i32,
    cpu_limit: u32,

    server_address: String,
    postgres_port: u16,
    http_port: u16,
    client_port: u16,
    connection_pool_size: u32,
    peer_retry_delay: u64,
    peer_retry_count: u64,
    peer_connect_timeout: u64,
    peer_recv_timeout: u64,
    peer_send_timeout: u64,

    log_filename: String,
    log_dir: String,
    log_to_stdout: bool,
    log_file_max_size: i64,
    log_file_rotate_count: u32,
    log_level: LogLevel,

    data_dir: String,
    wal_dir: String,
    storage_type: StorageType,
    object_storage_url: String,
    object_storage_bucket: String,
    object_storage_access_key: String,
    object_storage_secret_key: String,
    object_storage_https: bool,
    optimize_interval: u64,
    cleanup_interval: u64,
    compact_interval: u64,
    mem_index_capacity: u64,
    snapshot_dir: String,

    buffer_manager_size: i64,
    lru_num: usize,
    temp_dir: String,
    mem_index_memory_quota: i64,

    checkpoint_interval: u64,
    wal_compact_threshold: i64,
    wal_flush: String,

    result_cache: String,
    cache_result_capacity: u64,
    resource_dir: String,
    persistence_dir: String,
}

impl Default for Config {
    /// C++: `Config` defaults (`config_ut.cpp::test1`).
    fn default() -> Self {
        Self {
            version: "0.7.4".into(),
            time_zone: "UTC".into(),
            time_zone_bias: 8,
            cpu_limit: std::thread::available_parallelism()
                .map(|n| n.get() as u32)
                .unwrap_or(1),
            server_address: "0.0.0.0".into(),
            postgres_port: 5432,
            http_port: 23820,
            client_port: 23817,
            connection_pool_size: 128,
            peer_retry_delay: 1000,
            peer_retry_count: 2,
            peer_connect_timeout: 2000,
            peer_recv_timeout: 0,
            peer_send_timeout: 0,
            log_filename: "infinity.log".into(),
            log_dir: "/var/infinity/log".into(),
            log_to_stdout: true,
            log_file_max_size: 1024 * 1024 * 1024,
            log_file_rotate_count: 8,
            log_level: LogLevel::Info,
            data_dir: "/var/infinity/data".into(),
            wal_dir: "/var/infinity/wal".into(),
            storage_type: StorageType::Local,
            object_storage_url: "0.0.0.0:9000".into(),
            object_storage_bucket: "infinity".into(),
            object_storage_access_key: "minioadmin".into(),
            object_storage_secret_key: "minioadmin".into(),
            object_storage_https: false,
            optimize_interval: 10,
            cleanup_interval: 60,
            compact_interval: 120,
            mem_index_capacity: 65536,
            snapshot_dir: "/var/infinity/snapshot".into(),
            buffer_manager_size: 8 * 1024 * 1024 * 1024,
            lru_num: 7,
            temp_dir: "/var/infinity/tmp".into(),
            mem_index_memory_quota: 4 * 1024 * 1024 * 1024,
            checkpoint_interval: 86400,
            wal_compact_threshold: 1024 * 1024 * 1024,
            wal_flush: "only_write".into(),
            result_cache: "off".into(),
            cache_result_capacity: 10000,
            resource_dir: "/usr/share/infinity/resource".into(),
            persistence_dir: "/var/infinity/persistence".into(),
        }
    }
}

impl Config {
    /// C++: `Config::Init(path, nullptr)`.
    pub fn init(&mut self, path: Option<&Path>) -> Result<(), ConfigError> {
        if let Some(path) = path {
            let text = std::fs::read_to_string(path).map_err(|_| ConfigError::InvalidConfig)?;
            self.apply_toml(&text)?;
        }
        self.validate()
    }

    fn apply_toml(&mut self, text: &str) -> Result<(), ConfigError> {
        let mut section = String::new();
        for raw in text.lines() {
            let line = raw.split('#').next().unwrap_or("").trim();
            if line.is_empty() {
                continue;
            }
            if let Some(name) = line.strip_prefix('[').and_then(|l| l.strip_suffix(']')) {
                section = name.trim().to_ascii_lowercase();
                if !matches!(
                    section.as_str(),
                    "general"
                        | "network"
                        | "log"
                        | "storage"
                        | "storage.object_storage"
                        | "buffer"
                        | "wal"
                        | "resource"
                ) {
                    return Err(ConfigError::InvalidConfig);
                }
                continue;
            }
            let (key, value) = line.split_once('=').ok_or(ConfigError::InvalidConfig)?;
            let key = key.trim().to_ascii_lowercase();
            let value = value.trim();
            self.set(&section, &key, value)?;
        }
        Ok(())
    }

    fn set(&mut self, section: &str, key: &str, raw: &str) -> Result<(), ConfigError> {
        let string = || raw.trim_matches('"').to_string();
        let integer = || raw.parse::<i64>().map_err(|_| ConfigError::InvalidConfig);
        let boolean = || match raw {
            "true" => Ok(true),
            "false" => Ok(false),
            _ => Err(ConfigError::InvalidConfig),
        };
        match (section, key) {
            ("general", "version") => self.version = string(),
            ("general", "time_zone") => {
                let (name, bias) = parse_time_zone(&string())?;
                self.time_zone = name;
                self.time_zone_bias = bias;
            }
            ("general", "cpu_limit") => self.cpu_limit = integer()? as u32,
            ("network", "server_address") => self.server_address = string(),
            ("network", "postgres_port") => self.postgres_port = integer()? as u16,
            ("network", "http_port") => self.http_port = integer()? as u16,
            ("network", "client_port") => self.client_port = integer()? as u16,
            ("network", "connection_pool_size") => self.connection_pool_size = integer()? as u32,
            ("network", "peer_retry_delay") => self.peer_retry_delay = integer()? as u64,
            ("network", "peer_retry_count") => self.peer_retry_count = integer()? as u64,
            ("network", "peer_connect_timeout") => self.peer_connect_timeout = integer()? as u64,
            ("network", "peer_recv_timeout") => self.peer_recv_timeout = integer()? as u64,
            ("network", "peer_send_timeout") => self.peer_send_timeout = integer()? as u64,
            ("log", "log_filename") => self.log_filename = string(),
            ("log", "log_dir") => self.log_dir = string(),
            ("log", "log_to_stdout") => self.log_to_stdout = boolean()?,
            ("log", "log_file_max_size") => self.log_file_max_size = parse_byte_size(&string())?,
            ("log", "log_file_rotate_count") => self.log_file_rotate_count = integer()? as u32,
            ("log", "log_level") => self.log_level = parse_log_level(&string())?,
            ("storage", "data_dir") => self.data_dir = string(),
            ("storage", "persistence_dir") => self.persistence_dir = string(),
            ("storage", "optimize_interval") => {
                self.optimize_interval = parse_time_info(&string())?
            }
            ("storage", "cleanup_interval") => self.cleanup_interval = parse_time_info(&string())?,
            ("storage", "compact_interval") => self.compact_interval = parse_time_info(&string())?,
            ("storage", "mem_index_capacity") => self.mem_index_capacity = integer()? as u64,
            ("storage", "snapshot_dir") => self.snapshot_dir = string(),
            ("storage", "storage_type") => {
                self.storage_type = match string().to_ascii_lowercase().as_str() {
                    "local" => StorageType::Local,
                    "minio" => StorageType::Minio,
                    _ => return Err(ConfigError::InvalidConfig),
                }
            }
            ("storage.object_storage", "url") => self.object_storage_url = string(),
            ("storage.object_storage", "bucket_name") => self.object_storage_bucket = string(),
            ("storage.object_storage", "access_key") => self.object_storage_access_key = string(),
            ("storage.object_storage", "secret_key") => self.object_storage_secret_key = string(),
            ("storage.object_storage", "enable_https") => self.object_storage_https = boolean()?,
            ("buffer", "buffer_manager_size") => {
                self.buffer_manager_size = parse_byte_size(&string())?
            }
            ("buffer", "lru_num") => self.lru_num = integer()? as usize,
            ("buffer", "temp_dir") => self.temp_dir = string(),
            ("buffer", "memindex_memory_quota") => {
                self.mem_index_memory_quota = parse_byte_size(&string())?
            }
            ("buffer", "result_cache") => self.result_cache = string(),
            ("buffer", "cache_result_capacity") => self.cache_result_capacity = integer()? as u64,
            ("wal", "wal_dir") => self.wal_dir = string(),
            ("wal", "checkpoint_interval") => {
                self.checkpoint_interval = parse_time_info(&string())?
            }
            ("wal", "wal_compact_threshold") => {
                self.wal_compact_threshold = parse_byte_size(&string())?
            }
            ("wal", "wal_flush") => {
                let value = string();
                if !matches!(value.as_str(), "only_write" | "flush" | "append") {
                    return Err(ConfigError::InvalidConfig);
                }
                self.wal_flush = value;
            }
            ("resource", "resource_dir") => self.resource_dir = string(),
            _ => return Err(ConfigError::InvalidConfig),
        }
        Ok(())
    }

    fn validate(&self) -> Result<(), ConfigError> {
        if self.version != "0.7.4" {
            return Err(ConfigError::MismatchVersion);
        }
        if self.server_address.parse::<IpAddr>().is_err() {
            return Err(ConfigError::InvalidIpAddr);
        }
        if self.connection_pool_size > u16::MAX as u32 {
            return Err(ConfigError::InvalidConfig);
        }
        // `log_file_max_size` must cover at least 1 MiB; `ParseByteSize`
        // rejects the unsupported TB unit.
        if self.log_file_max_size < 1024 * 1024 {
            return Err(ConfigError::InvalidConfig);
        }
        if self.optimize_interval > 720 * 3600 {
            return Err(ConfigError::InvalidConfig);
        }
        Ok(())
    }

    // C++ accessors below (`Config::Version()` …).
    pub fn version(&self) -> &str {
        &self.version
    }
    pub fn time_zone(&self) -> &str {
        &self.time_zone
    }
    pub fn time_zone_bias(&self) -> i32 {
        self.time_zone_bias
    }
    pub fn cpu_limit(&self) -> u32 {
        self.cpu_limit
    }
    pub fn server_address(&self) -> &str {
        &self.server_address
    }
    pub fn postgres_port(&self) -> u16 {
        self.postgres_port
    }
    pub fn http_port(&self) -> u16 {
        self.http_port
    }
    pub fn client_port(&self) -> u16 {
        self.client_port
    }
    pub fn connection_pool_size(&self) -> u32 {
        self.connection_pool_size
    }
    pub fn peer_retry_delay(&self) -> u64 {
        self.peer_retry_delay
    }
    pub fn peer_retry_count(&self) -> u64 {
        self.peer_retry_count
    }
    pub fn peer_connect_timeout(&self) -> u64 {
        self.peer_connect_timeout
    }
    pub fn peer_recv_timeout(&self) -> u64 {
        self.peer_recv_timeout
    }
    pub fn peer_send_timeout(&self) -> u64 {
        self.peer_send_timeout
    }
    pub fn log_file_name(&self) -> &str {
        &self.log_filename
    }
    pub fn log_dir(&self) -> &str {
        &self.log_dir
    }
    pub fn log_file_path(&self) -> String {
        format!("{}/{}", self.log_dir, self.log_filename)
    }
    pub fn log_to_stdout(&self) -> bool {
        self.log_to_stdout
    }
    pub fn log_file_max_size(&self) -> i64 {
        self.log_file_max_size
    }
    pub fn log_file_rotate_count(&self) -> u32 {
        self.log_file_rotate_count
    }
    pub fn log_level(&self) -> LogLevel {
        self.log_level
    }
    pub fn data_dir(&self) -> &str {
        &self.data_dir
    }
    pub fn wal_dir(&self) -> &str {
        &self.wal_dir
    }
    pub fn storage_type(&self) -> StorageType {
        self.storage_type
    }
    pub fn object_storage_url(&self) -> &str {
        &self.object_storage_url
    }
    pub fn object_storage_bucket(&self) -> &str {
        &self.object_storage_bucket
    }
    pub fn object_storage_access_key(&self) -> &str {
        &self.object_storage_access_key
    }
    pub fn object_storage_secret_key(&self) -> &str {
        &self.object_storage_secret_key
    }
    pub fn object_storage_https(&self) -> bool {
        self.object_storage_https
    }
    pub fn optimize_index_interval(&self) -> u64 {
        self.optimize_interval
    }
    pub fn cleanup_interval(&self) -> u64 {
        self.cleanup_interval
    }
    pub fn compact_interval(&self) -> u64 {
        self.compact_interval
    }
    pub fn mem_index_capacity(&self) -> u64 {
        self.mem_index_capacity
    }
    pub fn snapshot_dir(&self) -> &str {
        &self.snapshot_dir
    }
    pub fn buffer_manager_size(&self) -> i64 {
        self.buffer_manager_size
    }
    pub fn lru_num(&self) -> usize {
        self.lru_num
    }
    pub fn temp_dir(&self) -> &str {
        &self.temp_dir
    }
    pub fn mem_index_memory_quota(&self) -> i64 {
        self.mem_index_memory_quota
    }
    pub fn checkpoint_interval(&self) -> u64 {
        self.checkpoint_interval
    }
    pub fn wal_compact_threshold(&self) -> i64 {
        self.wal_compact_threshold
    }
    pub fn result_cache(&self) -> &str {
        &self.result_cache
    }
    pub fn cache_result_num(&self) -> u64 {
        self.cache_result_capacity
    }
    pub fn resource_path(&self) -> &str {
        &self.resource_dir
    }
    pub fn persistence_dir(&self) -> &str {
        &self.persistence_dir
    }
}

/// C++: `Config::ParseTimeZoneStr`: `utc±N` → canonical name and hour bias.
fn parse_time_zone(text: &str) -> Result<(String, i32), ConfigError> {
    let lower = text.trim().to_ascii_lowercase();
    if lower == "utc" || lower == "gmt" {
        return Ok(("UTC".to_string(), 0));
    }
    let rest = lower
        .strip_prefix("utc")
        .or_else(|| lower.strip_prefix("gmt"))
        .ok_or(ConfigError::InvalidTimezone)?;
    let bias: i32 = rest.parse().map_err(|_| ConfigError::InvalidTimezone)?;
    if bias.abs() > 14 {
        return Err(ConfigError::InvalidConfig);
    }
    Ok(("UTC".to_string(), bias))
}

/// C++: `Config::ParseByteSize`: `KB`/`MB`/`GB` suffixes; `TB` is rejected.
fn parse_byte_size(text: &str) -> Result<i64, ConfigError> {
    let lower = text.trim().to_ascii_lowercase();
    let (number, factor) = if let Some(v) = lower.strip_suffix("kb") {
        (v, 1024i64)
    } else if let Some(v) = lower.strip_suffix("mb") {
        (v, 1024 * 1024)
    } else if let Some(v) = lower.strip_suffix("gb") {
        (v, 1024 * 1024 * 1024)
    } else if lower.ends_with("tb") {
        return Err(ConfigError::InvalidByteSize);
    } else {
        (lower.as_str(), 1)
    };
    let number: i64 = number.parse().map_err(|_| ConfigError::InvalidByteSize)?;
    number
        .checked_mul(factor)
        .ok_or(ConfigError::InvalidByteSize)
}

/// C++: `Config::ParseTimeInfo`: `s`/`m`/`h` suffixes.
fn parse_time_info(text: &str) -> Result<u64, ConfigError> {
    let lower = text.trim().to_ascii_lowercase();
    let (number, factor) = if let Some(v) = lower.strip_suffix('s') {
        (v, 1u64)
    } else if let Some(v) = lower.strip_suffix('m') {
        (v, 60)
    } else if let Some(v) = lower.strip_suffix('h') {
        (v, 3600)
    } else {
        return Err(ConfigError::InvalidTimeInfo);
    };
    let number: u64 = number.parse().map_err(|_| ConfigError::InvalidTimeInfo)?;
    number
        .checked_mul(factor)
        .ok_or(ConfigError::InvalidTimeInfo)
}

/// C++: `LogLevel` parsing.
fn parse_log_level(text: &str) -> Result<LogLevel, ConfigError> {
    match text.trim().to_ascii_lowercase().as_str() {
        "trace" => Ok(LogLevel::Trace),
        "debug" => Ok(LogLevel::Debug),
        "info" => Ok(LogLevel::Info),
        "warning" => Ok(LogLevel::Warning),
        "error" => Ok(LogLevel::Error),
        "critical" => Ok(LogLevel::Critical),
        _ => Err(ConfigError::InvalidLogLevel),
    }
}

#[cfg(test)]
mod tests {
    //! Ports of `ConfigTest` (`src/unit_test/main/config_ut.cpp`).

    use super::*;

    fn fixture(name: &str) -> std::path::PathBuf {
        std::path::Path::new(env!("CARGO_MANIFEST_DIR"))
            .join("../../../test/data/config")
            .join(name)
    }

    /// C++: `ConfigTest.test1`.
    #[test]
    fn defaults() {
        let mut config = Config::default();
        config.init(None).unwrap();
        assert_eq!(config.version(), "0.7.4");
        assert_eq!(config.time_zone(), "UTC");
        assert_eq!(config.time_zone_bias(), 8);
        assert_eq!(
            config.cpu_limit(),
            std::thread::available_parallelism().unwrap().get() as u32
        );
        assert_eq!(config.server_address(), "0.0.0.0");
        assert_eq!(config.postgres_port(), 5432);
        assert_eq!(config.http_port(), 23820);
        assert_eq!(config.client_port(), 23817);
        assert_eq!(config.peer_retry_delay(), 1000);
        assert_eq!(config.peer_retry_count(), 2);
        assert_eq!(config.peer_connect_timeout(), 2000);
        assert_eq!(config.peer_recv_timeout(), 0);
        assert_eq!(config.peer_send_timeout(), 0);
        assert_eq!(config.log_file_name(), "infinity.log");
        assert_eq!(config.log_dir(), "/var/infinity/log");
        assert_eq!(config.log_file_path(), "/var/infinity/log/infinity.log");
        assert!(config.log_to_stdout());
        assert_eq!(config.log_file_max_size(), 1024 * 1024 * 1024);
        assert_eq!(config.log_file_rotate_count(), 8);
        assert_eq!(config.log_level(), LogLevel::Info);
        assert_eq!(config.data_dir(), "/var/infinity/data");
        assert_eq!(config.wal_dir(), "/var/infinity/wal");
        assert_eq!(config.storage_type(), StorageType::Local);
        assert_eq!(config.buffer_manager_size(), 8 * 1024 * 1024 * 1024);
        assert_eq!(config.lru_num(), 7);
        assert_eq!(config.temp_dir(), "/var/infinity/tmp");
        assert_eq!(config.mem_index_memory_quota(), 4 * 1024 * 1024 * 1024);
        assert_eq!(config.result_cache(), "off");
        assert_eq!(config.cache_result_num(), 10000);
    }

    /// C++: `ConfigTest.test2`.
    #[test]
    fn full_config_file() {
        let mut config = Config::default();
        config.init(Some(&fixture("infinity_conf.toml"))).unwrap();
        assert_eq!(config.version(), "0.7.4");
        assert_eq!(config.time_zone(), "UTC");
        assert_eq!(config.time_zone_bias(), -8);
        assert_eq!(config.cpu_limit(), 2);
        assert_eq!(config.server_address(), "127.0.0.1");
        assert_eq!(config.postgres_port(), 25432);
        assert_eq!(config.http_port(), 24821);
        assert_eq!(config.client_port(), 24817);
        assert_eq!(config.peer_retry_delay(), 1000);
        assert_eq!(config.peer_retry_count(), 2);
        assert_eq!(config.peer_connect_timeout(), 2000);
        assert_eq!(config.peer_recv_timeout(), 2000);
        assert_eq!(config.peer_send_timeout(), 2000);
        assert!(!config.log_to_stdout());
        assert_eq!(config.log_file_max_size(), 2 * 1024 * 1024 * 1024);
        assert_eq!(config.log_file_rotate_count(), 3);
        assert_eq!(config.log_level(), LogLevel::Trace);
        assert_eq!(config.object_storage_url(), "0.0.0.0:9000");
        assert_eq!(config.object_storage_bucket(), "infinity");
        assert_eq!(config.object_storage_access_key(), "minioadmin");
        assert_eq!(config.object_storage_secret_key(), "minioadmin");
        assert!(!config.object_storage_https());
        assert_eq!(config.buffer_manager_size(), 3 * 1024 * 1024 * 1024);
        assert_eq!(config.lru_num(), 8);
        assert_eq!(config.mem_index_memory_quota(), 2 * 1024 * 1024 * 1024);
        assert_eq!(config.result_cache(), "off");
    }

    /// C++: `ConfigTest.TestWrongParamNames`.
    #[test]
    fn wrong_parameter_names() {
        let mut config = Config::default();
        assert_eq!(
            config.init(Some(&fixture("test_conf_invalid_param.toml"))),
            Err(ConfigError::InvalidConfig)
        );
    }

    /// C++: `ConfigTest.TestConfInvalidValues`.
    #[test]
    fn invalid_values() {
        for (name, error) in [
            (
                "test_conf_invalid_version.toml",
                ConfigError::MismatchVersion,
            ),
            (
                "test_conf_invalid_timezone.toml",
                ConfigError::InvalidTimezone,
            ),
            (
                "test_conf_invalid_server_address.toml",
                ConfigError::InvalidIpAddr,
            ),
            (
                "test_conf_invalid_bytesize.toml",
                ConfigError::InvalidByteSize,
            ),
            (
                "test_conf_invalid_log_level.toml",
                ConfigError::InvalidLogLevel,
            ),
            (
                "test_conf_invalid_timeinfo.toml",
                ConfigError::InvalidTimeInfo,
            ),
        ] {
            let mut config = Config::default();
            assert_eq!(config.init(Some(&fixture(name))), Err(error), "{name}");
        }
    }

    /// C++: `ConfigTest.TestOutofRangeValues`.
    #[test]
    fn out_of_range_values() {
        for name in [
            "test_conf_out_of_bound_bytesize.toml",
            "test_conf_out_of_bound_number.toml",
            "test_conf_out_of_bound_timeinfo.toml",
            "test_conf_out_of_bound_timezone.toml",
            "test_conf_out_of_bound_walflush.toml",
        ] {
            let mut config = Config::default();
            assert_eq!(
                config.init(Some(&fixture(name))),
                Err(ConfigError::InvalidConfig),
                "{name}"
            );
        }
    }

    /// C++: `ConfigTest.TestValidValues`.
    #[test]
    fn valid_values() {
        let mut config = Config::default();
        config
            .init(Some(&fixture("test_conf_valid_value.toml")))
            .unwrap();
        assert_eq!(config.version(), "0.7.4");
        assert_eq!(config.time_zone(), "UTC");
        assert_eq!(config.time_zone_bias(), -8);
        assert_eq!(config.cpu_limit(), 2);
        assert_eq!(config.server_address(), "0.0.0.0");
        assert_eq!(config.postgres_port(), 5432);
        assert_eq!(config.http_port(), 23820);
        assert_eq!(config.client_port(), 23817);
        assert_eq!(config.connection_pool_size(), 128);
        assert_eq!(config.peer_retry_delay(), 100);
        assert_eq!(config.peer_retry_count(), 1);
        assert_eq!(config.peer_connect_timeout(), 200);
        assert_eq!(config.peer_recv_timeout(), 200);
        assert_eq!(config.peer_send_timeout(), 200);
        assert_eq!(config.log_file_max_size(), 1024 * 1024 * 1024);
        assert_eq!(config.log_file_rotate_count(), 10);
        assert_eq!(config.log_level(), LogLevel::Info);
        assert_eq!(config.optimize_index_interval(), 10);
        assert_eq!(config.cleanup_interval(), 60);
        assert_eq!(config.compact_interval(), 120);
        assert_eq!(config.mem_index_capacity(), 1048576);
        assert_eq!(config.snapshot_dir(), "/var/infinity/snapshot");
        assert_eq!(config.buffer_manager_size(), 4 * 1024 * 1024 * 1024);
        assert_eq!(config.lru_num(), 7);
        assert_eq!(config.mem_index_memory_quota(), 1024 * 1024 * 1024);
        assert_eq!(config.checkpoint_interval(), 86400);
        assert_eq!(config.wal_compact_threshold(), 1024 * 1024 * 1024);
        assert_eq!(config.resource_path(), "/usr/share/infinity/resource");
        assert_eq!(config.persistence_dir(), "/var/infinity/persistence");
    }
}

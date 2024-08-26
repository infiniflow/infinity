---
sidebar_position: 5
slug: /configurations
---

# Configurations

If you need to change or customize the behavior of the Infinity runtime, you can use a configuration file. By default, Infinity uses the default configuration. To specify your own configuration file, you can use the `--config path/to/config.toml` option.

When Infinity starts up, it determines and loads each item in the configuration in a specific order, and merges these configurations from different sources into a single configuration used by Infinity.

## Configuration Loading Order and Priority

When Infinity starts up, configurations are loaded and determined in the following order:

1. Default values embedded in the code.

2. Values from the configuration file provided with --config PATH (if exists).

The loading methods listed above have increasing priority, so a higher-priority source will override values loaded from a lower-priority source. If the provided configuration file is not found, Infinity will display a warning at startup. The whole configuration is also displayed upon startup.

## Configuration file example


```toml
[general]
# version of Infinity Server, defaults to version in source code
version                  = "0.3.0"
# timezone
time_zone                = "utc-8"
# number of worker threads, defaults to concurrent threads supported by compiler implementation
# range : [1, 16384]
cpu_limit                = 8

# Network-related configuration
[network]
# IP address of infinity server
server_address           = "0.0.0.0"

# Range for ports : [1024, 65535]
# Port of postgres server
postgres_port            = 5432
# Port of HTTP server
http_port                = 23820
# Port of Python SDK server
client_port              = 23817
# the maximum number of connections, defaults to 256
# range : [1, 65536]
connection_pool_size     = 128

# Log related configurations
[log]
# filename of log files
log_filename             = "infinity.log"
# directory of log files
log_dir                  = "/var/infinity/log"
# controls if the log should be printed to standard output
# range : {true|false} 
log_to_stdout            = false
# The maximum size of a single log file, defaults to 1GB
# range : at least 1GB
log_file_max_size        = "10GB"
# The number of rotating log files, defaults to 8
# range : [1, 655636]
log_file_rotate_count    = 10
# The level of output log, defaults to info
# range : {trace/debug/info/warning/error/critical}
log_level               = "info"

# storage-related configuration
[storage]
# The directory of data files
data_dir                 = "/var/infinity/data"

# periodically activates garbage collection:
# use "number + unit of time" to specify intervals
# s means seconds, for example "60s", 60 seconds
# m means minutes, for example "60m", 60 minutes
# h means hours, for example "1h", 1 hour
# set to "0s" to disable corresponding periodic tasks

# interval between index optimization tasks
# range : ["10s", "720h"]
optimize_interval        = "10s"
# interval between cleanup tasks
# range : ["10s", "720h"]
cleanup_interval         = "60s"
# interval between compaction tasks
# range : ["10s", "720h"]
compact_interval         = "120s"
# The threshold for flushing a single in-memory index to disk,
# when the number of rows the index includes exceeds this threshold,
# the system performs a flush operation for that in-memory index.
# range : [8192, 8388608]
mem_index_capacity       = 1048576

# buffer manager-related configurations
[buffer]
# the size of memory that buffer manager holds, defaults to 8GB
buffer_manager_size      = "4GB"
# the number of LRU caches in buffer manager
# range : [1, 100]
lru_num                  = 7
# When the required memory exceeds the size of the free memory in the buffer manager,
# the buffer manager dumps some of the its in-use memory to a temporary storage path specified by temp_dir.
temp_dir                 = "/var/infinity/tmp"
# The global memory index threshold triggers a flush to disk.
# When the memory used by all existing in-memory indices in the system exceeds this threshold
# the system performs a flush operation for all in-memory indices.
memindex_memory_quota   = "1GB"

# WAL related configuration
[wal]
# directory of WAL files
wal_dir                       = "/var/infinity/wal"
# interval between periodically full checkpoints
# range : ["0s", "720h"]
full_checkpoint_interval      = "86400s"
# interval between periodically delta(incremental) checkpoints
# range : ["0s", "720h"]
delta_checkpoint_interval     = "60s"

# the size threshold of WAL files for triggering compaction
# after a WAL file grows bigger than this threshold, the system will perform a compaction to it
# range : ["1KB", "1024GB"]
wal_compact_threshold         = "1GB"

# controls how WAL manager flush its logs to the disk
# flush_at_once: write and flush log each commit
# only_write: write log, OS control when to flush the log, default
# flush_per_second: logs are written after each commit and flushed to disk per second.
wal_flush                     = "only_write"

[resource]
# The resource files used by Infinity (such as the dictionary files used by the analyzer)
resource_dir                  = "/var/infinity/resource"

[persistence]
# Persistent File Storage Path
# When set to "" or not provided, persistent manager is diabled, and files are stored directly on the local file system.
# When not empty, persistent manager is enabled. The generated files will be stored by the persistence manager on the path specified by persistence_dir.
# For example, if the user adds three files: A, B, and C.
# When persistence manager is disabled, the three files are stored on the local file system in their specified raw paths: A, B, C.
# When persistence manager is enabled, the persistence manager determines how to store these files on the specified persistent path.
# toggle persistence on to reduce possible file fragments in your local filesystem
persistence_dir          = "/var/infinity/persistence"
```
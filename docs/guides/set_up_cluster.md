---
sidebar_position: 2
slug: /set_up_cluster
---
# Set up an Infinity cluster

Architecture overview and user guide for Infinity cluster.

## Overview

An Infinity cluster consists of one leader node, up to four follower nodes, and an unlimited number of learner nodes:

- **Leader node**: Responsible for data writes and transaction submissions.
- **Follower node**: Responsible for synchronous log/WAL writes.
- **Learner node**: Responsible for asynchronous log/WAL writes.

As of v0.5.0, the supported shared storage is MinIO.


![infinity_cluster](https://github.com/user-attachments/assets/3e9abeed-1698-4741-8bdb-ba3b05c1d7a3)

### Architecture

Infinity employs a distributed architecture comprising one leader node, *N* follower nodes, and *M* learner nodes, where 0 &le; *N* &le; 4 and *M* is unrestricted. As illustrated in the diagram above, all nodes in the cluster use MinIO for persistent storage.

- **Leader node**: The node responsible for transaction processing and node status management. When a transaction occurs, the leader node transmits the logs to both follower and learner nodes. The leader node confirms the completion of the transaction only upon receiving messages confirming completion of log persistence from *all* follower nodes
- **Follower node**: The synchronous log/WAL write node for the leader. It acts as a backup for the leader node, maintaining strong consistency with its state. 
- **Learner node**: The *asynchronous* log/WAL write node for the leader. A learner also serves as a backup for the leader node. However, Its state may be behind that of the leader, as it is not required to maintain strong consistency with the leader, and the leader does not need to confirm whether all learner nodes have completed log persistence.

### Startup and communication processes

When started up in cluster mode, a node enters `ADMIN` state, but is not automatically assigned a role like leader, follower, or learner. You must call `ADMIN SET NODE ROLE` to assign it a role. Once a leader node starts, it does the following:

1. Reads logs from the local disk to determine the metadata and data to read from shared storage.
2. Waits for connections from follower or learner nodes.

Once you set a node to follower or learner, it registers with the leader node as per the IP address specified in `ADMIN SET NODE ROLE`. Upon receiving the registration request, the leader node sends back its current log and snapshot locations for the followers and learners to restore their database state from shared storage.

### Keep-alive mechanism

Once successfully registered with the leader node, a follower or learner starts sending periodic heartbeats to it. The leader node relies on these heartbeats to manage the online status of each node. For example, if it does not receive heartbeats from a particular node for a specified time period, it sets that node's online status to `timeout`.

### Log synchronization

When a transaction occurs, the leader node sends its log to both follower and learner nodes. The leader confirms the transaction's completion only after receiving confirmation that all its follower nodes have successfully persisted the log. While the leader also sends logs to learner nodes, it does not require confirmation from them.

### Configurations

#### Cluster-specific settings

```toml
[general]
# 
server_mode                  = "admin"

[network]
# The IP address on which the current node listens
peer_ip                      =
# The port number on which the current node listens
peer_port                    =

[storage]
# The local temporary directory holding the data read from the shared storage
persistence_dir              =
# The local temporary directory holding the metadata read from the shared storage
data_dir                     =
# The storage type. Available options:
# - "local": Use the local disk for storage (default). The option for STANDALONE mode
# - "minio": Use MinIO for shared storage. The only option for CLUSTER mode, as of v0.5.0
storage_type                 = "minio"

[storage.object_storage]
# The shared storage's (MinIO's) URL
url                          = "127.0.0.1:9000"
# The name of the bucket in the shared storage
bucket_name                  = "infinity"
# The username or ACCESS_KEY for accessing MinIO
access_key                   = "minioadmin"
# The password or SECRET_KEY for accessing MinIO
secret_key                   = "minioadmin"
# Whether to enable HTTPS communication. Defaults to false
enable_https                 = false
```



#### Considerations

### State/Role transition

![uml](https://github.com/user-attachments/assets/276700ab-ee60-4a9c-a449-7e1d8ad5a92b)

### Distributed APIs

- [ADMIN SET NODE ROLE](https://infiniflow.org/docs/dev/http_api_reference#admin-set-node-role)
- [ADMIN SHOW NODE VARIABLES](https://infiniflow.org/docs/dev/http_api_reference#admin-show-node-variables)
- [ADMIN SHOW NODE CONFIGS](https://infiniflow.org/docs/dev/http_api_reference#admin-show-node-configs)
- [ADMIN SHOW NODE VARIABLE](https://infiniflow.org/docs/dev/http_api_reference#admin-show-node-variable)
- [ADMIN SHOW CURRENT NODE](https://infiniflow.org/docs/dev/http_api_reference#admin-show-current-node)
- [ADMIN SHOW NODE](https://infiniflow.org/docs/dev/http_api_reference#admin-show-node)
- [ADMIN LIST NODES](https://infiniflow.org/docs/dev/http_api_reference#admin-list-nodes)
- [ADMIN REMOVE NODE](https://infiniflow.org/docs/dev/http_api_reference#admin-remove-node)

## Set up an Infinity cluster

### 1. Customize your configuration file

### 2. Set the leader node

### 3. Set a follower node

2. Register with the leader.

### 4. Set a learner node

2. Register with the leader.

### 5. check nodes' states

### 6. Promote a follower to leader

1. If the leader node fails, call `ADMIN SET NODE ROLE` to promote a follower node in the cluster to leader.
2. Call `SET NODE ROLE` to set each follower and learner to subscribe to the new leader node.

### ### 7. Remove a node from the cluster

You cannot directly demote a leader node to a standalone mode. It must go through an `admin` state, as illustrated in the state/role transition diagram.
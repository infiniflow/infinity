---
sidebar_position: 2
slug: /set_up_cluster
---
# Set up an Infinity cluster

Architecture overview and user guide for Infinity cluster.

## Overview

An Infinity cluster consists of one leader node, up to four follower nodes, and a number of learner nodes:

- **Leader node**: The read node and only write node.
- **Follower node**: Read node.
- **Learner node**: Read node.

As of v0.5.0, the supported shared storage is MinIO.


![infinity_cluster](https://github.com/user-attachments/assets/3e9abeed-1698-4741-8bdb-ba3b05c1d7a3)

### Architecture

Infinity employs a distributed architecture comprising one leader node and *N* follower nodes, where 0 &le; *N* &le; 4. As illustrated in the diagram above, all nodes in the cluster use MinIO for persistent storage.

- **Leader node**: The node responsible for processing transactions and managing connection status of other nodes in the cluster. When a transaction occurs, the leader node transmits the logs to both follower and learner nodes. The leader node confirms the completion of the transaction only upon receiving messages confirming completion of log persistence from *all* follower nodes.
- **Follower node**: Receives log/WAL from the leader synchronously. It acts as a backup for the leader node, maintaining strong consistency with its state. 
- **Learner node**: Receives log/WAL from the leader *asynchronously*. A learner also serves as a backup for the leader node. However, Its state may be behind that of the leader, as it is not required to maintain strong consistency with the leader, and the leader does not need to confirm whether all learner nodes have completed log persistence.

From the user's perspective, the leader is the only write node, and all write operations must go through the leader node; all nodes in the cluster serve as read nodes, allowing you to send read operations to any of the leader, follower, or learner nodes, thereby alleviating the write burden on the leader.

### Startup and communication processes

When started up in cluster mode, a node enters `ADMIN` state, but is not automatically assigned a role like leader, follower, or learner. You must call `ADMIN SET NODE ROLE` to assign it a role. Once a leader node starts, it reads logs from the local disk to determine the metadata and data to read from shared storage.

Once you set a node to follower or learner using `ADMIN SET NODE ROLE`, it registers with the leader node. Upon receiving the registration request, the leader node sends back its current log for the followers and learners to construct their database state from shared storage.

### Keep-alive mechanism

Once successfully registered with the leader node, a follower or learner starts sending periodic heartbeats to it. The leader node relies on these heartbeats to manage the connection status of each node. For example, if it does not receive heartbeats from a particular node for a specified time period, it sets that node's online status to `timeout`.

### Log synchronization

When a transaction occurs, the leader node sends its log to both follower and learner nodes. The leader confirms the transaction's completion only after receiving confirmation that all its follower nodes have successfully persisted the log. While the leader also sends logs to learner nodes, it does not require confirmation from them.

### Configurations

All nodes in an Infinity cluster require a customized configuration file to start up. Below, we only list the key parameters. For further configurations, such as object storage settings, see the [Configurations](https://infiniflow.org/docs/dev/configurations).

- `server_mode`: The mode in which the server starts.  

  Available options:  

  - `"standalone"`: Start Infinity as a standalone server.
  - `"admin"`:
    - Start Infinity either as a standalone server in `ADMIN` state (when `storage_type` is set to `"local"`)
    - Start Infinity as a cluster node in `ADMIN` state (when `storage_type` is set to `"minio"`)

- `peer_ip`: The IP address on which the current node listens. Used for registration and inter-node communication.

- `peer_port`: The port number on which the current node listens. Used for registration and inter-node communication.

- `storage_type`: The type of storage to use.  

  Available options:  

  - `"local"`: Use the local disk storage (default).
  - `"minio"`: Use MinIO for shared storage. If you set `server_mode` to `"admin"` and `storage_type` to `"minio"`, the node will start as a cluster node in `ADMIN` state.

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

You are required to customize a configuration file to start Infinity as a cluster node. Ensure that you properly set `server_mode`, `peer_ip`, `peer_port`, `storage_type`, and other parameters. When a cluster node starts, it automatically operates in `ADMIN` state.

For further instructions on specifying a configuration file or setting parameters, see the [Configurations](https://infiniflow.org/docs/dev/configurations).

### 2. Set the leader node

 A cluster can have at most one leader node. If the cluster you start does not have a leader node, call `ADMIN SET NODE ROLE` to promote the node you just started, which is in `ADMIN` state, to leader. Below is an example code:

````shell
```shell
curl --request POST \
    --url http://localhost:23821/admin/node/current \
    --header 'accept: application/json' \
    --header 'content-type: application/json' \
    --data ' {
        "role" : "leader",
        "name" : "Trump",
    } '
```
````

*When the method call succeeds, the node's state changes to `LEADER`.*

:::tip NOTE

A node in `ADMIN`, `FOLLOWER`, or `LEARNER` state can be promoted to leader node. In other words, you cannot promote a standalone node directly to leader.

:::

### 3. Set a follower node

3.1. Use a different configuration file to start another cluster node.  

​       *When the node starts, it is in `ADMIN` state.*

3.2. If the number of follower nodes in your cluster is less than four, call `ADMIN SET NODE ROLE` to promote the new node to follower node.

```shell
curl --request POST \
    --url http://localhost:23822/admin/node/current \
    --header 'accept: application/json' \
    --header 'content-type: application/json' \
    --data ' {
        "role" : "follower",
        "name" : "Tom",
        "address" : "0.0.0.0:23851"
    } '
```

​       *When the method call succeeds, the node is registered with the leader node, which listens on 0.0.0.0:23851, and its state changes to `FOLLOWER`.*

:::tip NOTE

A node in `ADMIN`, `LEADER`, or `LEARNER` state can be switched to follower node. In other words, you cannot promote a standalone node directly to follower.

:::

### 4. Set a learner node

4.1. Use a third configuration file to start another cluster node.  

​       *When the node starts, it is in `ADMIN` state.*

Register with the leader.

### 5. Promote a follower/learner to leader

1. If the leader node fails, call `ADMIN SET NODE ROLE` to promote a follower node in the cluster to leader.
2. Call `SET NODE ROLE` to set each follower and learner to subscribe to the new leader node.

### ### 6. Remove a node from the cluster

You cannot directly demote a leader node to a STANDALONE state. It must go through an `ADMIN` state, as illustrated in the state/role transition diagram.
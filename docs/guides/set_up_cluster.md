---
sidebar_position: 2
slug: /set_up_cluster
---
# Set up an Infinity cluster

Architecture overview and user guide for Infinity cluster.

## Overview

An Infinity cluster consists of one leader node, up to four follower nodes, and an unlimited number of learner nodes:

- **Leader node**: Responsible for data writes and transaction submissions.
- **Follower node**: Responsible for synchronous log writes.
- **Learner node**: Responsible for asynchronous log writes.

As of v0.5.0, the supported shared storage is MinIO.


![infinity_cluster](https://github.com/user-attachments/assets/3e9abeed-1698-4741-8bdb-ba3b05c1d7a3)

### Architecture

Infinity employs a distributed architecture comprising one leader node, *N* follower nodes, and *M* learner nodes, where 0 &le; *N* &le; 4 and *M* is unrestricted. As illustrated in the diagram above, all nodes in the cluster use MinIO for persistent storage.

- **Leader node**: The node responsible for transaction processing and node status management. When a transaction occurs, the leader node transmits the logs to both follower and learner nodes. The leader node confirms the completion of the transaction only upon receiving messages confirming completion of log persistence from *all* follower nodes
- **Follower node**: The synchronous log (WAL) write node for the leader. It acts as a backup for the leader node, maintaining strong consistency with its state. 
- **Learner node**: The *asynchronous* log (WAL) write node for the leader. A learner also serves as a backup for the leader node. However, Its state may be behind that of the leader, as it is not required to maintain strong consistency with the leader, and the leader does not need to confirm whether all learner nodes have completed log persistence.

:::tip NOTE

While follower nodes maintain strong consistency with the leader, learner nodes do not require such consistency. Consequently, data read from a learner may be outdated compared to data read from a follower.

:::

### Startup and communication processes

### Log synchronization

### Configurations

#### Cluster-specific settings

#### References

- [infinity/conf/leader.toml](https://github.com/infiniflow/infinity/blob/main/conf/leader.toml)
- [infinify/conf/follower.toml](https://github.com/infiniflow/infinity/blob/main/conf/follower.toml)
- [infinity/conf/learner.toml](https://github.com/infiniflow/infinity/blob/main/conf/learner.toml)

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

## Set up an Infinity cluster

### 1. Customize your configuration file

### 2. Set the leader node

### 3. Set a follower node

2. Subscribe to the leader.

### 4. Set a learner node

2. Subscribe to the leader.

### 5. check nodes' states

### 6. Promote a follower to leader

1. If the leader node fails, call `ADMIN SET NODE ROLE` to promote a follower node in the cluster to leader.
2. Call `SET NODE ROLE` to set each follower and learner to subscribe to the new leader node.

### Demote a leader to a standalone node

You cannot directly demote a leader node to a standalone mode. It must go through an `admin` state, as illustrated in the state/role transition diagram.
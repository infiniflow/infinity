---
sidebar_position: 2
slug: /set_up_cluster
---
# Set up an Infinity cluster

Architecture overview and user guide for Infinity cluster.

## Overview

An Infinity cluster consists of one leader node, up to four follower nodes, and several learner nodes:

- **Leader node**: The read node and the only write node.
- **Follower node**: Read node.
- **Learner node**: Read node.

As of v0.5.0, the supported shared storage is MinIO.

![infinity_cluster](https://github.com/user-attachments/assets/3e9abeed-1698-4741-8bdb-ba3b05c1d7a3)

### Architecture

Infinity employs a distributed architecture comprising one leader node, *N* follower nodes (0 &le; *N* &le; 4), and a number of learner nodes. As illustrated in the diagram above, all nodes in the cluster use MinIO for persistent storage.

- **Leader node**: The node responsible for processing transactions and managing connection status of other nodes in the cluster. When a transaction occurs, the leader node transmits the logs to both follower and learner nodes. The leader node confirms the completion of the transaction only upon receiving messages confirming completion of log persistence from *all* follower nodes.
- **Follower node**: Receives log/WAL from the leader synchronously. It acts as a backup for the leader node, maintaining strong consistency with the leader's data state.
- **Learner node**: Receives log/WAL from the leader *asynchronously*. A learner also serves as a backup for the leader node. However, its state may be behind that of the leader, because it is not required to maintain strong consistency with the leader, and neither does the leader need to confirm whether all learner nodes have completed log persistence.

From the user's perspective, the leader is the only write node, and all write operations must go through the leader node; all nodes in the cluster serve as read nodes, allowing you to send read operations to any of the leader, follower, or learner nodes, thereby alleviating the write burden on the leader.

### Startup and communication processes

When started up in cluster mode, a node enters `ADMIN` mode, but is not automatically assigned a role like leader, follower, or learner. You must call `ADMIN SET NODE ROLE` to assign it a role. Once a leader node starts, it reads logs from the local disk to determine the metadata and data to read from shared storage.

Once you set a node to follower or learner using `ADMIN SET NODE ROLE`, it registers with the leader node. Upon receiving the registration request, the leader node sends back its current log for the followers and learners to construct their data state from shared storage.

### Keep-alive mechanism

Once successfully registered with the leader node, a follower or learner starts sending periodic heartbeats to it. The leader node relies on these heartbeats to manage the connection status of each node. For example, if it does not receive heartbeats from a particular node for a specified time period, it sets that node's connection status to `timeout`.

### Log synchronization

When a transaction occurs, the leader node sends its log to both follower and learner nodes. The leader confirms the transaction's completion only after receiving confirmation that all its follower nodes have successfully persisted the log. While the leader also sends logs to learner nodes, it does not require confirmation from them.

### Mode and role transition

![mode_transition](https://github.com/user-attachments/assets/932072a3-9ffb-4aad-89f1-7eef0fff931c)

## Set up an Infinity cluster

### Customize configuration files for cluster

For *each* cluster node, you are required to prepare a customized configuration file to start it. Ensure that you properly set `server_mode`, `peer_ip`, `peer_port`, `storage_type`, and other related parameters.

1. Set `server_mode` to `"admin"`.

2. Set `storage_type` to `"minio"`.

3. Set `peer_ip` and `peer_port`.

4. Update object storage-specific settings.

5. Save your changes and start up Infinity using the customized configuration file.  

   *When a cluster node starts, it automatically operates in `ADMIN` mode.*

For further instructions on specifying a configuration file or setting parameters, see the [Configurations](https://infiniflow.org/docs/dev/configurations).

### Set the leader node

 A cluster can have only one leader node. If the cluster you start does not have a leader node, call `ADMIN SET NODE ROLE` to promote the node you just started, which currently operates in `ADMIN` mode, to leader. Below is an example code:

```shell
curl --request POST \
    --url http://localhost:23821/admin/node/current \
    --header 'accept: application/json' \
    --header 'content-type: application/json' \
    --data ' {
        "role" : "leader",
        "name" : "Harry",
    } '
```

*When the method call succeeds, the node switches to leader and operates in `CLUSTER` mode.*

:::tip NOTE

A node in `ADMIN` mode with `storage_type = "minio"` or in `CLUSTER` mode (as a follower or learner node) can be promoted to leader.

:::

You can also use `ADMIN SHOW CURRENT NODE` to verify the node's role and connection status:

```shell
curl --request GET \
    --url http://localhost:23821/admin/node/current \
    --header 'accept: application/json'
```

### Set a follower node

If the current node operates in `ADMIN` mode and the number of follower nodes in your cluster is less than four, call `ADMIN SET NODE ROLE` to promote this node to follower node:

```shell
curl --request POST \
    --url http://localhost:23822/admin/node/current \
    --header 'accept: application/json' \
    --header 'content-type: application/json' \
    --data ' {
        "role" : "follower",
        "name" : "Hermione",
        "address" : "0.0.0.0:23851"
    } '
```

*When the method call succeeds, the node is promoted to follower and registered with the leader node, which listens on `0.0.0.0:23851`.*

:::tip NOTE

A node in `ADMIN` mode with `storage_type = "minio"` can be promoted to follower node.

:::

### Set a learner node

If the current node operates in `ADMIN` mode, call `ADMIN SET NODE ROLE` to promote this new node to learner node.

```shell
curl --request POST \
    --url http://localhost:23823/admin/node/current \
    --header 'accept: application/json' \
    --header 'content-type: application/json' \
    --data ' {
        "role" : "learner",
        "name" : "Ron",
        "address" : "0.0.0.0:23851"
    } '
```

*When the method call succeeds, the node is promoted to learner and registered with the leader node, which listens on `0.0.0.0:23851`.*

:::tip NOTE

Only a node in `ADMIN` mode with `storage_type = "minio"` can be promoted to learner node.

:::

### Check cluster health status

You can send an HTTP request `ADMIN LIST NODES` to any node in the cluster to display the health status of all nodes. In the following code example, a follower node is called:

```shell
curl --request GET \
    --url http://localhost:23822/admin/nodes \
    --header 'accept: application/json'
```

*When the method call succeeds, you get the following information of each node:*

- *The HTTP address of the node.*
- *The number of heartbeats received from the leader node.*
- *The name of the node.*
- *The role of the node: leader, follower, or learner.*
- *The connection status of the node.*
- *The last time that the node was updated.*

:::tip NOTE

See `ADMIN LIST NODES` for further details.

:::

### Remove a node from the cluster

Call `ADMIN REMOVE NODE` to remove a node from the cluster. Note that you must send your HTTP request to the leader node for this action. In the following code example, learner Ron will be removed:

```shell
curl --request DELETE \
    --url http://localhost:23821/admin/node/ron \
    --header 'accept: application/json' \
    --header 'content-type: application/json'
```

*When the method call succeeds, the node operates in `ADMIN` mode and is unregistered.*

## Distributed APIs

- [ADMIN SET NODE ROLE](https://infiniflow.org/docs/dev/http_api_reference#admin-set-node-role)
- [ADMIN SHOW NODE VARIABLES](https://infiniflow.org/docs/dev/http_api_reference#admin-show-node-variables)
- [ADMIN SHOW NODE CONFIGS](https://infiniflow.org/docs/dev/http_api_reference#admin-show-node-configs)
- [ADMIN SHOW NODE VARIABLE](https://infiniflow.org/docs/dev/http_api_reference#admin-show-node-variable)
- [ADMIN SHOW CURRENT NODE](https://infiniflow.org/docs/dev/http_api_reference#admin-show-current-node)
- [ADMIN SHOW NODE](https://infiniflow.org/docs/dev/http_api_reference#admin-show-node)
- [ADMIN LIST NODES](https://infiniflow.org/docs/dev/http_api_reference#admin-list-nodes)
- [ADMIN REMOVE NODE](https://infiniflow.org/docs/dev/http_api_reference#admin-remove-node)
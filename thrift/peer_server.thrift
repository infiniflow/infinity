namespace cpp infinity_peer_server

// https://github.com/apache/thrift/blob/master/test/Recursive.thrift

enum NodeType {
kLeader,
kFollower,
kLearner,
}

struct NodeInfo {
1: string node_name,
2: NodeType node_type,
3: string node_ip,
4: i64 node_port,
5: i64 txn_timestamp,
}

struct RegisterRequest {
1: string node_name,
2: NodeType node_type,
3: string node_ip,
4: i64 node_port,
5: i64 txn_timestamp,
}

struct RegisterResponse {
1: i64 error_code,
2: string error_msg,
3: string leader_name,
4: i64 leader_term,
5: i64 heart_beat_interval, // microseconds
}

struct UnRegisterRequest {
1: string node_name,
}

struct UnRegisterResponse {
1: i64 error_code,
2: string error_msg,
}

struct HeartBeatRequest {
1: string node_name,
2: i64 txn_timestamp,
}

struct HeartBeatResponse {
1: i64 error_code,
2: string error_msg,
3: i64 leader_term,
4: list<NodeInfo> other_nodes,
}

struct SyncLogRequest {
1: string node_name,
2: list<binary> log_entries,
}

struct SyncLogResponse {
1: i64 error_code,
2: string error_msg,
3: i64 txn_timestamp,
}

struct ChangeRoleRequest {
1: string node_name,
2: NodeType node_type,
}

struct ChangeRoleResponse {
1: string node_name,
}

struct NewLeaderRequest {
1: string node_name,
2: string node_ip,
3: i64 node_port,
4: NodeType new_node_type,
5: i64 new_leader_term,
}

struct NewLeaderResponse {
1: string node_name,
}

// Service
service PeerService {

// From follower/learner to leader.
RegisterResponse Register(1:RegisterRequest request),
UnRegisterResponse UnRegister(1:UnRegisterRequest request),

// From follower/learner to leader
HeartBeatResponse HeartBeat(1:HeartBeatRequest request),

// From leader to follower/learner
SyncLogResponse SyncLog(1:SyncLogRequest request),

// From leader to follower/learner
ChangeRoleResponse ChangeRole(1:ChangeRoleRequest request),

// From new leader to others
NewLeaderResponse NewLeader(1:NewLeaderRequest request),

}

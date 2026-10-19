//! Cluster/peer layer ported from `src/network/` and `src/main/cluster_manager*`.
//!
//! # Transport design change
//!
//! The C++ implementation generates a Thrift client/server from
//! `thrift/peer_server.thrift` and speaks TBinaryProtocol over TCP. Porting the
//! generated Thrift IDL into Rust would mean re-creating code generation, and the
//! distributed deployment mode is not wired into the Rust server yet. Instead the
//! request/response boundary is a [`PeerTransport`] trait. [`InProcessPeerTransport`]
//! implements it on top of [`PeerServerThriftService`] directly; the state machines,
//! registry rules, task queueing/retry and pooled server semantics below are the
//! real ported logic, not stubs. A network transport can implement the same trait
//! later without touching the tasks or the cluster state.
//!
//! The cluster manager itself is not part of the 11 audited classes; it is included
//! here because the peer service and the heartbeat state machine are meaningless
//! without it (the C++ service delegates every request to `ClusterManager`).

use std::collections::{BTreeMap, VecDeque};
use std::fmt;
use std::sync::atomic::{AtomicBool, AtomicU64, AtomicU8, AtomicUsize, Ordering};
use std::sync::{Arc, Condvar, Mutex, MutexGuard};
use std::time::{Duration, SystemTime, UNIX_EPOCH};

/// Error codes from `src/common/status.cppm` (`ErrorCode`).
pub mod error_code {
    pub const INVALID_SERVER_ADDRESS: i64 = 7016;
    pub const CANT_CONNECT_SERVER: i64 = 7018;
    pub const NOT_EXIST_NODE: i64 = 7019;
    pub const DUPLICATE_NODE: i64 = 7020;
    pub const CANT_CONNECT_LEADER: i64 = 7021;
    pub const NOT_REGISTERED: i64 = 7025;
    pub const CANT_SWITCH_ROLE: i64 = 7026;
    pub const TOO_MANY_FOLLOWER: i64 = 7027;
    pub const TOO_MANY_LEARNER: i64 = 7028;
    pub const UNEXPECTED_ERROR: i64 = 7011;
    pub const INVALID_NODE_ROLE: i64 = 8007;
    pub const INVALID_NODE_STATUS: i64 = 8008;
    pub const NODE_INFO_UPDATED: i64 = 8009;
}

/// Errors surfaced by the peer transport / cluster API.
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum PeerError {
    /// The transport could not reach the peer.
    Transport(String),
    /// The peer accepted the call but reported an application-level failure.
    Rejected { code: i64, message: String },
}

impl PeerError {
    pub fn rejected(code: i64, message: impl Into<String>) -> Self {
        Self::Rejected {
            code,
            message: message.into(),
        }
    }

    pub fn code(&self) -> i64 {
        match self {
            Self::Transport(_) => error_code::CANT_CONNECT_SERVER,
            Self::Rejected { code, .. } => *code,
        }
    }
}

impl fmt::Display for PeerError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::Transport(message) => write!(f, "peer transport error: {message}"),
            Self::Rejected { code, message } => write!(f, "peer error {code}: {message}"),
        }
    }
}

impl std::error::Error for PeerError {}

fn lock<T>(mutex: &Mutex<T>) -> MutexGuard<'_, T> {
    mutex
        .lock()
        .unwrap_or_else(|poisoned| poisoned.into_inner())
}

fn system_now_secs() -> u64 {
    SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .map(|elapsed| elapsed.as_secs())
        .unwrap_or(0)
}

/// `NodeRole` from `src/parser/statement/admin_statement.h`.
#[derive(Debug, Clone, Copy, Default, PartialEq, Eq, Hash)]
pub enum NodeRole {
    Admin,
    Standalone,
    Leader,
    Follower,
    Learner,
    #[default]
    UnInitialized,
}

impl NodeRole {
    pub fn is_non_leader(self) -> bool {
        matches!(self, Self::Follower | Self::Learner)
    }
}

impl fmt::Display for NodeRole {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        let name = match self {
            Self::Admin => "admin",
            Self::Standalone => "standalone",
            Self::Leader => "leader",
            Self::Follower => "follower",
            Self::Learner => "learner",
            Self::UnInitialized => "uninitialized",
        };
        f.write_str(name)
    }
}

/// `NodeStatus` from `src/network/node_info.cppm`.
#[derive(Debug, Clone, Copy, Default, PartialEq, Eq, Hash)]
pub enum NodeStatus {
    Alive,
    Timeout,
    LostConnection,
    Removed,
    #[default]
    Invalid,
}

impl fmt::Display for NodeStatus {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        let name = match self {
            Self::Alive => "alive",
            Self::Timeout => "timeout",
            Self::LostConnection => "lost connection",
            Self::Removed => "removed",
            Self::Invalid => "invalid",
        };
        f.write_str(name)
    }
}

/// `NodeType` from `thrift/peer_server.thrift`.
#[derive(Debug, Clone, Copy, Default, PartialEq, Eq, Hash)]
pub enum NodeType {
    Admin,
    Leader,
    Follower,
    Learner,
    #[default]
    Invalid,
}

impl NodeType {
    pub fn from_role(role: NodeRole) -> Self {
        match role {
            NodeRole::Admin => Self::Admin,
            NodeRole::Leader => Self::Leader,
            NodeRole::Follower => Self::Follower,
            NodeRole::Learner => Self::Learner,
            NodeRole::Standalone | NodeRole::UnInitialized => Self::Invalid,
        }
    }
}

impl fmt::Display for NodeType {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        let name = match self {
            Self::Admin => "admin",
            Self::Leader => "leader",
            Self::Follower => "follower",
            Self::Learner => "learner",
            Self::Invalid => "invalid",
        };
        f.write_str(name)
    }
}

/// `NodeInfo` from `src/network/node_info.cppm`.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct NodeInfo {
    node_role: NodeRole,
    node_status: NodeStatus,
    node_name: String,
    node_ip: String,
    node_port: i64,
    txn_ts: i64,
    update_ts: u64,
    leader_term: i64,
    heartbeat_interval: i64,
    heartbeat_count: u64,
}

impl NodeInfo {
    pub fn new(
        node_role: NodeRole,
        node_status: NodeStatus,
        node_name: impl Into<String>,
        node_ip: impl Into<String>,
        node_port: i64,
        txn_ts: i64,
        update_ts: u64,
    ) -> Self {
        Self {
            node_role,
            node_status,
            node_name: node_name.into(),
            node_ip: node_ip.into(),
            node_port,
            txn_ts,
            update_ts,
            leader_term: 0,
            heartbeat_interval: 0,
            heartbeat_count: 0,
        }
    }

    /// Matches the third C++ constructor, which also seeds the heartbeat count.
    #[allow(clippy::too_many_arguments)]
    pub fn with_heartbeat_count(
        node_role: NodeRole,
        node_status: NodeStatus,
        node_name: impl Into<String>,
        node_ip: impl Into<String>,
        node_port: i64,
        txn_ts: i64,
        update_ts: u64,
        heartbeat_count: u64,
    ) -> Self {
        let mut node = Self::new(
            node_role,
            node_status,
            node_name,
            node_ip,
            node_port,
            txn_ts,
            update_ts,
        );
        node.heartbeat_count = heartbeat_count;
        node
    }

    /// Matches `NodeInfo(NodeRole, const std::string &ip_addr, i64 port)`:
    /// name/timestamps are empty and the status starts as `kInvalid`.
    pub fn endpoint(node_role: NodeRole, node_ip: impl Into<String>, node_port: i64) -> Self {
        Self::new(
            node_role,
            NodeStatus::Invalid,
            String::new(),
            node_ip,
            node_port,
            0,
            0,
        )
    }

    pub fn node_role(&self) -> NodeRole {
        self.node_role
    }

    pub fn set_node_role(&mut self, role: NodeRole) {
        self.node_role = role;
    }

    pub fn node_status(&self) -> NodeStatus {
        self.node_status
    }

    pub fn set_node_status(&mut self, status: NodeStatus) {
        self.node_status = status;
    }

    pub fn node_name(&self) -> &str {
        &self.node_name
    }

    pub fn set_node_name(&mut self, name: impl Into<String>) {
        self.node_name = name.into();
    }

    pub fn node_ip(&self) -> &str {
        &self.node_ip
    }

    pub fn set_node_ip(&mut self, ip: impl Into<String>) {
        self.node_ip = ip.into();
    }

    pub fn node_port(&self) -> i64 {
        self.node_port
    }

    pub fn set_node_port(&mut self, port: i64) {
        self.node_port = port;
    }

    pub fn txn_ts(&self) -> i64 {
        self.txn_ts
    }

    pub fn set_txn_ts(&mut self, txn_ts: i64) {
        self.txn_ts = txn_ts;
    }

    pub fn update_ts(&self) -> u64 {
        self.update_ts
    }

    pub fn set_update_ts(&mut self, update_ts: u64) {
        self.update_ts = update_ts;
    }

    pub fn leader_term(&self) -> i64 {
        self.leader_term
    }

    pub fn set_leader_term(&mut self, leader_term: i64) {
        self.leader_term = leader_term;
    }

    pub fn heartbeat_interval(&self) -> i64 {
        self.heartbeat_interval
    }

    pub fn set_heartbeat_interval(&mut self, interval: i64) {
        self.heartbeat_interval = interval;
    }

    pub fn heartbeat_count(&self) -> u64 {
        self.heartbeat_count
    }

    pub fn increase_heartbeat_count(&mut self) {
        self.heartbeat_count += 1;
    }

    /// `IsSameNode`: identity is the name plus the reachable endpoint.
    pub fn is_same_node(&self, other: &Self) -> bool {
        self.node_name == other.node_name
            && self.node_ip == other.node_ip
            && self.node_port == other.node_port
    }
}

// ---------------------------------------------------------------------------
// Thrift-shaped request/response structs (field names follow the IDL).
// ---------------------------------------------------------------------------

#[derive(Debug, Clone, Default, PartialEq, Eq)]
pub struct RegisterRequest {
    pub node_name: String,
    pub node_type: NodeType,
    pub node_ip: String,
    pub node_port: i64,
    pub txn_timestamp: i64,
}

#[derive(Debug, Clone, Default, PartialEq, Eq)]
pub struct RegisterResponse {
    pub error_code: i64,
    pub error_msg: String,
    pub leader_name: String,
    pub leader_term: i64,
    pub heart_beat_interval: i64,
}

#[derive(Debug, Clone, Default, PartialEq, Eq)]
pub struct UnregisterRequest {
    pub node_name: String,
}

#[derive(Debug, Clone, Default, PartialEq, Eq)]
pub struct UnregisterResponse {
    pub error_code: i64,
    pub error_msg: String,
}

#[derive(Debug, Clone, Default, PartialEq, Eq)]
pub struct HeartBeatRequest {
    pub node_name: String,
    pub node_type: NodeType,
    pub node_ip: String,
    pub node_port: i64,
    pub txn_timestamp: i64,
}

#[derive(Debug, Clone, Default, PartialEq, Eq)]
pub struct HeartBeatResponse {
    pub error_code: i64,
    pub error_msg: String,
    pub leader_term: i64,
    pub other_nodes: Vec<NodeInfo>,
    pub sender_status: NodeStatus,
}

#[derive(Debug, Clone, Default, PartialEq, Eq)]
pub struct SyncLogRequest {
    pub node_name: String,
    pub log_entries: Vec<String>,
    pub on_startup: bool,
}

#[derive(Debug, Clone, Default, PartialEq, Eq)]
pub struct SyncLogResponse {
    pub error_code: i64,
    pub error_msg: String,
    pub txn_timestamp: i64,
}

#[derive(Debug, Clone, Default, PartialEq, Eq)]
pub struct ChangeRoleRequest {
    pub node_name: String,
    pub node_type: NodeType,
}

#[derive(Debug, Clone, Default, PartialEq, Eq)]
pub struct ChangeRoleResponse {
    pub error_code: i64,
    pub error_msg: String,
}

#[derive(Debug, Clone, Default, PartialEq, Eq)]
pub struct NewLeaderRequest {
    pub node_name: String,
    pub node_ip: String,
    pub node_port: i64,
    pub new_node_type: NodeType,
    pub new_leader_term: i64,
}

#[derive(Debug, Clone, Default, PartialEq, Eq)]
pub struct NewLeaderResponse {
    pub node_name: String,
}

// ---------------------------------------------------------------------------
// Peer tasks
// ---------------------------------------------------------------------------

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum PeerTaskType {
    Invalid,
    Terminate,
    Register,
    Unregister,
    HeartBeat,
    LogSync,
    ChangeRole,
    NewLeader,
}

impl fmt::Display for PeerTaskType {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        let name = match self {
            Self::Invalid => "invalid",
            Self::Terminate => "terminate",
            Self::Register => "register",
            Self::Unregister => "unregister",
            Self::HeartBeat => "heartbeat",
            Self::LogSync => "log sync",
            Self::ChangeRole => "change role",
            Self::NewLeader => "new leader",
        };
        f.write_str(name)
    }
}

/// Completion state shared by every task. Mirrors the C++ `PeerTask::Wait` /
/// `PeerTask::Complete` pair; async tasks complete immediately.
#[derive(Debug)]
struct Completion {
    is_async: bool,
    complete: Mutex<bool>,
    cv: Condvar,
}

impl Completion {
    fn new(is_async: bool) -> Self {
        Self {
            is_async,
            complete: Mutex::new(false),
            cv: Condvar::new(),
        }
    }

    fn wait(&self) {
        if self.is_async {
            return;
        }
        let mut complete = lock(&self.complete);
        while !*complete {
            complete = self
                .cv
                .wait(complete)
                .unwrap_or_else(|poisoned| poisoned.into_inner());
        }
    }

    fn complete(&self) {
        if self.is_async {
            return;
        }
        *lock(&self.complete) = true;
        self.cv.notify_all();
    }

    fn is_complete(&self) -> bool {
        self.is_async || *lock(&self.complete)
    }
}

/// The task dispatch abstraction. [`PeerClient`] queues `Arc<dyn PeerTask>` and
/// invokes [`PeerTask::execute`] on the transport, exactly like the C++
/// `PeerClient::Process` switch statement.
pub trait PeerTask: Send + Sync {
    fn task_type(&self) -> PeerTaskType;
    fn to_string(&self) -> String;
    fn execute(&self, transport: &dyn PeerTransport) -> Result<(), PeerError>;
    fn wait(&self);
    fn complete(&self);
    fn is_async(&self) -> bool;
    fn is_complete(&self) -> bool;
}

macro_rules! impl_task_common {
    ($name:ident, $ty:expr) => {
        fn task_type(&self) -> PeerTaskType {
            $ty
        }

        fn wait(&self) {
            self.completion.wait();
        }

        fn complete(&self) {
            self.completion.complete();
        }

        fn is_async(&self) -> bool {
            self.completion.is_async
        }

        fn is_complete(&self) -> bool {
            self.completion.is_complete()
        }
    };
}

/// `TerminatePeerTask`.
#[derive(Debug)]
pub struct TerminatePeerTask {
    completion: Completion,
}

impl TerminatePeerTask {
    pub fn new(force_async: bool) -> Self {
        Self {
            completion: Completion::new(force_async),
        }
    }
}

impl PeerTask for TerminatePeerTask {
    impl_task_common!(TerminatePeerTask, PeerTaskType::Terminate);

    fn to_string(&self) -> String {
        PeerTaskType::Terminate.to_string()
    }

    fn execute(&self, _transport: &dyn PeerTransport) -> Result<(), PeerError> {
        Ok(())
    }
}

/// `RegisterPeerTask`.
#[derive(Debug)]
pub struct RegisterPeerTask {
    pub node_name: String,
    pub node_role: NodeRole,
    pub node_ip: String,
    pub node_port: i64,
    pub txn_ts: i64,
    completion: Completion,
    response: Mutex<RegisterResponse>,
}

impl RegisterPeerTask {
    pub fn new(
        node_name: impl Into<String>,
        node_role: NodeRole,
        node_ip: impl Into<String>,
        node_port: i64,
        txn_ts: i64,
    ) -> Self {
        Self {
            node_name: node_name.into(),
            node_role,
            node_ip: node_ip.into(),
            node_port,
            txn_ts,
            completion: Completion::new(false),
            response: Mutex::new(RegisterResponse::default()),
        }
    }

    pub fn response(&self) -> RegisterResponse {
        lock(&self.response).clone()
    }

    pub fn set_response(&self, response: RegisterResponse) {
        *lock(&self.response) = response;
    }
}

impl PeerTask for RegisterPeerTask {
    impl_task_common!(RegisterPeerTask, PeerTaskType::Register);

    fn to_string(&self) -> String {
        format!(
            "register@{}, {}:{}, {}",
            self.node_name, self.node_ip, self.node_port, self.node_role
        )
    }

    fn execute(&self, transport: &dyn PeerTransport) -> Result<(), PeerError> {
        let request = RegisterRequest {
            node_name: self.node_name.clone(),
            node_type: NodeType::from_role(self.node_role),
            node_ip: self.node_ip.clone(),
            node_port: self.node_port,
            txn_timestamp: self.txn_ts,
        };
        match transport.register(&request) {
            Ok(response) => {
                self.set_response(response);
                Ok(())
            }
            Err(error) => {
                self.set_response(RegisterResponse {
                    error_code: error_code::CANT_CONNECT_LEADER,
                    error_msg: error.to_string(),
                    ..RegisterResponse::default()
                });
                Err(error)
            }
        }
    }
}

/// `UnregisterPeerTask`.
#[derive(Debug)]
pub struct UnregisterPeerTask {
    pub node_name: String,
    completion: Completion,
    response: Mutex<UnregisterResponse>,
}

impl UnregisterPeerTask {
    pub fn new(node_name: impl Into<String>) -> Self {
        Self {
            node_name: node_name.into(),
            completion: Completion::new(false),
            response: Mutex::new(UnregisterResponse::default()),
        }
    }

    pub fn response(&self) -> UnregisterResponse {
        lock(&self.response).clone()
    }

    pub fn set_response(&self, response: UnregisterResponse) {
        *lock(&self.response) = response;
    }
}

impl PeerTask for UnregisterPeerTask {
    impl_task_common!(UnregisterPeerTask, PeerTaskType::Unregister);

    fn to_string(&self) -> String {
        format!("unregister@{}", self.node_name)
    }

    fn execute(&self, transport: &dyn PeerTransport) -> Result<(), PeerError> {
        let request = UnregisterRequest {
            node_name: self.node_name.clone(),
        };
        match transport.unregister(&request) {
            Ok(response) => {
                self.set_response(response);
                Ok(())
            }
            Err(error) => {
                self.set_response(UnregisterResponse {
                    error_code: error_code::CANT_CONNECT_LEADER,
                    error_msg: error.to_string(),
                });
                Err(error)
            }
        }
    }
}

/// `HeartBeatPeerTask`.
#[derive(Debug)]
pub struct HeartBeatPeerTask {
    pub node_name: String,
    pub node_role: NodeRole,
    pub node_ip: String,
    pub node_port: i64,
    pub txn_ts: i64,
    completion: Completion,
    response: Mutex<HeartBeatResponse>,
}

impl HeartBeatPeerTask {
    pub fn new(
        node_name: impl Into<String>,
        node_role: NodeRole,
        node_ip: impl Into<String>,
        node_port: i64,
        txn_ts: i64,
    ) -> Self {
        Self {
            node_name: node_name.into(),
            node_role,
            node_ip: node_ip.into(),
            node_port,
            txn_ts,
            completion: Completion::new(false),
            response: Mutex::new(HeartBeatResponse::default()),
        }
    }

    pub fn response(&self) -> HeartBeatResponse {
        lock(&self.response).clone()
    }

    pub fn set_response(&self, response: HeartBeatResponse) {
        *lock(&self.response) = response;
    }
}

impl PeerTask for HeartBeatPeerTask {
    impl_task_common!(HeartBeatPeerTask, PeerTaskType::HeartBeat);

    fn to_string(&self) -> String {
        format!("heartbeat@{}, {}", self.node_name, self.txn_ts)
    }

    fn execute(&self, transport: &dyn PeerTransport) -> Result<(), PeerError> {
        let request = HeartBeatRequest {
            node_name: self.node_name.clone(),
            node_type: NodeType::from_role(self.node_role),
            node_ip: self.node_ip.clone(),
            node_port: self.node_port,
            txn_timestamp: self.txn_ts,
        };
        match transport.heartbeat(&request) {
            Ok(mut response) => {
                // The C++ client drops its own entry from the leader's node list.
                if let Some(from_node) = transport.local_node_name() {
                    response
                        .other_nodes
                        .retain(|node| node.node_name() != from_node);
                }
                self.set_response(response);
                Ok(())
            }
            Err(error) => {
                self.set_response(HeartBeatResponse {
                    error_code: error_code::CANT_CONNECT_LEADER,
                    error_msg: error.to_string(),
                    sender_status: NodeStatus::Invalid,
                    ..HeartBeatResponse::default()
                });
                Err(error)
            }
        }
    }
}

/// `SyncLogTask`.
#[derive(Debug)]
pub struct SyncLogTask {
    pub node_name: String,
    pub log_strings: Vec<String>,
    pub on_register: bool,
    completion: Completion,
    response: Mutex<SyncLogResponse>,
}

impl SyncLogTask {
    pub fn new(node_name: impl Into<String>, log_strings: Vec<String>, on_register: bool) -> Self {
        Self {
            node_name: node_name.into(),
            log_strings,
            on_register,
            completion: Completion::new(false),
            response: Mutex::new(SyncLogResponse::default()),
        }
    }

    pub fn response(&self) -> SyncLogResponse {
        lock(&self.response).clone()
    }

    pub fn set_response(&self, response: SyncLogResponse) {
        *lock(&self.response) = response;
    }
}

impl PeerTask for SyncLogTask {
    impl_task_common!(SyncLogTask, PeerTaskType::LogSync);

    fn to_string(&self) -> String {
        format!("log sync@{}, {}", self.node_name, self.log_strings.len())
    }

    fn execute(&self, transport: &dyn PeerTransport) -> Result<(), PeerError> {
        let request = SyncLogRequest {
            node_name: self.node_name.clone(),
            log_entries: self.log_strings.clone(),
            on_startup: self.on_register,
        };
        match transport.sync_log(&request) {
            Ok(response) => {
                self.set_response(response);
                Ok(())
            }
            Err(error) => {
                self.set_response(SyncLogResponse {
                    error_code: error_code::CANT_CONNECT_SERVER,
                    error_msg: error.to_string(),
                    txn_timestamp: 0,
                });
                Err(error)
            }
        }
    }
}

/// `ChangeRoleTask`.
#[derive(Debug)]
pub struct ChangeRoleTask {
    pub node_name: String,
    pub role_name: String,
    completion: Completion,
    response: Mutex<ChangeRoleResponse>,
}

impl ChangeRoleTask {
    pub fn new(node_name: impl Into<String>, role_name: impl Into<String>) -> Self {
        Self {
            node_name: node_name.into(),
            role_name: role_name.into(),
            completion: Completion::new(false),
            response: Mutex::new(ChangeRoleResponse::default()),
        }
    }

    pub fn response(&self) -> ChangeRoleResponse {
        lock(&self.response).clone()
    }

    pub fn set_response(&self, response: ChangeRoleResponse) {
        *lock(&self.response) = response;
    }
}

impl PeerTask for ChangeRoleTask {
    impl_task_common!(ChangeRoleTask, PeerTaskType::ChangeRole);

    fn to_string(&self) -> String {
        format!("change role to {}", self.role_name)
    }

    fn execute(&self, transport: &dyn PeerTransport) -> Result<(), PeerError> {
        // C++ lowercases the role and only understands "admin".
        let node_type = if self.role_name.eq_ignore_ascii_case("admin") {
            NodeType::Admin
        } else {
            NodeType::Invalid
        };
        let request = ChangeRoleRequest {
            node_name: self.node_name.clone(),
            node_type,
        };
        match transport.change_role(&request) {
            Ok(response) => {
                self.set_response(response);
                Ok(())
            }
            Err(error) => {
                self.set_response(ChangeRoleResponse {
                    error_code: error_code::CANT_CONNECT_SERVER,
                    error_msg: error.to_string(),
                });
                Err(error)
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Transport
// ---------------------------------------------------------------------------

/// Request/response boundary that replaces the generated Thrift client.
pub trait PeerTransport: Send + Sync {
    fn register(&self, request: &RegisterRequest) -> Result<RegisterResponse, PeerError>;
    fn unregister(&self, request: &UnregisterRequest) -> Result<UnregisterResponse, PeerError>;
    fn heartbeat(&self, request: &HeartBeatRequest) -> Result<HeartBeatResponse, PeerError>;
    fn sync_log(&self, request: &SyncLogRequest) -> Result<SyncLogResponse, PeerError>;
    fn change_role(&self, request: &ChangeRoleRequest) -> Result<ChangeRoleResponse, PeerError>;
    fn new_leader(&self, request: &NewLeaderRequest) -> Result<NewLeaderResponse, PeerError>;

    fn connect(&self) -> Result<(), PeerError> {
        Ok(())
    }

    fn disconnect(&self) -> Result<(), PeerError> {
        Ok(())
    }

    fn is_connected(&self) -> bool {
        true
    }

    /// Name of the node that owns this transport; used to filter self out of the
    /// leader's heartbeat node list.
    fn local_node_name(&self) -> Option<String> {
        None
    }

    /// The clock used for `update_ts` / heartbeat bookkeeping. Tests override it.
    fn now_secs(&self) -> u64 {
        system_now_secs()
    }
}

/// `LogSyncSink` stands in for `WalManager::FlushLogByReplication` plus the
/// cluster replay path. The Rust engine has no replicated WAL yet, so the port
/// exposes the application hook and records progress in the cluster manager.
pub trait LogSyncSink: Send + Sync {
    fn apply(
        &self,
        node_name: &str,
        entries: &[String],
        on_startup: bool,
    ) -> Result<(), (i64, String)>;
}

/// A sink that records every applied batch. Used by tests and by the in-process
/// transport when no real WAL replication is attached.
#[derive(Debug, Default)]
pub struct RecordingLogSink {
    applied: Mutex<Vec<(String, Vec<String>, bool)>>,
}

impl RecordingLogSink {
    pub fn applied(&self) -> Vec<(String, Vec<String>, bool)> {
        lock(&self.applied).clone()
    }

    pub fn entry_count(&self) -> usize {
        lock(&self.applied)
            .iter()
            .map(|(_, entries, _)| entries.len())
            .sum()
    }
}

impl LogSyncSink for RecordingLogSink {
    fn apply(
        &self,
        node_name: &str,
        entries: &[String],
        on_startup: bool,
    ) -> Result<(), (i64, String)> {
        lock(&self.applied).push((node_name.to_string(), entries.to_vec(), on_startup));
        Ok(())
    }
}

// ---------------------------------------------------------------------------
// Cluster state (backing store for PeerServerThriftService)
// ---------------------------------------------------------------------------

/// Heartbeat timing state machine. The leader runs one of these to decide when
/// an alive follower has gone silent; a follower runs one to decide when to send
/// the next heartbeat. `now` is always injected so the transitions are testable.
#[derive(Debug, Clone)]
pub struct HeartbeatScheduler {
    interval_ms: u64,
    last_beat_ms: u64,
    running: bool,
}

impl HeartbeatScheduler {
    /// `CheckHeartBeat` / `HeartBeatToLeaderThread` default to 1000 ms.
    pub fn new(interval_ms: u64) -> Self {
        Self {
            interval_ms,
            last_beat_ms: 0,
            running: false,
        }
    }

    pub fn start(&mut self, now_ms: u64) {
        self.running = true;
        self.last_beat_ms = now_ms;
    }

    pub fn stop(&mut self) {
        self.running = false;
    }

    pub fn is_running(&self) -> bool {
        self.running
    }

    pub fn interval_ms(&self) -> u64 {
        self.interval_ms
    }

    pub fn set_interval_ms(&mut self, interval_ms: u64) {
        self.interval_ms = interval_ms;
    }

    /// Returns `true` exactly once per elapsed interval. `now - last` is used so
    /// a caller that wakes late still fires, matching the `wait_for` loop.
    pub fn take_tick(&mut self, now_ms: u64) -> bool {
        if !self.running || now_ms.saturating_sub(self.last_beat_ms) < self.interval_ms {
            return false;
        }
        self.last_beat_ms = now_ms;
        true
    }
}

/// How many seconds of silence makes a leader mark a follower `kTimeout`
/// (`other_node_update_ts + 4 < this_node_update_ts` in C++).
pub const HEARTBEAT_TIMEOUT_SECS: u64 = 4;

/// Result of leader-side heartbeat processing, mapped one-to-one onto
/// `HeartBeatResponse`.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct HeartbeatUpdate {
    other_nodes: Vec<NodeInfo>,
    leader_term: i64,
    sender_status: NodeStatus,
    error_code: i64,
    error_msg: String,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum UpdateNodeOp {
    Remove,
    LostConnection,
}

#[derive(Debug)]
struct ClusterInner {
    this_node: Option<NodeInfo>,
    other_nodes: BTreeMap<String, NodeInfo>,
    leader_node: Option<NodeInfo>,
    current_role: NodeRole,
}

impl Default for ClusterInner {
    fn default() -> Self {
        Self {
            this_node: None,
            other_nodes: BTreeMap::new(),
            leader_node: None,
            current_role: NodeRole::UnInitialized,
        }
    }
}

/// The state machine behind every peer RPC. Ports the parts of `ClusterManager`
/// that the 11 audited classes depend on: role transitions, node registry,
/// heartbeat timeouts, follower registration and log-sync progress.
#[derive(Debug)]
pub struct ClusterManager {
    inner: Mutex<ClusterInner>,
    follower_limit: AtomicU8,
    applied_log_count: AtomicU64,
}

impl Default for ClusterManager {
    fn default() -> Self {
        Self::new()
    }
}

impl ClusterManager {
    pub fn new() -> Self {
        Self {
            inner: Mutex::new(ClusterInner::default()),
            follower_limit: AtomicU8::new(4),
            applied_log_count: AtomicU64::new(0),
        }
    }

    pub fn role(&self) -> NodeRole {
        lock(&self.inner).current_role
    }

    pub fn this_node(&self) -> Option<NodeInfo> {
        lock(&self.inner).this_node.clone()
    }

    pub fn leader_node(&self) -> Option<NodeInfo> {
        lock(&self.inner).leader_node.clone()
    }

    pub fn node_count(&self) -> usize {
        lock(&self.inner).other_nodes.len()
    }

    pub fn applied_log_count(&self) -> u64 {
        self.applied_log_count.load(Ordering::SeqCst)
    }

    fn init_single(
        &self,
        role: NodeRole,
        node_name: &str,
        node_ip: &str,
        node_port: i64,
        now_secs: u64,
    ) -> Result<(), PeerError> {
        let mut inner = lock(&self.inner);
        if inner.this_node.is_some() {
            return Err(PeerError::rejected(
                error_code::UNEXPECTED_ERROR,
                "Init node error: already initialized.",
            ));
        }
        inner.this_node = Some(NodeInfo::new(
            role,
            NodeStatus::Alive,
            node_name,
            node_ip,
            node_port,
            0,
            now_secs,
        ));
        inner.current_role = role;
        Ok(())
    }

    pub fn init_as_admin(&self, node_name: &str, node_ip: &str, node_port: i64) {
        let mut inner = lock(&self.inner);
        inner.current_role = NodeRole::Admin;
        inner.this_node = Some(NodeInfo::new(
            NodeRole::Admin,
            NodeStatus::Alive,
            node_name,
            node_ip,
            node_port,
            0,
            0,
        ));
    }

    pub fn init_as_standalone(&self, node_name: &str, node_ip: &str, node_port: i64) {
        let mut inner = lock(&self.inner);
        inner.this_node = Some(NodeInfo::new(
            NodeRole::Standalone,
            NodeStatus::Alive,
            node_name,
            node_ip,
            node_port,
            0,
            0,
        ));
        inner.current_role = NodeRole::Standalone;
    }

    pub fn init_as_leader(
        &self,
        node_name: &str,
        node_ip: &str,
        node_port: i64,
        now_secs: u64,
    ) -> Result<(), PeerError> {
        self.init_single(NodeRole::Leader, node_name, node_ip, node_port, now_secs)?;
        if let Some(node) = lock(&self.inner).this_node.as_mut() {
            // `CheckHeartBeat` sets this before starting the timeout thread.
            node.set_heartbeat_interval(1000);
        }
        Ok(())
    }

    pub fn init_as_follower(
        &self,
        node_name: &str,
        node_ip: &str,
        node_port: i64,
        leader_ip: &str,
        leader_port: i64,
        now_secs: u64,
    ) -> Result<(), PeerError> {
        self.init_reader(
            NodeRole::Follower,
            node_name,
            node_ip,
            node_port,
            leader_ip,
            leader_port,
            now_secs,
        )
    }

    pub fn init_as_learner(
        &self,
        node_name: &str,
        node_ip: &str,
        node_port: i64,
        leader_ip: &str,
        leader_port: i64,
        now_secs: u64,
    ) -> Result<(), PeerError> {
        self.init_reader(
            NodeRole::Learner,
            node_name,
            node_ip,
            node_port,
            leader_ip,
            leader_port,
            now_secs,
        )
    }

    #[allow(clippy::too_many_arguments)]
    fn init_reader(
        &self,
        role: NodeRole,
        node_name: &str,
        node_ip: &str,
        node_port: i64,
        leader_ip: &str,
        leader_port: i64,
        now_secs: u64,
    ) -> Result<(), PeerError> {
        self.init_single(role, node_name, node_ip, node_port, now_secs)?;
        let mut inner = lock(&self.inner);
        inner.leader_node = Some(NodeInfo::endpoint(NodeRole::Leader, leader_ip, leader_port));
        Ok(())
    }

    /// Leader-only `CheckHeartBeat`: bump the leader clock and time out nodes
    /// whose last update is more than [`HEARTBEAT_TIMEOUT_SECS`] old.
    pub fn check_heartbeat(&self, now_secs: u64) -> Result<Vec<String>, PeerError> {
        let mut inner = lock(&self.inner);
        if inner.current_role != NodeRole::Leader {
            return Err(PeerError::rejected(
                error_code::INVALID_NODE_ROLE,
                "Invalid node role.",
            ));
        }
        if let Some(this) = inner.this_node.as_mut() {
            this.set_update_ts(now_secs);
        }
        let mut timed_out = Vec::new();
        for (name, node) in inner.other_nodes.iter_mut() {
            if node.node_status() == NodeStatus::Alive
                && node.update_ts() + HEARTBEAT_TIMEOUT_SECS < now_secs
            {
                node.set_node_status(NodeStatus::Timeout);
                timed_out.push(name.clone());
            }
        }
        Ok(timed_out)
    }

    /// Leader-side registration. The follower-limit check is a faithful port of
    /// the C++ `follower_count + 1 == follower_limit_` condition (so with the
    /// default limit of 4, the fourth follower is rejected).
    pub fn add_node_info(&self, new_node: &NodeInfo) -> Result<(), PeerError> {
        let mut inner = lock(&self.inner);
        if inner.current_role != NodeRole::Leader {
            return Err(PeerError::rejected(
                error_code::INVALID_NODE_ROLE,
                "Non-leader role can't add other node.",
            ));
        }
        let this_node = inner.this_node.as_ref().ok_or_else(|| {
            PeerError::rejected(error_code::UNEXPECTED_ERROR, "Leader is not initialized.")
        })?;
        if new_node.node_name() == this_node.node_name() {
            return Err(PeerError::rejected(
                error_code::DUPLICATE_NODE,
                format!("Duplicate node: {}", new_node.node_name()),
            ));
        }
        if new_node.node_ip() == this_node.node_ip()
            && new_node.node_port() == this_node.node_port()
        {
            return Err(PeerError::rejected(
                error_code::INVALID_SERVER_ADDRESS,
                format!(
                    "Follower or learner peer server address {}:{} is the same as leader",
                    new_node.node_ip(),
                    new_node.node_port()
                ),
            ));
        }
        if inner.other_nodes.contains_key(new_node.node_name()) {
            return Err(PeerError::rejected(
                error_code::DUPLICATE_NODE,
                format!("Duplicate node: {}", new_node.node_name()),
            ));
        }

        let follower_count = inner
            .other_nodes
            .values()
            .filter(|node| node.node_role() == NodeRole::Follower)
            .count();
        let learner_count = inner
            .other_nodes
            .values()
            .filter(|node| node.node_role() == NodeRole::Learner)
            .count();
        match new_node.node_role() {
            NodeRole::Follower => {
                if follower_count + 1 == usize::from(self.follower_limit.load(Ordering::SeqCst)) {
                    return Err(PeerError::rejected(
                        error_code::TOO_MANY_FOLLOWER,
                        format!(
                            "Too many followers: {}",
                            self.follower_limit.load(Ordering::SeqCst)
                        ),
                    ));
                }
            }
            NodeRole::Learner => {
                if learner_count + 1 == usize::from(u8::MAX) {
                    return Err(PeerError::rejected(
                        error_code::TOO_MANY_LEARNER,
                        "Too many learners.".to_string(),
                    ));
                }
            }
            other => {
                return Err(PeerError::rejected(
                    error_code::INVALID_NODE_ROLE,
                    format!("Invalid node role: {other}"),
                ));
            }
        }

        inner
            .other_nodes
            .insert(new_node.node_name().to_string(), new_node.clone());
        Ok(())
    }

    pub fn remove_node_info(&self, node_name: &str) -> Result<(), PeerError> {
        let mut inner = lock(&self.inner);
        if inner.current_role != NodeRole::Leader {
            return Err(PeerError::rejected(
                error_code::INVALID_NODE_ROLE,
                format!("Can't remove node in {} mode", inner.current_role),
            ));
        }
        if inner
            .this_node
            .as_ref()
            .is_some_and(|node| node.node_name() == node_name)
        {
            return Err(PeerError::rejected(
                error_code::INVALID_NODE_ROLE,
                format!("Can't remove current node: {node_name}"),
            ));
        }
        if inner.other_nodes.remove(node_name).is_none() {
            return Err(PeerError::rejected(
                error_code::NOT_EXIST_NODE,
                format!("Not exist node: {node_name}"),
            ));
        }
        Ok(())
    }

    pub fn update_node_by_leader(
        &self,
        node_name: &str,
        op: UpdateNodeOp,
    ) -> Result<(), PeerError> {
        let mut inner = lock(&self.inner);
        match op {
            UpdateNodeOp::Remove => {
                if inner.other_nodes.remove(node_name).is_none() {
                    return Err(PeerError::rejected(
                        error_code::NOT_EXIST_NODE,
                        format!("Attempt to remove non-exist node: {node_name}"),
                    ));
                }
            }
            UpdateNodeOp::LostConnection => {
                let node = inner.other_nodes.get_mut(node_name).ok_or_else(|| {
                    PeerError::rejected(
                        error_code::NOT_EXIST_NODE,
                        format!("Attempt to disconnect from non-exist node: {node_name}"),
                    )
                })?;
                node.set_node_status(NodeStatus::LostConnection);
            }
        }
        Ok(())
    }

    /// Leader-side heartbeat processing (`UpdateNodeInfoByHeartBeat`). The
    /// response fields and the sender status transitions are preserved.
    pub fn update_node_info_by_heartbeat(
        &self,
        non_leader: &NodeInfo,
        now_secs: u64,
    ) -> HeartbeatUpdate {
        let mut inner = lock(&self.inner);
        let mut update = HeartbeatUpdate {
            other_nodes: Vec::new(),
            leader_term: 0,
            sender_status: NodeStatus::Invalid,
            error_code: 0,
            error_msg: String::new(),
        };
        if inner.current_role != NodeRole::Leader {
            update.sender_status = NodeStatus::Removed;
            update.error_code = error_code::INVALID_NODE_ROLE;
            update.error_msg = format!("Leader node is already switch to {}", inner.current_role);
            return update;
        }
        update.leader_term = inner
            .this_node
            .as_ref()
            .map(NodeInfo::leader_term)
            .unwrap_or(0);

        // Encode every other node for the response. Invalid states are an error,
        // exactly as in the C++ loop.
        for (name, node) in inner.other_nodes.iter() {
            if name == non_leader.node_name() {
                continue;
            }
            match node.node_role() {
                NodeRole::Leader | NodeRole::Follower | NodeRole::Learner => {}
                other => {
                    update.error_code = error_code::INVALID_NODE_ROLE;
                    update.error_msg = format!("Invalid node role of node: {name}: {other}");
                    return update;
                }
            }
            match node.node_status() {
                NodeStatus::Alive | NodeStatus::Timeout => {}
                other => {
                    update.error_code = error_code::INVALID_NODE_STATUS;
                    update.error_msg = format!("Invalid node status : {other} of node: {name}");
                    return update;
                }
            }
            update.other_nodes.push(node.clone());
        }

        let Some(node) = inner.other_nodes.get_mut(non_leader.node_name()) else {
            update.sender_status = NodeStatus::LostConnection;
            update.error_code = error_code::NOT_REGISTERED;
            update.error_msg = format!("Node {} is not registered", non_leader.node_name());
            return update;
        };
        if node.node_ip() != non_leader.node_ip() || node.node_port() != non_leader.node_port() {
            update.sender_status = NodeStatus::Timeout;
            update.error_code = error_code::NODE_INFO_UPDATED;
            update.error_msg = format!(
                "Node {}, IP address: {} or port: {} is changed.",
                node.node_name(),
                node.node_ip(),
                node.node_port()
            );
            return update;
        }
        match node.node_status() {
            NodeStatus::Alive | NodeStatus::Timeout => {
                node.set_txn_ts(non_leader.txn_ts());
                node.set_update_ts(now_secs);
                node.increase_heartbeat_count();
                update.sender_status = NodeStatus::Alive;
            }
            NodeStatus::Removed => update.sender_status = NodeStatus::Removed,
            NodeStatus::LostConnection => update.sender_status = NodeStatus::LostConnection,
            NodeStatus::Invalid => {
                update.error_code = error_code::INVALID_NODE_STATUS;
                update.error_msg = "Invalid node status".to_string();
            }
        }
        update
    }

    /// Follower/learner registration response (`RegisterToLeaderNoLock`).
    pub fn apply_register_response(
        &self,
        response: &RegisterResponse,
        now_secs: u64,
    ) -> Result<(), PeerError> {
        if response.error_code != 0 {
            return Err(PeerError::rejected(
                response.error_code,
                format!("From leader: {}", response.error_msg),
            ));
        }
        let mut inner = lock(&self.inner);
        if !inner.current_role.is_non_leader() {
            return Err(PeerError::rejected(
                error_code::INVALID_NODE_ROLE,
                "Only follower or learner can register to leader.",
            ));
        }
        let leader = inner.leader_node.as_mut().ok_or_else(|| {
            PeerError::rejected(error_code::UNEXPECTED_ERROR, "Leader is not initialized.")
        })?;
        leader.set_update_ts(now_secs);
        leader.set_node_name(response.leader_name.clone());
        leader.set_node_status(NodeStatus::Alive);
        leader.set_leader_term(response.leader_term);
        leader.set_heartbeat_interval(if response.heart_beat_interval == 0 {
            1000
        } else {
            response.heart_beat_interval
        });
        Ok(())
    }

    /// Follower/learner heartbeat result (`HeartBeatToLeaderThread`).
    pub fn apply_heartbeat_response(
        &self,
        response: &HeartBeatResponse,
        now_secs: u64,
    ) -> Result<(), PeerError> {
        let mut inner = lock(&self.inner);
        if !inner.current_role.is_non_leader() {
            return Err(PeerError::rejected(
                error_code::INVALID_NODE_ROLE,
                "Only follower or learner sends heartbeats.",
            ));
        }
        if response.error_code != 0 {
            let status = if response.error_code == error_code::NOT_REGISTERED {
                response.sender_status
            } else {
                NodeStatus::Timeout
            };
            if let Some(leader) = inner.leader_node.as_mut() {
                leader.set_node_status(status);
            }
            return Err(PeerError::rejected(
                response.error_code,
                response.error_msg.clone(),
            ));
        }
        if let Some(leader) = inner.leader_node.as_mut() {
            leader.set_node_status(NodeStatus::Alive);
            leader.set_update_ts(now_secs);
            leader.set_leader_term(response.leader_term);
        }
        if let Some(this) = inner.this_node.as_mut() {
            this.increase_heartbeat_count();
            this.set_node_status(response.sender_status);
        }
        Self::update_node_info_locked(&mut inner, &response.other_nodes, now_secs);
        Ok(())
    }

    /// `UpdateNodeInfoNoLock`: replace the leader-provided view of all nodes.
    pub fn update_node_info_no_lock(&self, infos: &[NodeInfo], now_secs: u64) {
        let mut inner = lock(&self.inner);
        Self::update_node_info_locked(&mut inner, infos, now_secs);
    }

    fn update_node_info_locked(inner: &mut ClusterInner, infos: &[NodeInfo], now_secs: u64) {
        let mut seen = std::collections::BTreeSet::new();
        for info in infos {
            seen.insert(info.node_name().to_string());
            match inner.other_nodes.get_mut(info.node_name()) {
                Some(existing) => {
                    existing.set_txn_ts(info.txn_ts());
                    existing.set_node_ip(info.node_ip());
                    existing.set_node_port(info.node_port());
                    existing.set_node_role(info.node_role());
                    existing.set_node_status(info.node_status());
                    existing.set_update_ts(now_secs);
                }
                None => {
                    let mut node = info.clone();
                    node.set_update_ts(now_secs);
                    inner.other_nodes.insert(node.node_name().to_string(), node);
                }
            }
        }
        inner.other_nodes.retain(|name, _| seen.contains(name));
    }

    /// Heartbeat interval learned from the leader (milliseconds).
    pub fn heartbeat_interval_ms(&self) -> i64 {
        lock(&self.inner)
            .leader_node
            .as_ref()
            .map(NodeInfo::heartbeat_interval)
            .unwrap_or(0)
    }

    /// `UnInit`: returns the unregister task the caller should send (if any).
    pub fn uninit(&self, not_unregister: bool) -> Option<UnregisterPeerTask> {
        let mut inner = lock(&self.inner);
        let unregister = if !not_unregister
            && inner.current_role.is_non_leader()
            && inner
                .leader_node
                .as_ref()
                .is_some_and(|leader| leader.node_status() == NodeStatus::Alive)
        {
            inner
                .this_node
                .as_ref()
                .map(|node| UnregisterPeerTask::new(node.node_name().to_string()))
        } else {
            None
        };
        inner.this_node = None;
        inner.leader_node = None;
        inner.other_nodes.clear();
        inner.current_role = NodeRole::UnInitialized;
        unregister
    }

    /// `ChangeServerRole`: only switching to admin is supported.
    pub fn change_server_role(&self, role: NodeRole, force: bool) -> Result<(), PeerError> {
        if role != NodeRole::Admin {
            return Err(PeerError::rejected(
                error_code::CANT_SWITCH_ROLE,
                format!("Not support to change to role: {role}"),
            ));
        }
        let mut inner = lock(&self.inner);
        if !force && inner.current_role == NodeRole::UnInitialized {
            return Err(PeerError::rejected(
                error_code::CANT_SWITCH_ROLE,
                format!("Can't switch role from {}", inner.current_role),
            ));
        }
        inner.current_role = role;
        if let Some(this) = inner.this_node.as_mut() {
            this.set_node_role(role);
        }
        Ok(())
    }

    pub fn set_follower_number(&self, new_limit: usize) -> Result<(), PeerError> {
        if new_limit > 5 {
            return Err(PeerError::rejected(
                error_code::UNEXPECTED_ERROR,
                "Attempt to set follower count larger than 5.",
            ));
        }
        self.follower_limit.store(new_limit as u8, Ordering::SeqCst);
        Ok(())
    }

    pub fn follower_limit(&self) -> usize {
        usize::from(self.follower_limit.load(Ordering::SeqCst))
    }

    /// `ListNodes`: this node, then the leader for readers, then the rest in
    /// name order (the C++ `std::map` ordering).
    pub fn list_nodes(&self) -> Vec<NodeInfo> {
        let inner = lock(&self.inner);
        let mut nodes = Vec::with_capacity(inner.other_nodes.len() + 2);
        if let Some(this) = inner.this_node.clone() {
            nodes.push(this);
        }
        if inner.current_role.is_non_leader() {
            if let Some(leader) = inner.leader_node.clone() {
                nodes.push(leader);
            }
        }
        nodes.extend(inner.other_nodes.values().cloned());
        nodes
    }

    pub fn get_node_info_by_name(&self, node_name: &str) -> Result<NodeInfo, PeerError> {
        let inner = lock(&self.inner);
        if matches!(
            inner.current_role,
            NodeRole::Admin | NodeRole::Standalone | NodeRole::UnInitialized
        ) {
            return Err(PeerError::rejected(
                error_code::INVALID_NODE_ROLE,
                "Error node role type",
            ));
        }
        if inner.current_role.is_non_leader() {
            if let Some(leader) = inner.leader_node.as_ref() {
                if leader.node_name() == node_name {
                    return Ok(leader.clone());
                }
            }
        }
        if let Some(this) = inner.this_node.as_ref() {
            if this.node_name() == node_name {
                return Ok(this.clone());
            }
        }
        inner.other_nodes.get(node_name).cloned().ok_or_else(|| {
            PeerError::rejected(
                error_code::NOT_EXIST_NODE,
                format!("Not exist node: {node_name}"),
            )
        })
    }

    /// Records that a replicated log batch was applied.
    pub fn record_applied_logs(&self, count: usize) -> u64 {
        self.applied_log_count
            .fetch_add(count as u64, Ordering::SeqCst)
            + count as u64
    }
}

// ---------------------------------------------------------------------------
// PeerServerThriftService
// ---------------------------------------------------------------------------

/// `PeerServerThriftService`: leader-side RPC handler. Every method delegates to
/// [`ClusterManager`] and fills the Thrift response struct exactly like the C++
/// service does.
pub struct PeerServerThriftService {
    cluster: Arc<ClusterManager>,
    log_sink: Arc<dyn LogSyncSink>,
}

impl PeerServerThriftService {
    pub fn new(cluster: Arc<ClusterManager>, log_sink: Arc<dyn LogSyncSink>) -> Self {
        Self { cluster, log_sink }
    }

    pub fn cluster(&self) -> &Arc<ClusterManager> {
        &self.cluster
    }

    pub fn register(&self, request: &RegisterRequest, now_secs: u64) -> RegisterResponse {
        let mut response = RegisterResponse::default();
        if self.cluster.role() != NodeRole::Leader {
            response.error_code = error_code::INVALID_NODE_ROLE;
            response.error_msg = "Attempt to register with a non-leader node".to_string();
            return response;
        }
        let node_role = match request.node_type {
            NodeType::Follower => NodeRole::Follower,
            NodeType::Learner => NodeRole::Learner,
            other => {
                response.error_code = error_code::INVALID_NODE_ROLE;
                response.error_msg = format!("Invalid node type: {other}");
                return response;
            }
        };
        let node = NodeInfo::new(
            node_role,
            NodeStatus::Alive,
            request.node_name.clone(),
            request.node_ip.clone(),
            request.node_port,
            request.txn_timestamp,
            now_secs,
        );
        match self.cluster.add_node_info(&node) {
            Ok(()) => {
                if let Some(leader) = self.cluster.this_node() {
                    response.leader_name = leader.node_name().to_string();
                    response.leader_term = leader.leader_term();
                    response.heart_beat_interval = leader.heartbeat_interval();
                }
            }
            Err(error) => {
                response.error_code = error.code();
                response.error_msg = match error {
                    PeerError::Transport(message) => message,
                    PeerError::Rejected { message, .. } => message,
                };
            }
        }
        response
    }

    pub fn unregister(&self, request: &UnregisterRequest) -> UnregisterResponse {
        let mut response = UnregisterResponse::default();
        if self.cluster.role() != NodeRole::Leader {
            response.error_code = error_code::INVALID_NODE_ROLE;
            response.error_msg = "Attempt to unregister from a non-leader node".to_string();
            return response;
        }
        if let Err(error) = self
            .cluster
            .update_node_by_leader(&request.node_name, UpdateNodeOp::Remove)
        {
            response.error_code = error.code();
            response.error_msg = error.to_string();
        }
        response
    }

    pub fn heartbeat(&self, request: &HeartBeatRequest, now_secs: u64) -> HeartBeatResponse {
        let mut response = HeartBeatResponse::default();
        if self.cluster.role() != NodeRole::Leader {
            response.error_code = error_code::INVALID_NODE_ROLE;
            response.sender_status = NodeStatus::Removed;
            response.error_msg = format!(
                "Attempt to heartbeat from a non-leader node: {}",
                self.cluster.role()
            );
            return response;
        }
        let node_role = match request.node_type {
            NodeType::Leader => NodeRole::Leader,
            NodeType::Follower => NodeRole::Follower,
            NodeType::Learner => NodeRole::Learner,
            other => {
                response.error_code = error_code::INVALID_NODE_ROLE;
                response.error_msg = format!("Invalid node type: {other}");
                return response;
            }
        };
        let node = NodeInfo::new(
            node_role,
            NodeStatus::Alive,
            request.node_name.clone(),
            request.node_ip.clone(),
            request.node_port,
            request.txn_timestamp,
            now_secs,
        );
        let update = self.cluster.update_node_info_by_heartbeat(&node, now_secs);
        response.error_code = update.error_code;
        response.error_msg = update.error_msg;
        response.leader_term = update.leader_term;
        response.other_nodes = update.other_nodes;
        response.sender_status = update.sender_status;
        response
    }

    pub fn sync_log(&self, request: &SyncLogRequest) -> SyncLogResponse {
        let mut response = SyncLogResponse::default();
        if request.log_entries.is_empty() {
            response.error_code = error_code::UNEXPECTED_ERROR;
            response.error_msg = "No log is synced from leader node".to_string();
            return response;
        }
        match self
            .log_sink
            .apply(&request.node_name, &request.log_entries, request.on_startup)
        {
            Ok(()) => {
                let total = self.cluster.record_applied_logs(request.log_entries.len());
                response.txn_timestamp = total as i64;
            }
            Err((code, message)) => {
                response.error_code = code;
                response.error_msg = message;
            }
        }
        response
    }

    pub fn change_role(&self, request: &ChangeRoleRequest) -> ChangeRoleResponse {
        let mut response = ChangeRoleResponse::default();
        if request.node_type != NodeType::Admin {
            response.error_code = error_code::INVALID_NODE_ROLE;
            response.error_msg = "Not support to change to other type of node".to_string();
            return response;
        }
        if let Err(error) = self.cluster.change_server_role(NodeRole::Admin, true) {
            response.error_code = error.code();
            response.error_msg = error.to_string();
        }
        response
    }

    /// `NewLeader` is a no-op in C++ (`LOG_INFO` then return).
    pub fn new_leader(&self, _request: &NewLeaderRequest) -> NewLeaderResponse {
        NewLeaderResponse::default()
    }
}

/// The in-process replacement for the generated Thrift client. It forwards each
/// call straight to a [`PeerServerThriftService`], with an explicit connected
/// flag so the retry/reconnect path can be exercised without a socket.
pub struct InProcessPeerTransport {
    service: Arc<PeerServerThriftService>,
    connected: AtomicBool,
    now_secs: AtomicU64,
    from_node_name: Option<String>,
}

impl InProcessPeerTransport {
    pub fn new(service: Arc<PeerServerThriftService>) -> Self {
        Self {
            service,
            connected: AtomicBool::new(true),
            now_secs: AtomicU64::new(system_now_secs()),
            from_node_name: None,
        }
    }

    pub fn with_from_node_name(mut self, name: impl Into<String>) -> Self {
        self.from_node_name = Some(name.into());
        self
    }

    pub fn service(&self) -> &Arc<PeerServerThriftService> {
        &self.service
    }

    pub fn set_now_secs(&self, now_secs: u64) {
        self.now_secs.store(now_secs, Ordering::SeqCst);
    }

    pub fn set_connected(&self, connected: bool) {
        self.connected.store(connected, Ordering::SeqCst);
    }

    fn ensure_connected(&self) -> Result<(), PeerError> {
        if self.connected.load(Ordering::SeqCst) {
            Ok(())
        } else {
            Err(PeerError::Transport(
                "peer transport is not connected".to_string(),
            ))
        }
    }
}

impl PeerTransport for InProcessPeerTransport {
    fn register(&self, request: &RegisterRequest) -> Result<RegisterResponse, PeerError> {
        self.ensure_connected()?;
        Ok(self.service.register(request, self.now_secs()))
    }

    fn unregister(&self, request: &UnregisterRequest) -> Result<UnregisterResponse, PeerError> {
        self.ensure_connected()?;
        Ok(self.service.unregister(request))
    }

    fn heartbeat(&self, request: &HeartBeatRequest) -> Result<HeartBeatResponse, PeerError> {
        self.ensure_connected()?;
        Ok(self.service.heartbeat(request, self.now_secs()))
    }

    fn sync_log(&self, request: &SyncLogRequest) -> Result<SyncLogResponse, PeerError> {
        self.ensure_connected()?;
        Ok(self.service.sync_log(request))
    }

    fn change_role(&self, request: &ChangeRoleRequest) -> Result<ChangeRoleResponse, PeerError> {
        self.ensure_connected()?;
        Ok(self.service.change_role(request))
    }

    fn new_leader(&self, request: &NewLeaderRequest) -> Result<NewLeaderResponse, PeerError> {
        self.ensure_connected()?;
        Ok(self.service.new_leader(request))
    }

    fn connect(&self) -> Result<(), PeerError> {
        self.connected.store(true, Ordering::SeqCst);
        Ok(())
    }

    fn disconnect(&self) -> Result<(), PeerError> {
        self.connected.store(false, Ordering::SeqCst);
        Ok(())
    }

    fn is_connected(&self) -> bool {
        self.connected.load(Ordering::SeqCst)
    }

    fn local_node_name(&self) -> Option<String> {
        self.from_node_name.clone()
    }

    fn now_secs(&self) -> u64 {
        self.now_secs.load(Ordering::SeqCst)
    }
}

// ---------------------------------------------------------------------------
// PeerClient
// ---------------------------------------------------------------------------

/// Default `DEFAULT_PEER_RETRY_COUNT` from `src/common/default_values.cppm`.
pub const DEFAULT_PEER_RETRY_COUNT: i64 = 2;
/// Default `DEFAULT_PEER_RETRY_DELAY` (1 second).
pub const DEFAULT_PEER_RETRY_DELAY_MS: u64 = 1000;

/// `PeerClient`: owns a task queue and a worker that drains it, mirroring the C++
/// `Process` + `Call` retry loop. The Rust version is pull-based
/// ([`PeerClient::process_pending`]) instead of spawning a thread, which keeps the
/// dispatch deterministic and testable; [`PeerClient::send_sync`] gives the same
/// enqueue / wait behaviour the cluster manager uses.
pub struct PeerClient {
    from_node_name: String,
    ip_address: String,
    port: i64,
    transport: Arc<dyn PeerTransport>,
    connected: AtomicBool,
    queue: Mutex<VecDeque<Arc<dyn PeerTask>>>,
    queued_count: AtomicUsize,
    processed_count: AtomicU64,
    retry_count: i64,
    retry_delay: Duration,
}

impl PeerClient {
    pub fn new(
        from_node_name: impl Into<String>,
        ip_address: impl Into<String>,
        port: i64,
        transport: Arc<dyn PeerTransport>,
    ) -> Self {
        Self {
            from_node_name: from_node_name.into(),
            ip_address: ip_address.into(),
            port,
            transport,
            connected: AtomicBool::new(false),
            queue: Mutex::new(VecDeque::new()),
            queued_count: AtomicUsize::new(0),
            processed_count: AtomicU64::new(0),
            retry_count: DEFAULT_PEER_RETRY_COUNT,
            retry_delay: Duration::from_millis(DEFAULT_PEER_RETRY_DELAY_MS),
        }
    }

    pub fn with_retry_policy(mut self, retry_count: i64, retry_delay: Duration) -> Self {
        self.retry_count = retry_count;
        self.retry_delay = retry_delay;
        self
    }

    pub fn local_node_name(&self) -> &str {
        &self.from_node_name
    }

    pub fn ip_address(&self) -> &str {
        &self.ip_address
    }

    pub fn port(&self) -> i64 {
        self.port
    }

    pub fn init(&self) -> Result<(), PeerError> {
        self.reconnect()
    }

    pub fn reconnect(&self) -> Result<(), PeerError> {
        if self.connected.load(Ordering::SeqCst) && self.transport.is_connected() {
            return Ok(());
        }
        self.transport.connect()?;
        self.connected.store(true, Ordering::SeqCst);
        Ok(())
    }

    pub fn disconnect(&self) -> Result<(), PeerError> {
        self.connected.store(false, Ordering::SeqCst);
        self.transport.disconnect()
    }

    pub fn server_connected(&self) -> bool {
        self.connected.load(Ordering::SeqCst) && self.transport.is_connected()
    }

    pub fn queued_count(&self) -> usize {
        self.queued_count.load(Ordering::SeqCst)
    }

    pub fn processed_count(&self) -> u64 {
        self.processed_count.load(Ordering::SeqCst)
    }

    /// `PeerClient::Send`. The terminate task is reserved for `uninit`, exactly as
    /// in C++ where sending it through `Send` is unrecoverable.
    pub fn send(&self, task: Arc<dyn PeerTask>) -> Result<(), PeerError> {
        if task.task_type() == PeerTaskType::Terminate {
            return Err(PeerError::rejected(
                error_code::UNEXPECTED_ERROR,
                "Terminate the background processor",
            ));
        }
        lock(&self.queue).push_back(task);
        self.queued_count.fetch_add(1, Ordering::SeqCst);
        Ok(())
    }

    /// `PeerClient::UnInit(sync)`: enqueue a terminate task and drain. When
    /// `sync` is false the terminate task is async and completion is immediate.
    pub fn uninit(&self, sync: bool) {
        let terminate: Arc<dyn PeerTask> = Arc::new(TerminatePeerTask::new(!sync));
        lock(&self.queue).push_back(terminate);
        self.queued_count.fetch_add(1, Ordering::SeqCst);
        self.process_pending();
        let _ = self.disconnect();
    }

    /// Sends a task and waits for its completion (the C++ `Send` + `Wait` pair
    /// used by every cluster-manager operation).
    pub fn send_sync(&self, task: Arc<dyn PeerTask>) -> Result<(), PeerError> {
        self.send(task.clone())?;
        self.process_pending();
        task.wait();
        Ok(())
    }

    /// Drains the queue once, executing each task with the retry policy from
    /// `PeerClient::Call`. Returns the number of completed tasks.
    pub fn process_pending(&self) -> usize {
        let mut tasks = Vec::new();
        {
            let mut queue = lock(&self.queue);
            while let Some(task) = queue.pop_front() {
                tasks.push(task);
            }
        }
        self.queued_count.fetch_sub(tasks.len(), Ordering::SeqCst);

        let mut completed = 0;
        let mut stopped = false;
        let mut remaining = VecDeque::new();
        for task in tasks {
            if stopped {
                remaining.push_back(task);
                continue;
            }
            if task.task_type() == PeerTaskType::Terminate {
                task.complete();
                completed += 1;
                stopped = true;
                continue;
            }
            self.execute_with_retry(task.as_ref());
            task.complete();
            completed += 1;
        }
        if !remaining.is_empty() {
            let count = remaining.len();
            let mut queue = lock(&self.queue);
            while let Some(task) = remaining.pop_back() {
                queue.push_front(task);
            }
            self.queued_count.fetch_add(count, Ordering::SeqCst);
        }
        self.processed_count
            .fetch_add(completed as u64, Ordering::SeqCst);
        completed
    }

    fn execute_with_retry(&self, task: &dyn PeerTask) {
        for attempt in 0..=self.retry_count {
            match task.execute(self.transport.as_ref()) {
                Ok(()) => return,
                Err(PeerError::Transport(_)) if attempt < self.retry_count => {
                    // C++ reconnects before retrying (PeerClient::Call).
                    let _ = self.reconnect();
                    if !self.retry_delay.is_zero() {
                        std::thread::sleep(self.retry_delay);
                    }
                }
                Err(_) => return,
            }
        }
    }
}

// ---------------------------------------------------------------------------
// PoolPeerThriftServer
// ---------------------------------------------------------------------------

/// `PeerThriftServerStatus`.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum PeerThriftServerStatus {
    Stopped,
    Running,
    Stopping,
}

impl PeerThriftServerStatus {
    fn from_u8(value: u8) -> Self {
        match value {
            1 => Self::Running,
            2 => Self::Stopping,
            _ => Self::Stopped,
        }
    }
}

/// `PoolPeerThriftServer`: the pooled peer endpoint. Instead of a
/// `TThreadPoolServer` with `pool_size` worker threads, requests are round-robined
/// over `pool_size` logical workers in-process; the lifecycle state machine
/// (`Init` / `Start` / `Shutdown`) is a faithful port.
pub struct PoolPeerThriftServer {
    service: Arc<PeerServerThriftService>,
    address: Mutex<String>,
    port: Mutex<i32>,
    pool_size: Mutex<usize>,
    initialized: AtomicBool,
    status: AtomicU8,
    next_worker: AtomicUsize,
    worker_requests: Mutex<Vec<u64>>,
    now_secs: AtomicU64,
}

impl PoolPeerThriftServer {
    pub fn new(service: Arc<PeerServerThriftService>) -> Self {
        Self {
            service,
            address: Mutex::new(String::new()),
            port: Mutex::new(0),
            pool_size: Mutex::new(0),
            initialized: AtomicBool::new(false),
            status: AtomicU8::new(0),
            next_worker: AtomicUsize::new(0),
            worker_requests: Mutex::new(Vec::new()),
            now_secs: AtomicU64::new(system_now_secs()),
        }
    }

    pub fn init(
        &self,
        server_address: impl Into<String>,
        port_no: i32,
        pool_size: usize,
    ) -> Result<(), PeerError> {
        if pool_size == 0 {
            return Err(PeerError::rejected(
                error_code::UNEXPECTED_ERROR,
                "Peer server pool size must be positive.",
            ));
        }
        *lock(&self.address) = server_address.into();
        *lock(&self.port) = port_no;
        *lock(&self.pool_size) = pool_size;
        *lock(&self.worker_requests) = vec![0; pool_size];
        self.initialized.store(true, Ordering::SeqCst);
        Ok(())
    }

    pub fn start(&self) -> Result<(), PeerError> {
        if !self.initialized.load(Ordering::SeqCst) {
            return Err(PeerError::rejected(
                error_code::UNEXPECTED_ERROR,
                "Peer server is not initialized.",
            ));
        }
        if self
            .status
            .compare_exchange(0, 1, Ordering::SeqCst, Ordering::SeqCst)
            .is_err()
        {
            return Err(PeerError::rejected(
                error_code::UNEXPECTED_ERROR,
                format!("Peer server in unexpected state: {:?}", self.status()),
            ));
        }
        Ok(())
    }

    pub fn shutdown(&self) -> Result<(), PeerError> {
        match self
            .status
            .compare_exchange(1, 2, Ordering::SeqCst, Ordering::SeqCst)
        {
            Ok(_) => {
                // `server->stop()` then wait for the serve thread to settle.
                self.status.store(0, Ordering::SeqCst);
                Ok(())
            }
            Err(0) => Ok(()),
            Err(_) => Err(PeerError::rejected(
                error_code::UNEXPECTED_ERROR,
                "Peer server in unexpected state: stopping",
            )),
        }
    }

    pub fn status(&self) -> PeerThriftServerStatus {
        PeerThriftServerStatus::from_u8(self.status.load(Ordering::SeqCst))
    }

    pub fn address(&self) -> String {
        lock(&self.address).clone()
    }

    pub fn port(&self) -> i32 {
        *lock(&self.port)
    }

    pub fn pool_size(&self) -> usize {
        *lock(&self.pool_size)
    }

    pub fn set_now_secs(&self, now_secs: u64) {
        self.now_secs.store(now_secs, Ordering::SeqCst);
    }

    /// Per-worker dispatch counters. Each entry corresponds to one pool slot.
    pub fn worker_requests(&self) -> Vec<u64> {
        lock(&self.worker_requests).clone()
    }

    fn take_worker(&self) -> usize {
        let pool_size = *lock(&self.pool_size);
        let worker = self.next_worker.fetch_add(1, Ordering::SeqCst) % pool_size;
        let mut counts = lock(&self.worker_requests);
        counts[worker] += 1;
        worker
    }

    /// Handles one request on the next pool worker. The service call itself is
    /// the same `PeerServerThriftService` the single-threaded path uses.
    pub fn handle(&self, request: PeerRequest) -> Result<PeerResponse, PeerError> {
        if self.status() != PeerThriftServerStatus::Running {
            return Err(PeerError::rejected(
                error_code::UNEXPECTED_ERROR,
                "Peer server is not running.",
            ));
        }
        let _worker = self.take_worker();
        let now = self.now_secs.load(Ordering::SeqCst);
        Ok(match request {
            PeerRequest::Register(request) => {
                PeerResponse::Register(self.service.register(&request, now))
            }
            PeerRequest::Unregister(request) => {
                PeerResponse::Unregister(self.service.unregister(&request))
            }
            PeerRequest::HeartBeat(request) => {
                PeerResponse::HeartBeat(self.service.heartbeat(&request, now))
            }
            PeerRequest::SyncLog(request) => PeerResponse::SyncLog(self.service.sync_log(&request)),
            PeerRequest::ChangeRole(request) => {
                PeerResponse::ChangeRole(self.service.change_role(&request))
            }
            PeerRequest::NewLeader(request) => {
                PeerResponse::NewLeader(self.service.new_leader(&request))
            }
        })
    }
}

/// Pooled request envelope, corresponding to the six `PeerService` methods.
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum PeerRequest {
    Register(RegisterRequest),
    Unregister(UnregisterRequest),
    HeartBeat(HeartBeatRequest),
    SyncLog(SyncLogRequest),
    ChangeRole(ChangeRoleRequest),
    NewLeader(NewLeaderRequest),
}

/// Pooled response envelope.
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum PeerResponse {
    Register(RegisterResponse),
    Unregister(UnregisterResponse),
    HeartBeat(HeartBeatResponse),
    SyncLog(SyncLogResponse),
    ChangeRole(ChangeRoleResponse),
    NewLeader(NewLeaderResponse),
}

#[cfg(test)]
mod tests {
    use super::*;

    fn leader(cluster: &ClusterManager, now: u64) {
        cluster
            .init_as_leader("leader0", "127.0.0.1", 23817, now)
            .unwrap();
    }

    fn service_pair() -> (
        Arc<ClusterManager>,
        Arc<PeerServerThriftService>,
        Arc<RecordingLogSink>,
    ) {
        let cluster = Arc::new(ClusterManager::new());
        let sink = Arc::new(RecordingLogSink::default());
        let service = Arc::new(PeerServerThriftService::new(cluster.clone(), sink.clone()));
        (cluster, service, sink)
    }

    fn follower_request(name: &str, port: i64) -> RegisterRequest {
        RegisterRequest {
            node_name: name.to_string(),
            node_type: NodeType::Follower,
            node_ip: "127.0.0.2".to_string(),
            node_port: port,
            txn_timestamp: 100,
        }
    }

    fn heartbeat_request(name: &str, port: i64, txn: i64) -> HeartBeatRequest {
        HeartBeatRequest {
            node_name: name.to_string(),
            node_type: NodeType::Follower,
            node_ip: "127.0.0.2".to_string(),
            node_port: port,
            txn_timestamp: txn,
        }
    }

    #[test]
    fn register_and_heartbeat_follow_the_cpp_leader_rules() {
        let (cluster, service, _sink) = service_pair();
        leader(&cluster, 100);

        let registered = service.register(&follower_request("follower0", 23818), 100);
        assert_eq!(registered.error_code, 0);
        assert_eq!(registered.leader_name, "leader0");
        assert_eq!(registered.heart_beat_interval, 1000);
        assert_eq!(cluster.node_count(), 1);

        // Duplicate name and duplicate address are rejected.
        let duplicate = service.register(&follower_request("follower0", 23818), 101);
        assert_eq!(duplicate.error_code, error_code::DUPLICATE_NODE);

        let same_address = service.register(
            &RegisterRequest {
                node_ip: "127.0.0.1".to_string(),
                node_port: 23817,
                ..follower_request("follower1", 23819)
            },
            101,
        );
        assert_eq!(same_address.error_code, error_code::INVALID_SERVER_ADDRESS);

        // Heartbeat updates txn_ts and the heartbeat counter.
        let beat = service.heartbeat(&heartbeat_request("follower0", 23818, 120), 101);
        assert_eq!(beat.error_code, 0);
        assert_eq!(beat.sender_status, NodeStatus::Alive);
        assert_eq!(beat.leader_term, 0);
        let node = cluster.get_node_info_by_name("follower0").unwrap();
        assert_eq!(node.txn_ts(), 120);
        assert_eq!(node.heartbeat_count(), 1);
        assert_eq!(node.node_status(), NodeStatus::Alive);

        // A node silent for more than four seconds becomes kTimeout.
        assert!(cluster.check_heartbeat(104).unwrap().is_empty());
        assert_eq!(cluster.check_heartbeat(106).unwrap(), vec!["follower0"]);
        assert_eq!(
            cluster
                .get_node_info_by_name("follower0")
                .unwrap()
                .node_status(),
            NodeStatus::Timeout
        );

        // A later heartbeat keeps the status at kTimeout (C++ only refreshes ts).
        let beat = service.heartbeat(&heartbeat_request("follower0", 23818, 121), 107);
        assert_eq!(beat.sender_status, NodeStatus::Alive);
        assert_eq!(
            cluster
                .get_node_info_by_name("follower0")
                .unwrap()
                .node_status(),
            NodeStatus::Timeout
        );
    }

    #[test]
    fn heartbeat_rejects_unknown_moved_and_non_leader_nodes() {
        let (cluster, service, _sink) = service_pair();
        leader(&cluster, 100);
        service.register(&follower_request("follower0", 23818), 100);

        let unknown = service.heartbeat(&heartbeat_request("missing", 23820, 1), 101);
        assert_eq!(unknown.error_code, error_code::NOT_REGISTERED);
        assert_eq!(unknown.sender_status, NodeStatus::LostConnection);

        let moved = service.heartbeat(&heartbeat_request("follower0", 29999, 1), 101);
        assert_eq!(moved.error_code, error_code::NODE_INFO_UPDATED);
        assert_eq!(moved.sender_status, NodeStatus::Timeout);

        // Only leaders accept peer requests.
        let reader = Arc::new(ClusterManager::new());
        reader
            .init_as_follower("follower0", "127.0.0.2", 23818, "127.0.0.1", 23817, 100)
            .unwrap();
        let sink = Arc::new(RecordingLogSink::default());
        let reader_service = PeerServerThriftService::new(reader.clone(), sink);
        let rejected = reader_service.register(&follower_request("x", 1), 100);
        assert_eq!(rejected.error_code, error_code::INVALID_NODE_ROLE);
        let rejected = reader_service.heartbeat(&heartbeat_request("x", 1, 0), 100);
        assert_eq!(rejected.error_code, error_code::INVALID_NODE_ROLE);
        assert_eq!(rejected.sender_status, NodeStatus::Removed);
    }

    #[test]
    fn follower_limit_is_faithful_to_the_cpp_off_by_one_check() {
        let (cluster, service, _sink) = service_pair();
        leader(&cluster, 100);

        for (index, port) in (23818..23821).enumerate() {
            let response = service.register(&follower_request(&format!("f{index}"), port), 100);
            assert_eq!(response.error_code, 0, "follower {index} should register");
        }
        // 3 followers exist; 3 + 1 == follower_limit(4) -> rejected.
        let fourth = service.register(&follower_request("f3", 23821), 100);
        assert_eq!(fourth.error_code, error_code::TOO_MANY_FOLLOWER);

        // Admin/leader cannot register as a reader.
        let mut admin = follower_request("admin", 23822);
        admin.node_type = NodeType::Admin;
        assert_eq!(
            service.register(&admin, 100).error_code,
            error_code::INVALID_NODE_ROLE
        );

        assert_eq!(cluster.follower_limit(), 4);
        assert!(cluster.set_follower_number(6).is_err());
        cluster.set_follower_number(2).unwrap();
        assert_eq!(cluster.follower_limit(), 2);
    }

    #[test]
    fn unregister_and_uninit_produce_the_cpp_tasks() {
        let (cluster, service, _sink) = service_pair();
        leader(&cluster, 100);
        service.register(&follower_request("follower0", 23818), 100);

        let missing = service.unregister(&UnregisterRequest {
            node_name: "missing".to_string(),
        });
        assert_eq!(missing.error_code, error_code::NOT_EXIST_NODE);

        let removed = service.unregister(&UnregisterRequest {
            node_name: "follower0".to_string(),
        });
        assert_eq!(removed.error_code, 0);
        assert_eq!(cluster.node_count(), 0);

        // Follower uninit returns the unregister task when the leader is alive.
        let reader = ClusterManager::new();
        reader
            .init_as_follower("follower1", "127.0.0.2", 23819, "127.0.0.1", 23817, 100)
            .unwrap();
        reader
            .apply_register_response(
                &RegisterResponse {
                    leader_name: "leader0".to_string(),
                    leader_term: 7,
                    heart_beat_interval: 1000,
                    ..RegisterResponse::default()
                },
                100,
            )
            .unwrap();
        let task = reader.uninit(false).expect("unregister task");
        assert_eq!(task.to_string(), "unregister@follower1");
        assert_eq!(reader.role(), NodeRole::UnInitialized);
        assert!(reader.uninit(false).is_none());
    }

    #[test]
    fn follower_registration_and_heartbeat_state_machine() {
        let cluster = ClusterManager::new();
        cluster
            .init_as_follower("follower0", "127.0.0.2", 23818, "127.0.0.1", 23817, 100)
            .unwrap();

        // A zero heartbeat interval from the leader falls back to 1000 ms.
        cluster
            .apply_register_response(
                &RegisterResponse {
                    leader_name: "leader0".to_string(),
                    leader_term: 3,
                    heart_beat_interval: 0,
                    ..RegisterResponse::default()
                },
                101,
            )
            .unwrap();
        assert_eq!(cluster.heartbeat_interval_ms(), 1000);
        let leader = cluster.leader_node().unwrap();
        assert_eq!(leader.node_name(), "leader0");
        assert_eq!(leader.node_status(), NodeStatus::Alive);
        assert_eq!(leader.leader_term(), 3);

        // The two other nodes sent by the leader replace the local view.
        let other_a = NodeInfo::new(
            NodeRole::Follower,
            NodeStatus::Alive,
            "follower1",
            "127.0.0.3",
            23819,
            10,
            101,
        );
        let other_b = NodeInfo::new(
            NodeRole::Learner,
            NodeStatus::Alive,
            "learner0",
            "127.0.0.4",
            23820,
            11,
            101,
        );
        cluster
            .apply_heartbeat_response(
                &HeartBeatResponse {
                    leader_term: 4,
                    other_nodes: vec![other_a, other_b.clone()],
                    sender_status: NodeStatus::Alive,
                    ..HeartBeatResponse::default()
                },
                102,
            )
            .unwrap();
        assert_eq!(cluster.this_node().unwrap().heartbeat_count(), 1);
        assert_eq!(cluster.node_count(), 2);

        // The next heartbeat only reports one node; the missing one is removed.
        cluster
            .apply_heartbeat_response(
                &HeartBeatResponse {
                    leader_term: 5,
                    other_nodes: vec![other_b],
                    sender_status: NodeStatus::Alive,
                    ..HeartBeatResponse::default()
                },
                103,
            )
            .unwrap();
        assert_eq!(cluster.node_count(), 1);
        assert!(cluster.get_node_info_by_name("follower1").is_err());

        // A kNotRegistered error copies the leader's sender status; any other
        // error marks the leader as timed out.
        let failed = cluster.apply_heartbeat_response(
            &HeartBeatResponse {
                error_code: error_code::NOT_REGISTERED,
                error_msg: "gone".to_string(),
                sender_status: NodeStatus::Removed,
                ..HeartBeatResponse::default()
            },
            104,
        );
        assert!(failed.is_err());
        assert_eq!(
            cluster.leader_node().unwrap().node_status(),
            NodeStatus::Removed
        );
    }

    #[test]
    fn heartbeat_scheduler_uses_injected_time() {
        let mut scheduler = HeartbeatScheduler::new(1000);
        assert!(!scheduler.take_tick(0), "not running yet");

        scheduler.start(0);
        assert!(!scheduler.take_tick(999));
        assert!(scheduler.take_tick(1000));
        assert!(!scheduler.take_tick(1500));
        assert!(scheduler.take_tick(2000), "late wake still fires once");

        scheduler.set_interval_ms(500);
        assert!(!scheduler.take_tick(2400), "only 400ms since the last tick");
        assert!(scheduler.take_tick(2500));
        scheduler.stop();
        assert!(!scheduler.take_tick(10_000));
    }

    #[test]
    fn task_completion_and_string_rendering_match_cpp() {
        let terminate = TerminatePeerTask::new(false);
        assert!(!terminate.is_complete());
        terminate.complete();
        terminate.wait();
        assert!(terminate.is_complete());
        assert_eq!(terminate.to_string(), "terminate");

        // Async waits return immediately even before completion.
        let async_task = TerminatePeerTask::new(true);
        async_task.wait();
        assert!(async_task.is_complete());

        let register = RegisterPeerTask::new("f1", NodeRole::Follower, "10.0.0.2", 100, 7);
        assert_eq!(register.to_string(), "register@f1, 10.0.0.2:100, follower");
        assert_eq!(
            RegisterPeerTask::new("f1", NodeRole::Learner, "10.0.0.2", 100, 7).to_string(),
            "register@f1, 10.0.0.2:100, learner"
        );

        assert_eq!(UnregisterPeerTask::new("f1").to_string(), "unregister@f1");
        assert_eq!(
            HeartBeatPeerTask::new("f1", NodeRole::Follower, "10.0.0.2", 100, 9).to_string(),
            "heartbeat@f1, 9"
        );
        assert_eq!(
            SyncLogTask::new("f1", vec!["a".to_string(), "b".to_string()], false).to_string(),
            "log sync@f1, 2"
        );
        assert_eq!(
            ChangeRoleTask::new("f1", "admin").to_string(),
            "change role to admin"
        );
    }

    #[test]
    fn peer_client_dispatches_queued_tasks_and_filters_self() {
        let (cluster, service, _sink) = service_pair();
        leader(&cluster, 100);
        service.register(&follower_request("follower0", 23818), 100);
        service.register(
            &RegisterRequest {
                node_type: NodeType::Learner,
                ..follower_request("learner0", 23819)
            },
            100,
        );

        let transport =
            Arc::new(InProcessPeerTransport::new(service).with_from_node_name("follower0"));
        transport.set_now_secs(101);
        let client = PeerClient::new("follower0", "127.0.0.1", 23817, transport)
            .with_retry_policy(0, Duration::ZERO);
        client.init().unwrap();
        assert!(client.server_connected());

        let register = Arc::new(RegisterPeerTask::new(
            "follower0",
            NodeRole::Follower,
            "127.0.0.2",
            23818,
            100,
        ));
        client.send_sync(register.clone()).unwrap();
        // follower0 already exists, so the service reports a duplicate.
        assert_eq!(register.response().error_code, error_code::DUPLICATE_NODE);
        assert_eq!(client.queued_count(), 0);
        assert_eq!(client.processed_count(), 1);

        let heartbeat = Arc::new(HeartBeatPeerTask::new(
            "follower0",
            NodeRole::Follower,
            "127.0.0.2",
            23818,
            130,
        ));
        client.send_sync(heartbeat.clone()).unwrap();
        let response = heartbeat.response();
        assert_eq!(response.error_code, 0);
        // The caller never sees itself in the leader-provided node list.
        assert!(response
            .other_nodes
            .iter()
            .all(|node| node.node_name() != "follower0"));
        assert_eq!(response.other_nodes.len(), 1);
    }

    #[test]
    fn peer_client_retries_transport_errors_and_honours_terminate() {
        let (cluster, service, _sink) = service_pair();
        leader(&cluster, 100);
        service.register(&follower_request("follower0", 23818), 100);

        let transport = Arc::new(InProcessPeerTransport::new(service.clone()));
        transport.set_now_secs(101);
        transport.set_connected(false);
        let client = PeerClient::new("leader0", "127.0.0.1", 23817, transport.clone())
            .with_retry_policy(1, Duration::ZERO);
        // `reconnect` flips the in-process transport back on, so the retry succeeds.
        assert!(!client.server_connected());
        let heartbeat = Arc::new(HeartBeatPeerTask::new(
            "follower0",
            NodeRole::Follower,
            "127.0.0.2",
            23818,
            1,
        ));
        client.send_sync(heartbeat.clone()).unwrap();
        assert_eq!(heartbeat.response().error_code, 0);

        // Terminate is reserved for uninit and drains the queue.
        let pending = Arc::new(RegisterPeerTask::new(
            "follower0",
            NodeRole::Follower,
            "127.0.0.2",
            23818,
            1,
        ));
        client.send(pending.clone()).unwrap();
        client.uninit(true);
        assert!(!client.server_connected());
        assert_eq!(client.queued_count(), 0);
    }

    #[test]
    fn task_error_mapping_uses_cpp_error_codes() {
        struct DeadTransport;
        impl PeerTransport for DeadTransport {
            fn register(&self, _: &RegisterRequest) -> Result<RegisterResponse, PeerError> {
                Err(PeerError::Transport("down".to_string()))
            }
            fn unregister(&self, _: &UnregisterRequest) -> Result<UnregisterResponse, PeerError> {
                Err(PeerError::Transport("down".to_string()))
            }
            fn heartbeat(&self, _: &HeartBeatRequest) -> Result<HeartBeatResponse, PeerError> {
                Err(PeerError::Transport("down".to_string()))
            }
            fn sync_log(&self, _: &SyncLogRequest) -> Result<SyncLogResponse, PeerError> {
                Err(PeerError::Transport("down".to_string()))
            }
            fn change_role(&self, _: &ChangeRoleRequest) -> Result<ChangeRoleResponse, PeerError> {
                Err(PeerError::Transport("down".to_string()))
            }
            fn new_leader(&self, _: &NewLeaderRequest) -> Result<NewLeaderResponse, PeerError> {
                Err(PeerError::Transport("down".to_string()))
            }
        }

        let transport = DeadTransport;
        let register = RegisterPeerTask::new("f", NodeRole::Follower, "ip", 1, 0);
        assert!(register.execute(&transport).is_err());
        assert_eq!(
            register.response().error_code,
            error_code::CANT_CONNECT_LEADER
        );

        let unregister = UnregisterPeerTask::new("f");
        assert!(unregister.execute(&transport).is_err());
        assert_eq!(
            unregister.response().error_code,
            error_code::CANT_CONNECT_LEADER
        );

        let heartbeat = HeartBeatPeerTask::new("f", NodeRole::Follower, "ip", 1, 0);
        assert!(heartbeat.execute(&transport).is_err());
        assert_eq!(
            heartbeat.response().error_code,
            error_code::CANT_CONNECT_LEADER
        );

        let sync = SyncLogTask::new("f", vec!["e".to_string()], true);
        assert!(sync.execute(&transport).is_err());
        assert_eq!(sync.response().error_code, error_code::CANT_CONNECT_SERVER);

        let role = ChangeRoleTask::new("f", "admin");
        assert!(role.execute(&transport).is_err());
        assert_eq!(role.response().error_code, error_code::CANT_CONNECT_SERVER);
    }

    #[test]
    fn sync_log_applies_to_the_sink_and_records_progress() {
        let (cluster, service, sink) = service_pair();
        leader(&cluster, 100);

        let response = service.sync_log(&SyncLogRequest {
            node_name: "follower0".to_string(),
            log_entries: vec!["log-a".to_string(), "log-b".to_string()],
            on_startup: true,
        });
        assert_eq!(response.error_code, 0);
        assert_eq!(response.txn_timestamp, 2);
        assert_eq!(cluster.applied_log_count(), 2);
        let applied = sink.applied();
        assert_eq!(applied.len(), 1);
        assert_eq!(applied[0].0, "follower0");
        assert!(applied[0].2);

        let empty = service.sync_log(&SyncLogRequest {
            node_name: "follower0".to_string(),
            log_entries: Vec::new(),
            on_startup: false,
        });
        assert_eq!(empty.error_code, error_code::UNEXPECTED_ERROR);
        assert_eq!(cluster.applied_log_count(), 2);
    }

    #[test]
    fn change_role_only_supports_admin() {
        let (cluster, service, _sink) = service_pair();
        leader(&cluster, 100);

        let rejected = service.change_role(&ChangeRoleRequest {
            node_name: "follower0".to_string(),
            node_type: NodeType::Follower,
        });
        assert_eq!(rejected.error_code, error_code::INVALID_NODE_ROLE);
        assert_eq!(cluster.role(), NodeRole::Leader);

        let changed = service.change_role(&ChangeRoleRequest {
            node_name: "leader0".to_string(),
            node_type: NodeType::Admin,
        });
        assert_eq!(changed.error_code, 0);
        assert_eq!(cluster.role(), NodeRole::Admin);
        assert_eq!(cluster.this_node().unwrap().node_role(), NodeRole::Admin);
    }

    #[test]
    fn cluster_list_and_lookup_follow_node_role() {
        let cluster = ClusterManager::new();
        leader(&cluster, 100);
        for (index, port) in [23818, 23819].iter().enumerate() {
            let node = NodeInfo::new(
                NodeRole::Follower,
                NodeStatus::Alive,
                format!("f{index}"),
                "127.0.0.2",
                *port,
                0,
                100,
            );
            cluster.add_node_info(&node).unwrap();
        }
        let names: Vec<String> = cluster
            .list_nodes()
            .into_iter()
            .map(|node| node.node_name().to_string())
            .collect();
        assert_eq!(names, vec!["leader0", "f0", "f1"]);
        assert_eq!(
            cluster.get_node_info_by_name("f1").unwrap().node_port(),
            23819
        );
        assert_eq!(
            cluster.get_node_info_by_name("nope").unwrap_err().code(),
            error_code::NOT_EXIST_NODE
        );

        // Removing a node needs the leader role and an existing name.
        cluster
            .update_node_by_leader("f0", UpdateNodeOp::LostConnection)
            .unwrap();
        assert_eq!(
            cluster.get_node_info_by_name("f0").unwrap().node_status(),
            NodeStatus::LostConnection
        );
        cluster.remove_node_info("f0").unwrap();
        assert!(cluster.remove_node_info("f0").is_err());
        assert_eq!(cluster.node_count(), 1);
    }

    #[test]
    fn pooled_server_round_robins_and_enforces_lifecycle() {
        let (cluster, service, _sink) = service_pair();
        leader(&cluster, 100);
        let server = PoolPeerThriftServer::new(service);
        server.set_now_secs(100);

        assert!(server.start().is_err(), "must be initialized first");
        assert!(server.init("127.0.0.1", 23817, 0).is_err());
        server.init("127.0.0.1", 23817, 2).unwrap();
        assert_eq!(server.address(), "127.0.0.1");
        assert_eq!(server.pool_size(), 2);

        server.start().unwrap();
        assert_eq!(server.status(), PeerThriftServerStatus::Running);
        assert!(server.start().is_err(), "already running");

        for index in 0..5 {
            let response = server
                .handle(PeerRequest::Register(RegisterRequest {
                    node_name: format!("f{index}"),
                    node_type: NodeType::Learner,
                    ..follower_request("x", 23818 + index)
                }))
                .unwrap();
            match response {
                PeerResponse::Register(response) => assert_eq!(response.error_code, 0),
                other => panic!("unexpected response: {other:?}"),
            }
        }
        assert_eq!(server.worker_requests(), vec![3, 2]);

        server.shutdown().unwrap();
        assert_eq!(server.status(), PeerThriftServerStatus::Stopped);
        assert!(server
            .handle(PeerRequest::NewLeader(NewLeaderRequest::default()))
            .is_err());
        assert!(server.shutdown().is_ok(), "already stopped is a no-op");
        server.start().unwrap();
    }

    #[test]
    fn role_init_errors_are_reported() {
        let cluster = ClusterManager::new();
        leader(&cluster, 100);
        assert!(cluster
            .init_as_leader("other", "127.0.0.1", 1, 100)
            .is_err());
        assert!(cluster.check_heartbeat(100).is_ok());

        let reader = ClusterManager::new();
        reader
            .init_as_follower("f", "127.0.0.2", 1, "127.0.0.1", 2, 100)
            .unwrap();
        assert!(reader.check_heartbeat(100).is_err());
        assert!(reader.change_server_role(NodeRole::Learner, true).is_err());
    }
}

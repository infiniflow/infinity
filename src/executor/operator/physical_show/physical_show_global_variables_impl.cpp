module infinity_core:physical_show_global_variables.impl;

import :physical_show;
import :new_txn;
import :value;
import :value_expression;
import :data_block;
import :query_context;
import :operator_state;
import :result_cache_manager;
import :session_manager;
import :utility;
import :new_txn_manager;
import :background_process;
import :compaction_process;
import :system_info;
import :infinity_context;

import std;

using namespace infinity;

void PhysicalShow::ExecuteShowGlobalVariables(QueryContext *query_context, ShowOperatorState *operator_state) {
    // create data block for output state
    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();

    output_block_ptr->Init(*output_types_);

    for (auto &global_var_pair : VarUtil::global_name_map_) {
        const std::string &var_name = global_var_pair.first;
        GlobalVariable global_var_enum = global_var_pair.second;
        Config *config = query_context->global_config();
        switch (global_var_enum) {
            case GlobalVariable::kResultCache: {
                const std::string &result_cache_status = config->ResultCache();
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
                }
                {
                    // option value
                    Value value = Value::MakeVarchar(result_cache_status);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Result cache num");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
                }
                break;
            }
            case GlobalVariable::kCacheResultCapacity: {
                const std::string &result_cache_status = config->ResultCache();
                if (result_cache_status == "off") {
                    break;
                }
                ResultCacheManager *cache_mgr = query_context->storage()->GetResultCacheManagerPtr();
                size_t cache_num_capacity = cache_mgr->cache_num_capacity();
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
                }
                {
                    // option value
                    Value value = Value::MakeVarchar(std::to_string(cache_num_capacity));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Result cache capacity");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
                }
                break;
            }
            case GlobalVariable::kCacheResultNum: {
                const std::string &result_cache_status = config->ResultCache();
                if (result_cache_status == "off") {
                    break;
                }
                ResultCacheManager *cache_mgr = query_context->storage()->GetResultCacheManagerPtr();
                size_t cache_num_used = cache_mgr->cache_num_used();
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
                }
                {
                    // option value
                    Value value = Value::MakeVarchar(std::to_string(cache_num_used));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Result cache num");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
                }
                break;
            }
            case GlobalVariable::kMemoryCacheMiss: {
                BufferManager *buffer_manager = query_context->storage()->buffer_manager();
                u64 total_request_count = buffer_manager->TotalRequestCount();
                u64 cache_miss_count = buffer_manager->CacheMissCount();
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
                }
                {
                    // option value
                    Value value = Value::MakeVarchar(fmt::format("{}/{}", cache_miss_count, total_request_count));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Memory cache miss");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
                }
                break;
            }
            case GlobalVariable::kDiskCacheMiss: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
                }
                {
                    // option value
                    Value value = Value::MakeVarchar(fmt::format("{}/{}", VirtualStore::CacheMissCount(), VirtualStore::TotalRequestCount()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Disk cache miss");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
                }
                break;
            }
            case GlobalVariable::kQueryCount: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
                }
                {
                    // option value
                    Value value = Value::MakeVarchar(std::to_string(query_context->session_manager()->total_query_count()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Query count in total");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
                }
                break;
            }
            case GlobalVariable::kSessionCount: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
                }
                {
                    // option value
                    SessionManager *session_manager = query_context->session_manager();
                    u64 session_count = session_manager->GetSessionCount();
                    Value value = Value::MakeVarchar(std::to_string(session_count));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Session count in total");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
                }
                break;
            }
            case GlobalVariable::kBufferPoolUsage: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
                }
                {
                    // option value
                    BufferManager *buffer_manager = query_context->storage()->buffer_manager();
                    u64 memory_limit = buffer_manager->memory_limit();
                    u64 memory_usage = buffer_manager->memory_usage();
                    Value value =
                        Value::MakeVarchar(fmt::format("{}/{}", Utility::FormatByteSize(memory_usage), Utility::FormatByteSize(memory_limit)));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Buffer manager usage");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
                }
                break;
            }
            case GlobalVariable::kSchedulePolicy: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
                }
                {
                    // option value
                    Value value = Value::MakeVarchar("round robin");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Task scheduling policy");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
                }
                break;
            }
            case GlobalVariable::kNextTxnID: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
                }
                {
                    // option value
                    auto *new_txn_mgr = query_context->storage()->new_txn_manager();
                    Value value = Value::MakeVarchar(std::to_string(new_txn_mgr->current_transaction_id()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Next transaction id of system");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
                }
                break;
            }
            case GlobalVariable::kBufferedObjectCount: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
                }
                {
                    // option value
                    BufferManager *buffer_manager = query_context->storage()->buffer_manager();
                    Value value = Value::MakeVarchar(std::to_string(buffer_manager->BufferedObjectCount()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Buffered object count in buffer manager");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
                }
                break;
            }
            case GlobalVariable::kUnusedBufferObjectCount: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
                }
                {
                    // option value
                    BufferManager *buffer_manager = query_context->storage()->buffer_manager();
                    std::vector<size_t> size_list = buffer_manager->WaitingGCObjectCount();
                    size_t total_size = std::accumulate(size_list.begin(), size_list.end(), 0);
                    Value value = Value::MakeVarchar(std::to_string(total_size));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Unused object in buffer manager waiting for garbage collection");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
                }
                break;
            }
            case GlobalVariable::kActiveTxnCount: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
                }
                {
                    // option value
                    size_t active_txn_count = 0;
                    NewTxnManager *new_txn_mgr = query_context->storage()->new_txn_manager();
                    active_txn_count = new_txn_mgr->ActiveTxnCount();

                    Value value = Value::MakeVarchar(std::to_string(active_txn_count));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Active transaction count");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
                }
                break;
            }
            case GlobalVariable::kCurrentTs: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
                }
                {
                    // option value
                    TxnTimeStamp current_ts = 0;
                    NewTxnManager *new_txn_mgr = query_context->storage()->new_txn_manager();
                    current_ts = new_txn_mgr->CurrentTS();

                    Value value = Value::MakeVarchar(std::to_string(current_ts));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Current timestamp");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
                }
                break;
            }
            case GlobalVariable::kTotalCommitCount: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
                }
                {
                    // option value
                    u64 total_committed_txn_count = 0;
                    NewTxnManager *new_txn_mgr = query_context->storage()->new_txn_manager();
                    total_committed_txn_count = new_txn_mgr->total_committed_txn_count();

                    Value value = Value::MakeVarchar(std::to_string(total_committed_txn_count));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Global committed transaction count");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
                }
                break;
            }
            case GlobalVariable::kTotalRollbackCount: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
                }
                {
                    // option value
                    u64 total_rollbacked_txn_count = 0;
                    NewTxnManager *new_txn_mgr = query_context->storage()->new_txn_manager();
                    total_rollbacked_txn_count = new_txn_mgr->total_rollbacked_txn_count();

                    Value value = Value::MakeVarchar(std::to_string(total_rollbacked_txn_count));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Global rolled back transaction count");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
                }
                break;
            }
            case GlobalVariable::kActiveWALFilename: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
                }
                {
                    // option value
                    WalManager *wal_manager = query_context->storage()->wal_manager();
                    Value value = Value::MakeVarchar(wal_manager->GetWalFilename());
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Write ahead log filename");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
                }
                break;
            }
            case GlobalVariable::kProfileRecordCapacity: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
                }
                {
                    // option value
                    auto *catalog_ptr = query_context->storage()->new_catalog();
                    Value value = Value::MakeVarchar(std::to_string(catalog_ptr->ProfileHistorySize()));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Profile record history capacity");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
                }
                break;
            }
            case GlobalVariable::kBackgroundTaskCount: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
                }
                {
                    // option value
                    BGTaskProcessor *bg_processor = query_context->storage()->bg_processor();
                    size_t running_task_count = bg_processor->RunningTaskCount();
                    Value value = Value::MakeVarchar(std::to_string(running_task_count));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Background tasks count");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
                }
                break;
            }
            case GlobalVariable::kRunningBGTask: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
                }
                {
                    // option value
                    BGTaskProcessor *bg_processor = query_context->storage()->bg_processor();
                    Value value = Value::MakeVarchar(bg_processor->RunningTaskText());
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Current running background task");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
                }
                break;
            }
            case GlobalVariable::kRunningCompactTask: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
                }
                {
                    // option value
                    std::string task_count = "N/A";
                    if (query_context->storage()->GetStorageMode() == StorageMode::kWritable) {
                        CompactionProcessor *compaction_processor = query_context->storage()->compaction_processor();
                        task_count = std::to_string(compaction_processor->RunningTaskCount());
                    }
                    Value value = Value::MakeVarchar(task_count);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Current running background task");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
                }
                break;
            }
            case GlobalVariable::kSystemMemoryUsage: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
                }
                {
                    // option value
                    i64 memory_usage = SystemInfo::MemoryUsage();
                    Value value = Value::MakeVarchar(std::to_string(memory_usage));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Infinity system memory usage.");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
                }
                break;
            }
            case GlobalVariable::kOpenFileCount: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
                }
                {
                    // option value
                    i64 open_file_count = SystemInfo::OpenFileCount();
                    Value value = Value::MakeVarchar(std::to_string(open_file_count));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("File description opened count by Infinity.");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
                }
                break;
            }
            case GlobalVariable::kCPUUsage: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
                }
                {
                    // option value
                    f64 cpu_usage = SystemInfo::CPUUsage();
                    Value value = Value::MakeVarchar(fmt::format("{:.{}f}", cpu_usage * 100, 2));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Infinity system CPU usage.");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
                }
                break;
            }
            case GlobalVariable::kJeProf: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
                }
                {
#ifdef ENABLE_JEMALLOC_PROF
                    // option value
                    Value value = Value::MakeVarchar("off");
#else
                    Value value = Value::MakeVarchar("on");
#endif
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Use jemalloc to profile Infinity");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
                }
                break;
            }
            case GlobalVariable::kFollowerNum: {
                if (InfinityContext::instance().IsClusterRole()) {
                    std::shared_ptr<NodeInfo> this_node = InfinityContext::instance().cluster_manager()->ThisNode();
                    if (this_node->node_role() == NodeRole::kLeader) {
                        {
                            // option name
                            Value value = Value::MakeVarchar(var_name);
                            ValueExpression value_expr(value);
                            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
                        }
                        {
                            // option value
                            size_t follower_count = InfinityContext::instance().cluster_manager()->GetFollowerLimit();
                            Value value = Value::MakeVarchar(std::to_string(follower_count));
                            ValueExpression value_expr(value);
                            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
                        }
                        {
                            // option description
                            Value value = Value::MakeVarchar("Follower number for Infinity cluster");
                            ValueExpression value_expr(value);
                            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
                        }
                    }
                }
                break;
            }
            case GlobalVariable::kEnableProfile: {
                {
                    // option name
                    Value value = Value::MakeVarchar(var_name);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
                }
                {
                    // option value
                    bool enable_profile = query_context->storage()->new_catalog()->GetProfile();
                    std::string enable_profile_condition = enable_profile ? "true" : "false";
                    Value value = Value::MakeVarchar(enable_profile_condition);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
                }
                {
                    // option description
                    Value value = Value::MakeVarchar("Enable profile");
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
                }
                break;
            }
            default: {
                operator_state->status_ = Status::NoSysVar(var_name);
                RecoverableError(operator_state->status_);
                return;
            }
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
}
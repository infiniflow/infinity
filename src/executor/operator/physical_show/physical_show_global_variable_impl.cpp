module infinity_core:physical_show_global_variable.impl;

import :physical_show;
import :new_txn;
import :value;
import :value_expression;
import :data_block;
import :query_context;
import :operator_state;
import :data_table;
import :result_cache_manager;
import :session_manager;
import :utility;
import :new_txn_manager;
import :compaction_process;
import :background_process;
import :optimization_process;
import :system_info;
import :infinity_context;

import std;

using namespace infinity;

void PhysicalShow::ExecuteShowGlobalVariable(QueryContext *query_context, ShowOperatorState *operator_state) {
    std::shared_ptr<DataType> varchar_type = std::make_shared<DataType>(LogicalType::kVarchar);
    std::shared_ptr<DataType> integer_type = std::make_shared<DataType>(LogicalType::kBigInt);
    std::shared_ptr<DataType> double_type = std::make_shared<DataType>(LogicalType::kDouble);
    std::shared_ptr<DataType> bool_type = std::make_shared<DataType>(LogicalType::kBoolean);
    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();

    GlobalVariable global_var = VarUtil::GetGlobalVarByName(*object_name_);
    Config *config = query_context->global_config();

    switch (global_var) {
        case GlobalVariable::kResultCache: {
            std::vector<std::shared_ptr<ColumnDef>> output_column_defs = {
                std::make_shared<ColumnDef>(0, varchar_type, "value", std::set<ConstraintType>()),
            };

            std::shared_ptr<TableDef> table_def =
                TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("variables"), nullptr, output_column_defs);
            output_ = std::make_shared<DataTable>(table_def, TableType::kResult);

            std::vector<std::shared_ptr<DataType>> output_column_types{
                varchar_type,
            };

            output_block_ptr->Init(output_column_types);
            Value value = Value::MakeVarchar(config->ResultCache());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        case GlobalVariable::kCacheResultCapacity: {
            const std::string &result_cache_status = config->ResultCache();
            if (result_cache_status == "off") {
                operator_state->status_ = Status::NotSupport(fmt::format("Result cache is off"));
                RecoverableError(operator_state->status_);
            }
            ResultCacheManager *cache_mgr = query_context->storage()->GetResultCacheManagerPtr();

            std::vector<std::shared_ptr<ColumnDef>> output_column_defs = {
                std::make_shared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            std::shared_ptr<TableDef> table_def =
                TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("variables"), nullptr, output_column_defs);
            output_ = std::make_shared<DataTable>(table_def, TableType::kResult);

            std::vector<std::shared_ptr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);
            Value value = Value::MakeBigInt(cache_mgr->cache_num_capacity());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        case GlobalVariable::kCacheResultNum: {
            const std::string &result_cache_status = config->ResultCache();
            if (result_cache_status == "off") {
                operator_state->status_ = Status::NotSupport(fmt::format("Result cache is off"));
                RecoverableError(operator_state->status_);
            }
            ResultCacheManager *cache_mgr = query_context->storage()->GetResultCacheManagerPtr();

            std::vector<std::shared_ptr<ColumnDef>> output_column_defs = {
                std::make_shared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            std::shared_ptr<TableDef> table_def =
                TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("variables"), nullptr, output_column_defs);
            output_ = std::make_shared<DataTable>(table_def, TableType::kResult);

            std::vector<std::shared_ptr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);
            Value value = Value::MakeBigInt(cache_mgr->cache_num_used());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        case GlobalVariable::kMemoryCacheMiss: {
            std::vector<std::shared_ptr<ColumnDef>> output_column_defs = {
                std::make_shared<ColumnDef>(0, varchar_type, "value", std::set<ConstraintType>()),
            };

            std::shared_ptr<TableDef> table_def =
                TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("variables"), nullptr, output_column_defs);
            output_ = std::make_shared<DataTable>(table_def, TableType::kResult);

            std::vector<std::shared_ptr<DataType>> output_column_types{
                varchar_type,
            };

            BufferManager *buffer_manager = query_context->storage()->buffer_manager();
            u64 total_request_count = buffer_manager->TotalRequestCount();
            u64 cache_miss_count = buffer_manager->CacheMissCount();

            output_block_ptr->Init(output_column_types);
            Value value = Value::MakeVarchar(fmt::format("{}/{}", cache_miss_count, total_request_count));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        case GlobalVariable::kDiskCacheMiss: {
            std::vector<std::shared_ptr<ColumnDef>> output_column_defs = {
                std::make_shared<ColumnDef>(0, varchar_type, "value", std::set<ConstraintType>()),
            };

            std::shared_ptr<TableDef> table_def =
                TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("variables"), nullptr, output_column_defs);
            output_ = std::make_shared<DataTable>(table_def, TableType::kResult);

            std::vector<std::shared_ptr<DataType>> output_column_types{
                varchar_type,
            };

            output_block_ptr->Init(output_column_types);
            Value value = Value::MakeVarchar(fmt::format("{}/{}", VirtualStore::CacheMissCount(), VirtualStore::TotalRequestCount()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        case GlobalVariable::kQueryCount: {
            std::vector<std::shared_ptr<ColumnDef>> output_column_defs = {
                std::make_shared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            std::shared_ptr<TableDef> table_def =
                TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("variables"), nullptr, output_column_defs);
            output_ = std::make_shared<DataTable>(table_def, TableType::kResult);

            std::vector<std::shared_ptr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);
            Value value = Value::MakeBigInt(query_context->session_manager()->total_query_count());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        case GlobalVariable::kSessionCount: {
            std::vector<std::shared_ptr<ColumnDef>> output_column_defs = {
                std::make_shared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            std::shared_ptr<TableDef> table_def =
                TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("variables"), nullptr, output_column_defs);
            output_ = std::make_shared<DataTable>(table_def, TableType::kResult);

            std::vector<std::shared_ptr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            SessionManager *session_manager = query_context->session_manager();
            u64 session_count = session_manager->GetSessionCount();
            Value value = Value::MakeBigInt(session_count);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        case GlobalVariable::kBufferPoolUsage: {
            std::vector<std::shared_ptr<ColumnDef>> output_column_defs = {
                std::make_shared<ColumnDef>(0, varchar_type, "value", std::set<ConstraintType>()),
            };

            std::shared_ptr<TableDef> table_def =
                TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("variables"), nullptr, output_column_defs);
            output_ = std::make_shared<DataTable>(table_def, TableType::kResult);

            std::vector<std::shared_ptr<DataType>> output_column_types{
                varchar_type,
            };

            output_block_ptr->Init(output_column_types);

            BufferManager *buffer_manager = query_context->storage()->buffer_manager();
            u64 memory_limit = buffer_manager->memory_limit();
            u64 memory_usage = buffer_manager->memory_usage();
            Value value = Value::MakeVarchar(fmt::format("{}/{}", Utility::FormatByteSize(memory_usage), Utility::FormatByteSize(memory_limit)));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        case GlobalVariable::kSchedulePolicy: {
            std::vector<std::shared_ptr<ColumnDef>> output_column_defs = {
                std::make_shared<ColumnDef>(0, varchar_type, "value", std::set<ConstraintType>()),
            };

            std::shared_ptr<TableDef> table_def =
                TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("variables"), nullptr, output_column_defs);
            output_ = std::make_shared<DataTable>(table_def, TableType::kResult);

            std::vector<std::shared_ptr<DataType>> output_column_types{
                varchar_type,
            };

            output_block_ptr->Init(output_column_types);

            Value value = Value::MakeVarchar("round robin");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        case GlobalVariable::kNextTxnID: {
            std::vector<std::shared_ptr<ColumnDef>> output_column_defs = {
                std::make_shared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            std::shared_ptr<TableDef> table_def =
                TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("variables"), nullptr, output_column_defs);
            output_ = std::make_shared<DataTable>(table_def, TableType::kResult);

            std::vector<std::shared_ptr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            auto *new_txn_mgr = query_context->storage()->new_txn_manager();
            Value value = Value::MakeBigInt(new_txn_mgr->current_transaction_id());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        case GlobalVariable::kBufferedObjectCount: {
            std::vector<std::shared_ptr<ColumnDef>> output_column_defs = {
                std::make_shared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            std::shared_ptr<TableDef> table_def =
                TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("variables"), nullptr, output_column_defs);
            output_ = std::make_shared<DataTable>(table_def, TableType::kResult);

            std::vector<std::shared_ptr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            BufferManager *buffer_manager = query_context->storage()->buffer_manager();
            Value value = Value::MakeBigInt(buffer_manager->BufferedObjectCount());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        case GlobalVariable::kUnusedBufferObjectCount: {
            std::vector<std::shared_ptr<ColumnDef>> output_column_defs = {
                std::make_shared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            std::shared_ptr<TableDef> table_def =
                TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("variables"), nullptr, output_column_defs);
            output_ = std::make_shared<DataTable>(table_def, TableType::kResult);

            std::vector<std::shared_ptr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            BufferManager *buffer_manager = query_context->storage()->buffer_manager();
            std::vector<size_t> size_list = buffer_manager->WaitingGCObjectCount();
            size_t total_size = std::accumulate(size_list.begin(), size_list.end(), 0);
            Value value = Value::MakeBigInt(total_size);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        case GlobalVariable::kActiveTxnCount: {
            std::vector<std::shared_ptr<ColumnDef>> output_column_defs = {
                std::make_shared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            std::shared_ptr<TableDef> table_def =
                TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("variables"), nullptr, output_column_defs);
            output_ = std::make_shared<DataTable>(table_def, TableType::kResult);

            std::vector<std::shared_ptr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            size_t active_txn_count = 0;
            NewTxnManager *new_txn_mgr = query_context->storage()->new_txn_manager();
            active_txn_count = new_txn_mgr->ActiveTxnCount();

            Value value = Value::MakeBigInt(active_txn_count);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        case GlobalVariable::kCurrentTs: {
            std::vector<std::shared_ptr<ColumnDef>> output_column_defs = {
                std::make_shared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            std::shared_ptr<TableDef> table_def =
                TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("variables"), nullptr, output_column_defs);
            output_ = std::make_shared<DataTable>(table_def, TableType::kResult);

            std::vector<std::shared_ptr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            TxnTimeStamp current_ts = 0;
            NewTxnManager *new_txn_mgr = query_context->storage()->new_txn_manager();
            current_ts = new_txn_mgr->CurrentTS();

            Value value = Value::MakeBigInt(current_ts);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        case GlobalVariable::kTotalCommitCount: {
            std::vector<std::shared_ptr<ColumnDef>> output_column_defs = {
                std::make_shared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            std::shared_ptr<TableDef> table_def =
                TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("variables"), nullptr, output_column_defs);
            output_ = std::make_shared<DataTable>(table_def, TableType::kResult);

            std::vector<std::shared_ptr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            u64 total_committed_txn_count = 0;
            NewTxnManager *new_txn_mgr = query_context->storage()->new_txn_manager();
            total_committed_txn_count = new_txn_mgr->total_committed_txn_count();

            Value value = Value::MakeBigInt(total_committed_txn_count);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        case GlobalVariable::kTotalRollbackCount: {
            std::vector<std::shared_ptr<ColumnDef>> output_column_defs = {
                std::make_shared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            std::shared_ptr<TableDef> table_def =
                TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("variables"), nullptr, output_column_defs);
            output_ = std::make_shared<DataTable>(table_def, TableType::kResult);

            std::vector<std::shared_ptr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            u64 total_rollbacked_txn_count = 0;
            NewTxnManager *new_txn_mgr = query_context->storage()->new_txn_manager();
            total_rollbacked_txn_count = new_txn_mgr->total_rollbacked_txn_count();

            Value value = Value::MakeBigInt(total_rollbacked_txn_count);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        case GlobalVariable::kActiveWALFilename: {
            std::vector<std::shared_ptr<ColumnDef>> output_column_defs = {
                std::make_shared<ColumnDef>(0, varchar_type, "value", std::set<ConstraintType>()),
            };

            std::shared_ptr<TableDef> table_def =
                TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("variables"), nullptr, output_column_defs);
            output_ = std::make_shared<DataTable>(table_def, TableType::kResult);

            std::vector<std::shared_ptr<DataType>> output_column_types{
                varchar_type,
            };

            output_block_ptr->Init(output_column_types);

            WalManager *wal_manager = query_context->storage()->wal_manager();
            Value value = Value::MakeVarchar(wal_manager->GetWalFilename());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        case GlobalVariable::kProfileRecordCapacity: {
            std::vector<std::shared_ptr<ColumnDef>> output_column_defs = {
                std::make_shared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            std::shared_ptr<TableDef> table_def =
                TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("variables"), nullptr, output_column_defs);
            output_ = std::make_shared<DataTable>(table_def, TableType::kResult);

            std::vector<std::shared_ptr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            auto *catalog_ptr = query_context->storage()->new_catalog();
            Value value = Value::MakeBigInt(catalog_ptr->ProfileHistorySize());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        case GlobalVariable::kBackgroundTaskCount: {
            std::vector<std::shared_ptr<ColumnDef>> output_column_defs = {
                std::make_shared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            std::shared_ptr<TableDef> table_def =
                TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("variables"), nullptr, output_column_defs);
            output_ = std::make_shared<DataTable>(table_def, TableType::kResult);

            std::vector<std::shared_ptr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            BGTaskProcessor *bg_processor = query_context->storage()->bg_processor();
            size_t running_task_count = bg_processor->RunningTaskCount();
            Value value = Value::MakeBigInt(running_task_count);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        case GlobalVariable::kRunningBGTask: {
            std::vector<std::shared_ptr<ColumnDef>> output_column_defs = {
                std::make_shared<ColumnDef>(0, varchar_type, "value", std::set<ConstraintType>()),
            };

            std::shared_ptr<TableDef> table_def =
                TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("variables"), nullptr, output_column_defs);
            output_ = std::make_shared<DataTable>(table_def, TableType::kResult);

            std::vector<std::shared_ptr<DataType>> output_column_types{
                varchar_type,
            };

            output_block_ptr->Init(output_column_types);

            BGTaskProcessor *bg_processor = query_context->storage()->bg_processor();
            Value value = Value::MakeVarchar(bg_processor->RunningTaskText());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        case GlobalVariable::kRunningCompactTask: {
            std::vector<std::shared_ptr<ColumnDef>> output_column_defs = {
                std::make_shared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            std::shared_ptr<TableDef> table_def =
                TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("variables"), nullptr, output_column_defs);
            output_ = std::make_shared<DataTable>(table_def, TableType::kResult);

            std::vector<std::shared_ptr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            CompactionProcessor *compaction_processor = query_context->storage()->compaction_processor();
            Value value = Value::MakeBigInt(compaction_processor->RunningTaskCount());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        case GlobalVariable::kRunningOptimizeTask: {
            std::vector<std::shared_ptr<ColumnDef>> output_column_defs = {
                std::make_shared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            std::shared_ptr<TableDef> table_def =
                TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("variables"), nullptr, output_column_defs);
            output_ = std::make_shared<DataTable>(table_def, TableType::kResult);

            std::vector<std::shared_ptr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            OptimizationProcessor *optimize_processor = query_context->storage()->optimization_processor();
            Value value = Value::MakeBigInt(optimize_processor->RunningTaskCount());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        case GlobalVariable::kSystemMemoryUsage: {
            std::vector<std::shared_ptr<ColumnDef>> output_column_defs = {
                std::make_shared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            std::shared_ptr<TableDef> table_def =
                TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("variables"), nullptr, output_column_defs);
            output_ = std::make_shared<DataTable>(table_def, TableType::kResult);

            std::vector<std::shared_ptr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            i64 memory_usage = SystemInfo::MemoryUsage();
            Value value = Value::MakeBigInt(memory_usage);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        case GlobalVariable::kOpenFileCount: {
            std::vector<std::shared_ptr<ColumnDef>> output_column_defs = {
                std::make_shared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            std::shared_ptr<TableDef> table_def =
                TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("variables"), nullptr, output_column_defs);
            output_ = std::make_shared<DataTable>(table_def, TableType::kResult);

            std::vector<std::shared_ptr<DataType>> output_column_types{
                integer_type,
            };

            output_block_ptr->Init(output_column_types);

            i64 open_file_count = SystemInfo::OpenFileCount();
            Value value = Value::MakeBigInt(open_file_count);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        case GlobalVariable::kCPUUsage: {
            std::vector<std::shared_ptr<ColumnDef>> output_column_defs = {
                std::make_shared<ColumnDef>(0, double_type, "value", std::set<ConstraintType>()),
            };

            std::shared_ptr<TableDef> table_def =
                TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("variables"), nullptr, output_column_defs);
            output_ = std::make_shared<DataTable>(table_def, TableType::kResult);

            std::vector<std::shared_ptr<DataType>> output_column_types{
                double_type,
            };

            output_block_ptr->Init(output_column_types);

            f64 cpu_usage = SystemInfo::CPUUsage();
            Value value = Value::MakeDouble(cpu_usage * 100);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        case GlobalVariable::kJeProf: {

            std::vector<std::shared_ptr<DataType>> output_column_types{
                varchar_type,
            };

            output_block_ptr->Init(output_column_types);
#ifdef ENABLE_JEMALLOC_PROF
            // option value
            Value value = Value::MakeVarchar("off");
#else
            Value value = Value::MakeVarchar("on");
#endif
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        case GlobalVariable::kFollowerNum: {
            std::shared_ptr<NodeInfo> this_node = InfinityContext::instance().cluster_manager()->ThisNode();
            if (this_node->node_role() == NodeRole::kLeader) {
                std::vector<std::shared_ptr<ColumnDef>> output_column_defs = {
                    std::make_shared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
                };

                std::shared_ptr<TableDef> table_def = TableDef::Make(std::make_shared<std::string>("default_db"),
                                                                     std::make_shared<std::string>("variables"),
                                                                     nullptr,
                                                                     output_column_defs);
                output_ = std::make_shared<DataTable>(table_def, TableType::kResult);

                std::vector<std::shared_ptr<DataType>> output_column_types{
                    integer_type,
                };

                output_block_ptr->Init(output_column_types);

                i64 follower_number = InfinityContext::instance().cluster_manager()->GetFollowerLimit();
                Value value = Value::MakeBigInt(follower_number);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            } else {
                operator_state->status_ = Status::NotSupport(fmt::format("follower_number isn't supported in non-leader node of cluster deployment"));
                RecoverableError(operator_state->status_);
            }
            break;
        }
        case GlobalVariable::kEnableProfile: {
            std::vector<std::shared_ptr<ColumnDef>> output_column_defs = {
                std::make_shared<ColumnDef>(0, integer_type, "value", std::set<ConstraintType>()),
            };

            std::shared_ptr<TableDef> table_def =
                TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("variables"), nullptr, output_column_defs);
            output_ = std::make_shared<DataTable>(table_def, TableType::kResult);

            std::vector<std::shared_ptr<DataType>> output_column_types{
                bool_type,
            };

            output_block_ptr->Init(output_column_types);

            Value value = Value::MakeBool(InfinityContext::instance().storage()->new_catalog()->GetProfile());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
        default: {
            operator_state->status_ = Status::NoSysVar(*object_name_);
            RecoverableError(operator_state->status_);
            return;
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
}
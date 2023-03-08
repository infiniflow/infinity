//
// Created by jinhai on 23-1-17.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "main/session.h"
#include "planner/logical_planner.h"
#include "planner/optimizer.h"
#include "executor/physical_planner.h"
#include "scheduler/operator_pipeline.h"
#include "main/infinity.h"

#include "main/profiler/show_logical_plan.h"
#include "parser/sql_parser.h"

class LogicalPlannerTest : public BaseTest {
    void
    SetUp() override {
        infinity::GlobalResourceUsage::Init();
        infinity::Infinity::instance().Init();
    }

    void
    TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};

infinity::String
RunSQL(const infinity::String& sql_text, bool print) {
    using namespace infinity;
    if(print) {
        LOG_TRACE("{}", sql_text);
    }

    SharedPtr <SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr <ParserResult> parsed_result = MakeShared<ParserResult>();
    parser->Parse(sql_text, parsed_result);

    if(parsed_result->IsError()) {
        ParserError(parsed_result->error_message_)
    }

    SharedPtr<Session> session_ptr = MakeShared<Session>();
    SharedPtr<QueryContext> query_context_ptr = MakeShared<QueryContext>(session_ptr, session_ptr->transaction());
    query_context_ptr->set_current_schema(session_ptr->current_schema());

    LogicalPlanner logical_planner(query_context_ptr);
    Optimizer optimizer(query_context_ptr);
    PhysicalPlanner physical_planner(query_context_ptr);

    PlannerAssert(parsed_result->statements_ptr_->size() == 1, "Not support more statements");

    BaseStatement* statement = (*parsed_result->statements_ptr_)[0];
    logical_planner.Build(statement);
    parsed_result->Reset();

    SharedPtr<LogicalNode> unoptimized_plan = logical_planner.LogicalPlan();

    if(print) {
        ShowLogicalPlan show_logical_plan(unoptimized_plan);
        LOG_TRACE("unoptimized plan: \n{}", show_logical_plan.ToString());
    }

    if(unoptimized_plan == nullptr) {
        PlannerError("logical plan is nullptr");
    }
    // Apply optimized rule to the logical plan
    SharedPtr<LogicalNode> optimized_plan = optimizer.optimize(unoptimized_plan);

    // Build physical plan
    SharedPtr<PhysicalOperator> physical_plan = physical_planner.BuildPhysicalOperator(optimized_plan);

    // Create execution pipeline
    SharedPtr<Pipeline> pipeline = OperatorPipeline::Create(physical_plan);

    // Schedule the query pipeline
    Infinity::instance().scheduler()->Schedule(query_context_ptr, pipeline);

    // Initialize query result
    QueryResult query_result;
    query_result.result_ = pipeline->GetResult();
    query_result.root_operator_type_ = unoptimized_plan->operator_type();

    if(query_result.result_ == nullptr) {
        return "No result";
    } else{
        return query_result.ToString();
    }
}

void
BuildLogicalPlan(const infinity::String& sql_text) {
    using namespace infinity;

    SharedPtr <SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr <ParserResult> parsed_result = MakeShared<ParserResult>();
    parser->Parse(sql_text, parsed_result);

    if(parsed_result->IsError()) {
        ParserError(parsed_result->error_message_)
    }

    SharedPtr<Session> session_ptr = MakeShared<Session>();
    SharedPtr<QueryContext> query_context_ptr = MakeShared<QueryContext>(session_ptr, session_ptr->transaction());
    query_context_ptr->set_current_schema(session_ptr->current_schema());

    LogicalPlanner logical_planner(query_context_ptr);
    Optimizer optimizer(query_context_ptr);
    PhysicalPlanner physical_planner(query_context_ptr);

    PlannerAssert(parsed_result->statements_ptr_->size() == 1, "Not support more statements");

    BaseStatement* statement = (*parsed_result->statements_ptr_)[0];
    logical_planner.Build(statement);
    parsed_result->Reset();
}

TEST_F(LogicalPlannerTest, test1) {
    using namespace infinity;

    RunSQL("create table t1(a bigint, b bigint);", false);
    RunSQL("insert into t1 values(1, 2);", false);
    RunSQL("insert into t1 values(2, 4);", false);
    RunSQL("insert into t1 values(3, 6);", false);
    RunSQL("create table t2(a bigint, b bigint);", false);
    RunSQL("insert into t2 values(10, 20);", false);
    RunSQL("insert into t2 values(20, 40);", false);
    RunSQL("create table t3(a bigint, b bigint);", false);
    RunSQL("insert into t3 values(5, 25);", false);
    RunSQL("insert into t3 values(15, 35);", false);
    RunSQL("show tables;", false);
#if 1
    {
        const String sql_text = "select * from t1;";
        RunSQL(sql_text, true);
    }
    {
        const String sql_text = "select a from t1;";
        RunSQL(sql_text, true);
    }
    {
        const String sql_text = "select t1.a from t1;";
        RunSQL(sql_text, true);
    }
    {
        const String sql_text = "select b from t1;";
        RunSQL(sql_text, true);
    }
    {
        const String sql_text = "select t1.b from t1;";
        RunSQL(sql_text, true);
    }
    {
        const String sql_text = "select abs(b) from t1;";
        RunSQL(sql_text, true);
    }
    {
        const String sql_text = "select a+1 from t1;";
        RunSQL(sql_text, true);
    }
    {
        const String sql_text = "select a+1 from t1 order by b;";
        RunSQL(sql_text, true);
    }
    {
        const String sql_text = "select a, b from t1 group by a, b;";
        RunSQL(sql_text, true);
    }
    {
        const String sql_text = "select sum(b), b from t1 group by a;";
        EXPECT_THROW(RunSQL(sql_text, true), PlannerException);
    }
    {
        const String sql_text = "select sum(b), b from t1;";
        EXPECT_THROW(RunSQL(sql_text, true), PlannerException);
    }

    {
        const String sql_text = "select sum(b), count(a) from t1;";
        RunSQL(sql_text, true);
    }
#endif
    {
        const String sql_text = "select * from t1, t2;";
        RunSQL(sql_text, true);
    }
#if 1
    {
        const String sql_text = "select * from t1, t2, t3 where t1.a < 3;";
        RunSQL(sql_text, true);
    }
#endif
//    {
//        // Only check the ast.
//        const String sql_text = "(SELECT * FROM students INTERSECT SELECT * FROM students_2) UNION SELECT * FROM students_3 ORDER BY grade ASC;";
//        RunSQL(sql_text, true);
//    }

//    RunSQL("WITH cte AS (SELECT 42 AS i), cte2 AS (SELECT i*100 AS x FROM cte) SELECT * FROM cte2;");

//    BuiltinFunctions builtin_functions(catalog_ptr);
//    builtin_functions.Init();
}
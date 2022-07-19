
#include "sqlhelper.h"
#include <iostream>
#include <map>
#include <string>

namespace hsql {

void printOperatorExpression(Expr* expr, uintmax_t numIndent);
void printAlias(Alias* alias, uintmax_t numIndent);

std::ostream& operator<<(std::ostream& os, const OperatorType& op);
std::ostream& operator<<(std::ostream& os, const DatetimeField& op);

std::string indent(uintmax_t numIndent) { return std::string(numIndent, '\t'); }
void inprint(int64_t val, uintmax_t numIndent) { std::cout << indent(numIndent).c_str() << val << "  " << std::endl; }
void inprint(double val, uintmax_t numIndent) { std::cout << indent(numIndent).c_str() << val << std::endl; }
void inprint(const char* val, uintmax_t numIndent) { std::cout << indent(numIndent).c_str() << val << std::endl; }
void inprint(const char* val, const char* val2, uintmax_t numIndent) {
  std::cout << indent(numIndent).c_str() << val << "->" << val2 << std::endl;
}
void inprintC(char val, uintmax_t numIndent) { std::cout << indent(numIndent).c_str() << val << std::endl; }
void inprint(const OperatorType& op, uintmax_t numIndent) { std::cout << indent(numIndent) << op << std::endl; }
void inprint(const ColumnType& colType, uintmax_t numIndent) { std::cout << indent(numIndent) << colType << std::endl; }
void inprint(const DatetimeField& colType, uintmax_t numIndent) {
  std::cout << indent(numIndent) << colType << std::endl;
}

void printTableRefInfo(TableRef* table, uintmax_t numIndent) {
  switch (table->type) {
    case kTableName:
      inprint(table->name, numIndent);
      if (table->schema) {
        inprint("Schema", numIndent + 1);
        inprint(table->schema, numIndent + 2);
      }
      break;
    case kTableSelect:
      printSelectStatementInfo(table->select, numIndent);
      break;
    case kTableJoin:
      inprint("Join Table", numIndent);
      inprint("Left", numIndent + 1);
      printTableRefInfo(table->join->left, numIndent + 2);
      inprint("Right", numIndent + 1);
      printTableRefInfo(table->join->right, numIndent + 2);
      inprint("Join Condition", numIndent + 1);
      printExpression(table->join->condition, numIndent + 2);
      break;
    case kTableCrossProduct:
      for (TableRef* tbl : *table->list) printTableRefInfo(tbl, numIndent);
      break;
  }

  if (table->alias) {
    printAlias(table->alias, numIndent);
  }
}

void printAlias(Alias* alias, uintmax_t numIndent) {
  inprint("Alias", numIndent + 1);
  inprint(alias->name, numIndent + 2);

  if (alias->columns) {
    for (char* column : *(alias->columns)) {
      inprint(column, numIndent + 3);
    }
  }
}

void printOperatorExpression(Expr* expr, uintmax_t numIndent) {
  if (expr == nullptr) {
    inprint("null", numIndent);
    return;
  }

  inprint(expr->opType, numIndent);

  printExpression(expr->expr, numIndent + 1);
  if (expr->expr2 != nullptr) {
    printExpression(expr->expr2, numIndent + 1);
  } else if (expr->exprList != nullptr) {
    for (Expr* e : *expr->exprList) printExpression(e, numIndent + 1);
  }
}

void printExpression(Expr* expr, uintmax_t numIndent) {
  if (!expr) return;
  switch (expr->type) {
    case kExprStar:
      inprint("*", numIndent);
      break;
    case kExprColumnRef:
      inprint(expr->name, numIndent);
      if (expr->table) {
        inprint("Table:", numIndent + 1);
        inprint(expr->table, numIndent + 2);
      }
      break;
    // case kExprTableColumnRef: inprint(expr->table, expr->name, numIndent); break;
    case kExprLiteralFloat:
      inprint(expr->fval, numIndent);
      break;
    case kExprLiteralInt:
      inprint(expr->ival, numIndent);
      break;
    case kExprLiteralString:
      inprint(expr->name, numIndent);
      break;
    case kExprLiteralDate:
      inprint(expr->name, numIndent);
      break;
    case kExprLiteralNull:
      inprint("NULL", numIndent);
      break;
    case kExprLiteralInterval:
      inprint("INTERVAL", numIndent);
      inprint(expr->ival, numIndent + 1);
      inprint(expr->datetimeField, numIndent + 1);
      break;
    case kExprFunctionRef:
      inprint(expr->name, numIndent);
      for (Expr* e : *expr->exprList) printExpression(e, numIndent + 1);
      break;
    case kExprExtract:
      inprint("EXTRACT", numIndent);
      inprint(expr->datetimeField, numIndent + 1);
      printExpression(expr->expr, numIndent + 1);
      break;
    case kExprCast:
      inprint("CAST", numIndent);
      inprint(expr->columnType, numIndent + 1);
      printExpression(expr->expr, numIndent + 1);
      break;
    case kExprOperator:
      printOperatorExpression(expr, numIndent);
      break;
    case kExprSelect:
      printSelectStatementInfo(expr->select, numIndent);
      break;
    case kExprParameter:
      inprint(expr->ival, numIndent);
      break;
    case kExprArray:
      for (Expr* e : *expr->exprList) printExpression(e, numIndent + 1);
      break;
    case kExprArrayIndex:
      printExpression(expr->expr, numIndent + 1);
      inprint(expr->ival, numIndent);
      break;
    default:
      std::cerr << "Unrecognized expression type " << expr->type << std::endl;
      return;
  }
  if (expr->alias != nullptr) {
    inprint("Alias", numIndent + 1);
    inprint(expr->alias, numIndent + 2);
  }
}

void printOrderBy(const std::vector<OrderDescription*>* expr, uintmax_t numIndent) {
  if (!expr) return;
  for (const auto& order_description : *expr) {
    printExpression(order_description->expr, numIndent);
    if (order_description->type == kOrderAsc) {
      inprint("ascending", numIndent);
    } else {
      inprint("descending", numIndent);
    }
  }
}

void printSelectStatementInfo(const SelectStatement* stmt, uintmax_t numIndent) {
  inprint("SelectStatement", numIndent);
  inprint("Fields:", numIndent + 1);
  for (Expr* expr : *stmt->selectList) printExpression(expr, numIndent + 2);

  if (stmt->fromTable != nullptr) {
    inprint("Sources:", numIndent + 1);
    printTableRefInfo(stmt->fromTable, numIndent + 2);
  }

  if (stmt->whereClause != nullptr) {
    inprint("Search Conditions:", numIndent + 1);
    printExpression(stmt->whereClause, numIndent + 2);
  }

  if (stmt->groupBy != nullptr) {
    inprint("GroupBy:", numIndent + 1);
    for (Expr* expr : *stmt->groupBy->columns) printExpression(expr, numIndent + 2);
    if (stmt->groupBy->having != nullptr) {
      inprint("Having:", numIndent + 1);
      printExpression(stmt->groupBy->having, numIndent + 2);
    }
  }
  if (stmt->lockings != nullptr) {
    inprint("Lock Info:", numIndent + 1);
    for (LockingClause* lockingClause : *stmt->lockings) {
      inprint("Type", numIndent + 2);
      if (lockingClause->rowLockMode == RowLockMode::ForUpdate) {
        inprint("FOR UPDATE", numIndent + 3);
      } else if (lockingClause->rowLockMode == RowLockMode::ForNoKeyUpdate) {
        inprint("FOR NO KEY UPDATE", numIndent + 3);
      } else if (lockingClause->rowLockMode == RowLockMode::ForShare) {
        inprint("FOR SHARE", numIndent + 3);
      } else if (lockingClause->rowLockMode == RowLockMode::ForKeyShare) {
        inprint("FOR KEY SHARE", numIndent + 3);
      }
      if (lockingClause->tables != nullptr) {
        inprint("Target tables:", numIndent + 2);
        for (char* dtable : *lockingClause->tables) {
          inprint(dtable, numIndent + 3);
        }
      }
      if (lockingClause->rowLockWaitPolicy != RowLockWaitPolicy::None) {
        inprint("Waiting policy: ", numIndent + 2);
        if (lockingClause->rowLockWaitPolicy == RowLockWaitPolicy::NoWait)
          inprint("NOWAIT", numIndent + 3);
        else
          inprint("SKIP LOCKED", numIndent + 3);
      }
    }
  }

  if (stmt->setOperations != nullptr) {
    for (SetOperation* setOperation : *stmt->setOperations) {
      switch (setOperation->setType) {
        case SetType::kSetIntersect:
          inprint("Intersect:", numIndent + 1);
          break;
        case SetType::kSetUnion:
          inprint("Union:", numIndent + 1);
          break;
        case SetType::kSetExcept:
          inprint("Except:", numIndent + 1);
          break;
      }

      printSelectStatementInfo(setOperation->nestedSelectStatement, numIndent + 2);

      if (setOperation->resultOrder != nullptr) {
        inprint("SetResultOrderBy:", numIndent + 1);
        printOrderBy(setOperation->resultOrder, numIndent + 2);
      }

      if (setOperation->resultLimit != nullptr) {
        if (setOperation->resultLimit->limit != nullptr) {
          inprint("SetResultLimit:", numIndent + 1);
          printExpression(setOperation->resultLimit->limit, numIndent + 2);
        }

        if (setOperation->resultLimit->offset != nullptr) {
          inprint("SetResultOffset:", numIndent + 1);
          printExpression(setOperation->resultLimit->offset, numIndent + 2);
        }
      }
    }
  }

  if (stmt->order != nullptr) {
    inprint("OrderBy:", numIndent + 1);
    printOrderBy(stmt->order, numIndent + 2);
  }

  if (stmt->limit != nullptr && stmt->limit->limit != nullptr) {
    inprint("Limit:", numIndent + 1);
    printExpression(stmt->limit->limit, numIndent + 2);
  }

  if (stmt->limit != nullptr && stmt->limit->offset != nullptr) {
    inprint("Offset:", numIndent + 1);
    printExpression(stmt->limit->offset, numIndent + 2);
  }
}

void printImportStatementInfo(const ImportStatement* stmt, uintmax_t numIndent) {
  inprint("ImportStatement", numIndent);
  inprint(stmt->filePath, numIndent + 1);
  switch (stmt->type) {
    case ImportType::kImportCSV:
      inprint("CSV", numIndent + 1);
      break;
    case ImportType::kImportTbl:
      inprint("TBL", numIndent + 1);
      break;
    case ImportType::kImportBinary:
      inprint("BINARY", numIndent + 1);
      break;
    case ImportType::kImportAuto:
      inprint("AUTO", numIndent + 1);
      break;
  }
  inprint(stmt->tableName, numIndent + 1);
}

void printExportStatementInfo(const ExportStatement* stmt, uintmax_t numIndent) {
  inprint("ExportStatement", numIndent);
  inprint(stmt->filePath, numIndent + 1);
  switch (stmt->type) {
    case ImportType::kImportCSV:
      inprint("CSV", numIndent + 1);
      break;
    case ImportType::kImportTbl:
      inprint("TBL", numIndent + 1);
      break;
    case ImportType::kImportBinary:
      inprint("BINARY", numIndent + 1);
      break;
    case ImportType::kImportAuto:
      inprint("AUTO", numIndent + 1);
      break;
  }
  inprint(stmt->tableName, numIndent + 1);
}

void printCreateStatementInfo(const CreateStatement* stmt, uintmax_t numIndent) {
  inprint("CreateStatement", numIndent);
  inprint(stmt->tableName, numIndent + 1);
  if (stmt->filePath) inprint(stmt->filePath, numIndent + 1);
}

void printInsertStatementInfo(const InsertStatement* stmt, uintmax_t numIndent) {
  inprint("InsertStatement", numIndent);
  inprint(stmt->tableName, numIndent + 1);
  if (stmt->columns != nullptr) {
    inprint("Columns", numIndent + 1);
    for (char* col_name : *stmt->columns) {
      inprint(col_name, numIndent + 2);
    }
  }
  switch (stmt->type) {
    case kInsertValues:
      inprint("Values", numIndent + 1);
      for (Expr* expr : *stmt->values) {
        printExpression(expr, numIndent + 2);
      }
      break;
    case kInsertSelect:
      printSelectStatementInfo(stmt->select, numIndent + 1);
      break;
  }
}

void printTransactionStatementInfo(const TransactionStatement* stmt, uintmax_t numIndent) {
  inprint("TransactionStatement", numIndent);
  switch (stmt->command) {
    case kBeginTransaction:
      inprint("BEGIN", numIndent + 1);
      break;
    case kCommitTransaction:
      inprint("COMMIT", numIndent + 1);
      break;
    case kRollbackTransaction:
      inprint("ROLLBACK", numIndent + 1);
      break;
  }
}

void printStatementInfo(const SQLStatement* stmt) {
  switch (stmt->type()) {
    case kStmtSelect:
      printSelectStatementInfo((const SelectStatement*)stmt, 0);
      break;
    case kStmtInsert:
      printInsertStatementInfo((const InsertStatement*)stmt, 0);
      break;
    case kStmtCreate:
      printCreateStatementInfo((const CreateStatement*)stmt, 0);
      break;
    case kStmtImport:
      printImportStatementInfo((const ImportStatement*)stmt, 0);
      break;
    case kStmtExport:
      printExportStatementInfo((const ExportStatement*)stmt, 0);
      break;
    case kStmtTransaction:
      printTransactionStatementInfo((const TransactionStatement*)stmt, 0);
      break;
    default:
      break;
  }
}

std::ostream& operator<<(std::ostream& os, const OperatorType& op) {
  static const std::map<const OperatorType, const std::string> operatorToToken = {
      {kOpNone, "None"},     {kOpBetween, "BETWEEN"},
      {kOpCase, "CASE"},     {kOpCaseListElement, "CASE LIST ELEMENT"},
      {kOpPlus, "+"},        {kOpMinus, "-"},
      {kOpAsterisk, "*"},    {kOpSlash, "/"},
      {kOpPercentage, "%"},  {kOpCaret, "^"},
      {kOpEquals, "="},      {kOpNotEquals, "!="},
      {kOpLess, "<"},        {kOpLessEq, "<="},
      {kOpGreater, ">"},     {kOpGreaterEq, ">="},
      {kOpLike, "LIKE"},     {kOpNotLike, "NOT LIKE"},
      {kOpILike, "ILIKE"},   {kOpAnd, "AND"},
      {kOpOr, "OR"},         {kOpIn, "IN"},
      {kOpConcat, "CONCAT"}, {kOpNot, "NOT"},
      {kOpUnaryMinus, "-"},  {kOpIsNull, "IS NULL"},
      {kOpExists, "EXISTS"}};

  const auto found = operatorToToken.find(op);
  if (found == operatorToToken.cend()) {
    return os << static_cast<uint64_t>(op);
  } else {
    return os << (*found).second;
  }
}

std::ostream& operator<<(std::ostream& os, const DatetimeField& datetime) {
  static const std::map<const DatetimeField, const std::string> operatorToToken = {
      {kDatetimeNone, "None"}, {kDatetimeSecond, "SECOND"}, {kDatetimeMinute, "MINUTE"}, {kDatetimeHour, "HOUR"},
      {kDatetimeDay, "DAY"},   {kDatetimeMonth, "MONTH"},   {kDatetimeYear, "YEAR"}};

  const auto found = operatorToToken.find(datetime);
  if (found == operatorToToken.cend()) {
    return os << static_cast<uint64_t>(datetime);
  } else {
    return os << (*found).second;
  }
}

}  // namespace hsql

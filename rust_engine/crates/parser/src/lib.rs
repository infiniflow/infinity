// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

//! `infinity-parser`: a hand-written SQL lexer + recursive-descent parser
//! (plan §3 D5). This is the M2 seed covering a small but real SQL subset:
//! `CREATE/DROP TABLE`, `INSERT ... VALUES`, `SELECT`.

#![forbid(unsafe_code)]

pub mod ast;
pub mod lexer;
pub mod parser;
pub mod render;

#[cfg(test)]
mod corpora_ut;

pub use ast::{
    AggFunc, BinOp, BoolOp, CmpOp, ColumnDef, Cte, ExplainKind, ExprAst, FlushKind, JoinClause,
    JoinKind, Literal, MatchClause, OrderKey, Predicate, SearchClause, SelectAgg, SelectItem,
    SparseStore, Statement,
};
pub use parser::{
    normalize_datetime, parse, parse_expr, parse_one, parse_predicate, parse_select_item,
};

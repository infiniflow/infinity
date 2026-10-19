//! `infinity-parser`: a hand-written SQL lexer + recursive-descent parser
//! (plan §3 D5). This is the M2 seed covering a small but real SQL subset:
//! `CREATE/DROP TABLE`, `INSERT ... VALUES`, `SELECT`.

#![forbid(unsafe_code)]

pub mod ast;
pub mod lexer;
pub mod parser;

#[cfg(test)]
mod corpora_ut;

pub use ast::{
    AggFunc, BinOp, BoolOp, CmpOp, ColumnDef, Cte, ExplainKind, ExprAst, JoinClause, JoinKind,
    Literal, MatchClause, OrderKey, Predicate, SearchClause, SelectAgg, SelectItem, Statement,
};
pub use parser::{normalize_datetime, parse};

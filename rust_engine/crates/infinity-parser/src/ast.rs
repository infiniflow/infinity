//! SQL AST (subset).

use infinity_common::LogicalType;

/// A literal value.
#[derive(Debug, Clone, PartialEq)]
pub enum Literal {
    Int(i64),
    Float(f64),
    Str(String),
    Null,
    /// A vector / array literal, e.g. `[0.1, 0.2]` or a sparse `[0:1.0,20:2.0]`.
    /// Elements are kept as their original text so rendering can round-trip.
    Array(Vec<String>),
}

impl Literal {
    /// `.slt`-style text rendering (REAL uses fixed 6 decimals, per PoC H).
    pub fn to_slt_string(&self) -> String {
        match self {
            Literal::Int(i) => i.to_string(),
            Literal::Float(f) => format!("{f:.6}"),
            Literal::Str(s) => s.clone(),
            Literal::Null => "null".to_string(),
            Literal::Array(items) => format!("[{}]", items.join(",")),
        }
    }
}

/// A binary arithmetic operator.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum BinOp {
    Add,
    Sub,
    Mul,
    Div,
    Mod,
}

/// A scalar expression as parsed from SQL text (names not yet resolved).
#[derive(Debug, Clone, PartialEq)]
pub enum ExprAst {
    Column(String),
    Literal(Literal),
    Func {
        name: String,
        args: Vec<ExprAst>,
    },
    Binary(Box<ExprAst>, BinOp, Box<ExprAst>),
    /// A comparison usable as a projected boolean expression.
    Cmp(Box<ExprAst>, CmpOp, Box<ExprAst>),
    /// Boolean connectives in a projected expression.
    Not(Box<ExprAst>),
    And(Box<ExprAst>, Box<ExprAst>),
    Or(Box<ExprAst>, Box<ExprAst>),
}

/// A boolean connective.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum BoolOp {
    And,
    Or,
}

/// A `WHERE`-style boolean predicate tree.
#[derive(Debug, Clone, PartialEq)]
pub enum Predicate {
    /// `left <op> right`.
    Cmp(ExprAst, CmpOp, ExprAst),
    /// A bare boolean-valued expression (e.g. a predicate function call).
    IsTrue(ExprAst),
    /// `expr IS [NOT] NULL`.
    IsNull(ExprAst, bool),
    /// `expr [NOT] LIKE pattern [ESCAPE c]`.
    Like(ExprAst, ExprAst, Option<char>, bool),
    /// `expr [NOT] IN (v1, v2, …)`.
    In(ExprAst, Vec<ExprAst>, bool),
    /// `AND` / `OR` of two predicates.
    Bool(Box<Predicate>, BoolOp, Box<Predicate>),
    /// `NOT pred`.
    Not(Box<Predicate>),
}

impl Predicate {
    /// Convenience constructor for the common `column <op> literal` shape.
    pub fn simple(column: impl Into<String>, op: CmpOp, value: Literal) -> Predicate {
        Predicate::Cmp(ExprAst::Column(column.into()), op, ExprAst::Literal(value))
    }

    /// If this predicate is the simple `column <op> literal` shape, return its parts.
    pub fn as_column_literal(&self) -> Option<(&str, CmpOp, &Literal)> {
        match self {
            Predicate::Cmp(ExprAst::Column(c), op, ExprAst::Literal(l)) => Some((c, *op, l)),
            _ => None,
        }
    }
}

/// Comparison operator used in `WHERE`.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum CmpOp {
    Eq,
    Ne,
    Lt,
    Le,
    Gt,
    Ge,
}

impl CmpOp {
    pub fn as_str(&self) -> &'static str {
        match self {
            CmpOp::Eq => "=",
            CmpOp::Ne => "!=",
            CmpOp::Lt => "<",
            CmpOp::Le => "<=",
            CmpOp::Gt => ">",
            CmpOp::Ge => ">=",
        }
    }
}

/// An aggregate function.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum AggFunc {
    Count,
    Sum,
    Avg,
    Min,
    Max,
    First,
    Last,
}

impl AggFunc {
    pub fn as_str(&self) -> &'static str {
        match self {
            AggFunc::Count => "count",
            AggFunc::Sum => "sum",
            AggFunc::Avg => "avg",
            AggFunc::Min => "min",
            AggFunc::Max => "max",
            AggFunc::First => "first",
            AggFunc::Last => "last",
        }
    }

    /// Parses an aggregate name (case-insensitive).
    pub fn from_name(name: &str) -> Option<AggFunc> {
        Some(match name.to_ascii_uppercase().as_str() {
            "COUNT" => AggFunc::Count,
            "SUM" => AggFunc::Sum,
            "AVG" => AggFunc::Avg,
            "MIN" => AggFunc::Min,
            "MAX" => AggFunc::Max,
            "FIRST" => AggFunc::First,
            "LAST" => AggFunc::Last,
            _ => return None,
        })
    }
}

/// An aggregate call, e.g. `COUNT(*)` (no args), `SUM(col)` or `COUNT(DISTINCT a, b)`.
#[derive(Debug, Clone, PartialEq)]
pub struct SelectAgg {
    pub func: AggFunc,
    /// Argument texts (a column name, or the canonical text of an expression).
    pub args: Vec<String>,
    /// Aligned with `args`: `Some` when the argument is a computed expression
    /// (e.g. `CAST(c3 AS INTEGER)`) rather than a plain column.
    pub arg_exprs: Vec<Option<ExprAst>>,
    pub distinct: bool,
}

/// `ORDER BY col [ASC|DESC]`.
#[derive(Debug, Clone, PartialEq)]
pub struct OrderKey {
    /// The leading identifier (`ORDER BY c1 + c2` → `c1`).
    pub column: String,
    pub asc: bool,
    /// The full sort expression when it is more than a plain column.
    pub expr: Option<ExprAst>,
}

/// The join flavour named before `JOIN`.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
pub enum JoinKind {
    #[default]
    Inner,
    Left,
    Right,
    Cross,
}

impl JoinKind {
    /// The upper-case label used by `EXPLAIN` (`INNER JOIN`, `LEFT JOIN`, …).
    pub fn label(self) -> &'static str {
        match self {
            JoinKind::Inner => "INNER JOIN",
            JoinKind::Left => "LEFT JOIN",
            JoinKind::Right => "RIGHT JOIN",
            JoinKind::Cross => "CROSS JOIN",
        }
    }
}

/// `JOIN <table> ON <left> = <right>`. Keys are unqualified and resolved within
/// their own table (left key against the left table, right key against the right).
#[derive(Debug, Clone, PartialEq)]
pub struct JoinClause {
    pub table: String,
    pub left: String,
    pub right: String,
    pub kind: JoinKind,
}

/// A column definition.
#[derive(Debug, Clone, PartialEq)]
pub struct ColumnDef {
    pub name: String,
    pub ty: LogicalType,
    /// Raw declared type text, e.g. `TENSOR(FLOAT,4)` (uppercased, no spaces).
    pub type_text: String,
    pub default: Option<Literal>,
    pub not_null: bool,
    /// `COMMENT '…'`
    pub comment: Option<String>,
}

/// A `SELECT` list item.
#[derive(Debug, Clone, PartialEq)]
pub enum SelectItem {
    Star,
    Literal(Literal),
    Column(String),
    Aggregate(SelectAgg),
    /// A scalar function call, e.g. `ROUND(x, 4)` or `ROW_ID()`.
    Func {
        name: String,
        args: Vec<ExprAst>,
    },
    /// An arbitrary scalar expression (e.g. `MAX(c1)+SUM(c2)`).
    Expr(ExprAst),
}

/// One `MATCH <KIND> (<column>, <args...>)` term of a `SEARCH MATCH` clause.
#[derive(Debug, Clone, PartialEq)]
pub struct MatchClause {
    pub kind: String,
    pub column: String,
    pub args: Vec<ExprAst>,
    /// An in-parenthesis `WHERE <predicate>` filter (applied to candidates).
    pub filter: Option<Predicate>,
}

/// A `SEARCH MATCH …` clause with optional `WITH (…)` / `USING INDEX (…)` and
/// fusion tail (`{FUSION('…', '…')}`).
#[derive(Debug, Clone, PartialEq, Default)]
pub struct SearchClause {
    pub matches: Vec<MatchClause>,
    pub fusion: Option<String>,
    pub fusion_params: Option<String>,
    /// Every `FUSION(name, params)` tail in chain order (`fusion`/`fusion_params`
    /// keep the last one, which decides the final score).
    pub fusions: Vec<(String, String)>,
    pub using_index: Option<String>,
    pub with_opts: Option<String>,
}

/// A parsed statement.
///
/// `Select` is intentionally the largest variant; it is a short-lived parse
/// tree, so boxing its fields would only complicate matching.
/// The plan flavour named by `EXPLAIN <kind> …`.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
pub enum ExplainKind {
    #[default]
    Physical,
    Logical,
    /// `EXPLAIN FRAGMENT`: the multi-fragment physical plan.
    Fragment,
}

#[allow(clippy::large_enum_variant)]
#[derive(Debug, Clone, PartialEq)]
pub enum Statement {
    CreateTable {
        name: String,
        columns: Vec<ColumnDef>,
    },
    CreateDatabase {
        name: String,
        if_not_exists: bool,
    },
    DropDatabase {
        name: String,
        if_exists: bool,
    },
    /// `USE <database>`
    Use {
        name: String,
    },
    CreateIndex {
        name: String,
        table: String,
        column: String,
        /// `USING <kind>` (e.g. `Hnsw`, `Bmp`), empty when unspecified.
        index_type: String,
        /// `WITH (k = v, …)` options, keys upper-cased.
        opts: Vec<(String, String)>,
        /// `CREATE INDEX IF NOT EXISTS …`
        if_not_exists: bool,
    },
    DropTable {
        name: String,
    },
    DropIndex {
        name: String,
        if_exists: bool,
    },
    /// A tolerated session/utility statement (`SET …`, `PRAGMA …`).
    Utility {
        text: String,
    },
    CreateSnapshot {
        name: String,
        table: String,
    },
    DropSnapshot {
        name: String,
    },
    RestoreSnapshot {
        name: String,
    },
    Delete {
        table: String,
        where_pred: Option<Predicate>,
    },
    Update {
        table: String,
        assignments: Vec<(String, ExprAst)>,
        where_pred: Option<Predicate>,
    },
    Show {
        text: String,
    },
    /// `SHOW TABLE <t> COLUMNS`
    ShowTableColumns {
        table: String,
    },
    /// `SHOW TABLE <t> INDEX <name>`
    ShowTableIndex {
        table: String,
        index: String,
    },
    /// `SET CONFIG <key> <value>`
    SetConfig {
        key: String,
        value: String,
    },
    /// `SHOW GLOBAL VARIABLE <key>`
    ShowVariable {
        key: String,
    },
    Copy {
        table: String,
        from: bool,
        path: String,
        opts: Vec<(String, String)>,
    },
    Explain {
        kind: ExplainKind,
        inner: Box<Statement>,
    },
    Insert {
        table: String,
        /// Optional explicit column list (`INSERT INTO t (a, b) VALUES …`).
        columns: Option<Vec<String>>,
        rows: Vec<Vec<Literal>>,
    },
    AlterTableAdd {
        table: String,
        columns: Vec<ColumnDef>,
    },
    AlterTableDropColumn {
        table: String,
        column: String,
    },
    /// `ALTER TABLE <t> RENAME TO <new>`
    RenameTable {
        table: String,
        new_name: String,
    },
    Select {
        items: Vec<SelectItem>,
        from: Option<String>,
        join: Option<JoinClause>,
        where_pred: Option<Predicate>,
        group_by: Vec<ExprAst>,
        /// Output aliases, aligned with `items` (`<expr> AS name`).
        aliases: Vec<Option<String>>,
        having: Option<Box<Predicate>>,
        /// ORDER BY keys, in priority order (empty when absent).
        order_by: Vec<OrderKey>,
        limit: Option<u64>,
        offset: Option<u64>,
        search: Option<Box<SearchClause>>,
        distinct: bool,
        /// `HIGHLIGHT <col>, …` after the select list: those output columns are
        /// rendered with `<em>…</em>` around the query term occurrences.
        highlight: Vec<String>,
    },
}

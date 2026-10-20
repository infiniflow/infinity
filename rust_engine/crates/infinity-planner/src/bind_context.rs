//! C++ `Binding` / `BindContext`: the name-resolution state of one query block.
//!
//! Port of `src/planner/binding.cppm` + `binding_impl.cpp` and
//! `src/planner/bind_context.cppm` + `bind_context_impl.cpp`, keeping the C++
//! field and method names in `snake_case`.
//!
//! Field translations that are not a rename:
//!
//! * `column_types_`, `table_info_`, `block_index_` are dropped: the Rust engine
//!   has no `DataType`, `TableInfo` or `BlockIndex` type. A binding keeps the
//!   column names and their positions, which is all name resolution reads.
//! * `select_expression_` is `Vec<SelectItem>`: the C++ vector holds `ParsedExpr*`
//!   of the select list, and `SelectItem` is the parsed select-list item in the
//!   Rust AST. `ExprAst` cannot represent `*` or an aggregate, so it cannot be
//!   the element type.
//! * `bound_cte_set_` is `HashSet<String>` of CTE aliases: C++ keys the set on
//!   the `CommonTableExpressionInfo` object identity, and the alias is that
//!   object's key in `cte_map_`, so alias equality is the same identity.
//! * `correlated_column_exprs_` is `Vec<ResolvedColumn>`: C++ stores
//!   `ColumnExpression` objects, which carry the `(table index, column index)`
//!   binding the map is keyed on. `infinity_expression::Expr::Column` only holds
//!   a position, so [`ResolvedColumn`] is the identity-carrying counterpart.
//! * `expression_binder_` has no counterpart: `binder.rs` has no
//!   `ExpressionBinder` to point at.
//!
//! C++ keeps the parent as a raw `BindContext *` and mutates the root's counters
//! through it. Rust cannot alias mutable state that way, so the parent and the
//! two children are `Rc<BindContext>`, the three counters and the
//! `knn_table_index_` they publish are `Cell`s, and the correlated-column pair
//! is `RefCell` (the only mutable state a method reaches through the parent).
//! `generate_*` therefore take `&self`, exactly like the reference, which reads
//! and bumps the root's counters.

use std::cell::{Cell, RefCell};
use std::collections::{HashMap, HashSet};
use std::rc::Rc;

use infinity_common::{Error, Result};
use infinity_expression::Expr;
use infinity_parser::{SelectItem, Statement};

use crate::ColumnBinding;

/// C++ `BindingType`.
#[derive(Debug, Clone, Copy, Default, PartialEq, Eq)]
pub enum BindingType {
    #[default]
    Invalid,
    Table,
    Subquery,
    Cte,
    View,
}

/// C++ `Binding`: one name a column can be qualified with (a table, a CTE, a
/// subquery or a view) and the columns it produces.
#[derive(Debug, Clone, Default, PartialEq, Eq)]
pub struct Binding {
    pub binding_type: BindingType,
    /// Binding table alias or name.
    pub table_name: String,
    pub table_index: usize,
    /// C++ `column_types_` / `table_info_` / `block_index_` have no Rust
    /// counterpart; only the names and their positions are kept.
    pub column_names: Vec<String>,
    /// Column name -> column index mapping.
    pub name2index: HashMap<String, usize>,
}

impl Binding {
    /// C++ `Binding::MakeBinding`. The C++ overload also carries `TableInfo`
    /// and `BlockIndex`; Rust has neither, so one constructor covers both.
    pub fn make(
        binding_type: BindingType,
        name: &str,
        table_index: usize,
        column_names: Vec<String>,
    ) -> Self {
        let name2index = column_names
            .iter()
            .enumerate()
            .map(|(idx, column_name)| (column_name.clone(), idx))
            .collect();
        Self {
            binding_type,
            table_name: name.to_string(),
            table_index,
            column_names,
            name2index,
        }
    }
}

/// C++ `CommonTableExpressionInfo`.
#[derive(Debug, Clone, PartialEq)]
pub struct CommonTableExpressionInfo {
    pub alias: String,
    /// C++ `SelectStatement *select_statement_`; the Rust AST spells a parsed
    /// `SELECT` as `Statement::Select`.
    pub select_statement: Box<Statement>,
    /// Names this CTE masks in the scope that declares it.
    pub masked_name_set: HashSet<String>,
}

impl CommonTableExpressionInfo {
    pub fn new(
        alias: String,
        select_statement: Box<Statement>,
        masked_name_set: HashSet<String>,
    ) -> Self {
        Self {
            alias,
            select_statement,
            masked_name_set,
        }
    }
}

/// C++ `ColumnIdentifier` (`src/planner/column_identifier.cppm`), reduced to the
/// two names `ResolveColumnId` reads. `MakeColumnIdentifier` keeps only the last
/// two components of a `db.schema.table.column` reference, which
/// [`ColumnIdentifier::parse`] reproduces.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct ColumnIdentifier {
    pub table_name: Option<String>,
    pub column_name: String,
}

impl ColumnIdentifier {
    pub fn new(table_name: Option<String>, column_name: String) -> Self {
        Self {
            table_name,
            column_name,
        }
    }

    /// Splits `table.column` (or `db.schema.table.column`); a leading database
    /// and schema name are dropped, as the reference does.
    pub fn parse(name: &str) -> Self {
        let mut parts: Vec<&str> = name.split('.').collect();
        if parts.len() < 2 {
            return Self::new(None, name.to_string());
        }
        let column_name = parts.pop().unwrap_or(name).to_string();
        let table_name = parts.pop().map(str::to_string);
        Self::new(table_name, column_name)
    }
}

/// C++ `ColumnIdentifier::ToString`: `table.column`, or just `column`.
impl std::fmt::Display for ColumnIdentifier {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match &self.table_name {
            Some(table_name) => write!(f, "{table_name}.{}", self.column_name),
            None => f.write_str(&self.column_name),
        }
    }
}

/// What `BindContext::ResolveColumnId` returns instead of a `ColumnExpression`:
/// the binding the reference column carries, plus the correlation depth.
#[derive(Debug, Clone, PartialEq, Eq, Hash)]
pub struct ResolvedColumn {
    pub table_name: String,
    pub table_index: usize,
    pub column_name: String,
    pub column_index: usize,
    pub depth: usize,
}

impl ResolvedColumn {
    /// C++ `ColumnExpression::binding()`.
    pub fn binding(&self) -> ColumnBinding {
        ColumnBinding::new(self.table_index, self.column_index)
    }

    /// C++ `ColumnExpression::depth()`.
    pub fn depth(&self) -> usize {
        self.depth
    }
}

/// C++ `KnnDistanceType`.
#[derive(Debug, Clone, Copy, Default, PartialEq, Eq)]
pub enum KnnDistanceType {
    #[default]
    Invalid,
    L2,
    Hamming,
    InnerProduct,
    Cosine,
}

/// C++ `SparseMetricType`.
#[derive(Debug, Clone, Copy, Default, PartialEq, Eq)]
pub enum SparseMetricType {
    #[default]
    Invalid,
    InnerProduct,
}

/// The `ParsedExpr` kinds `BoundSearch` discriminates on.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum MatchExpr {
    /// C++ `KnnExpr` with its `distance_type_`.
    Knn(KnnDistanceType),
    /// C++ `MatchSparseExpr` with its `metric_type_`.
    MatchSparse(SparseMetricType),
    /// C++ `MatchTensorExpr`.
    MatchTensor,
    /// C++ `MatchExpr`.
    Match,
}

/// C++ `SearchExpr`, reduced to what `BoundSearch` inspects. The Rust parser has
/// no SEARCH AST (the executor derives the search plan from the request), so the
/// fusion expressions are represented by their emptiness, which is all the
/// reference reads.
#[derive(Debug, Clone, Default, PartialEq, Eq)]
pub struct SearchExpr {
    pub match_exprs: Vec<MatchExpr>,
    pub has_fusion: bool,
}

impl SearchExpr {
    pub fn new(match_exprs: Vec<MatchExpr>, has_fusion: bool) -> Self {
        Self {
            match_exprs,
            has_fusion,
        }
    }
}

/// C++ `BindContext`.
#[derive(Debug, Clone)]
pub struct BindContext {
    /// Parent bind context.
    pub parent: Option<Rc<BindContext>>,

    /// Left and right child bind context.
    pub left_child: Option<Rc<BindContext>>,
    pub right_child: Option<Rc<BindContext>>,

    /// CTE alias -> CTE statement.
    pub cte_map: HashMap<String, Rc<CommonTableExpressionInfo>>,

    /// All bindings: subquery, CTE, view, table, …
    pub binding_by_name: HashMap<String, Binding>,
    /// Column name -> names of the bindings that produce it.
    pub binding_names_by_column: HashMap<String, Vec<String>>,

    /// Binding table.
    pub table_names: Vec<String>,
    pub table_name2table_index: HashMap<String, usize>,
    pub table_table_index2table_name: HashMap<usize, String>,

    /// Select list. (C++ `ParsedExpr *`; see the module comment.)
    pub select_expression: Vec<SelectItem>,

    /// Initialised at step 5 of binding a select statement.
    pub select_alias2index: HashMap<String, i64>,
    pub select_expr_name2index: HashMap<String, i64>,

    pub select_index_to_group_by_index: HashMap<i64, i64>,
    pub select_index_to_aggregate_index: HashMap<i64, i64>,

    /// Bound expressions of the GROUP BY list.
    pub group_by_table_index: usize,
    pub group_by_table_name: String,
    pub group_exprs: Vec<Expr>,
    pub group_index_by_name: HashMap<String, i64>,

    pub unnest_table_index: usize,
    pub unnest_table_name: String,
    pub unnest_exprs: Vec<Expr>,
    pub unnest_index_by_name: HashMap<String, i64>,

    /// Bound aggregate function expressions.
    pub aggregate_table_index: usize,
    pub aggregate_table_name: String,
    pub aggregate_exprs: Vec<Expr>,
    pub aggregate_index_by_name: HashMap<String, i64>,

    /// Bound expressions of the select list.
    pub project_table_index: usize,
    pub project_table_name: String,
    pub project_exprs: Vec<Expr>,
    pub project_index_by_name: HashMap<String, i64>,
    pub result_index: usize,

    /// The table index `GenerateTableIndex` published last.
    pub knn_table_index: Cell<usize>,

    /// Bound CTEs, keyed by alias (see the module comment).
    pub bound_cte_set: HashSet<String>,
    /// Bound views.
    pub bound_view_set: HashSet<String>,
    /// Bound base tables.
    pub bound_table_set: HashSet<String>,
    /// Bound subqueries.
    pub bound_subquery_set: HashSet<String>,

    /// Bound correlated columns.
    pub correlated_column_exprs: RefCell<Vec<ResolvedColumn>>,
    pub correlated_column_map: RefCell<HashMap<ColumnBinding, usize>>,

    /// A sequence id.
    pub binding_context_id: usize,

    pub single_row: bool,

    pub allow_distance: bool,
    pub allow_similarity: bool,
    pub allow_score: bool,

    next_logical_node_id: Cell<usize>,
    next_bind_context_index: Cell<usize>,
    next_table_index: Cell<usize>,
}

impl Default for BindContext {
    fn default() -> Self {
        Self::make(None)
    }
}

impl BindContext {
    /// C++ `BindContext::Make(parent)`.
    pub fn make(parent: Option<Rc<BindContext>>) -> Self {
        let mut context = Self {
            parent,
            left_child: None,
            right_child: None,
            cte_map: HashMap::new(),
            binding_by_name: HashMap::new(),
            binding_names_by_column: HashMap::new(),
            table_names: Vec::new(),
            table_name2table_index: HashMap::new(),
            table_table_index2table_name: HashMap::new(),
            select_expression: Vec::new(),
            select_alias2index: HashMap::new(),
            select_expr_name2index: HashMap::new(),
            select_index_to_group_by_index: HashMap::new(),
            select_index_to_aggregate_index: HashMap::new(),
            group_by_table_index: 0,
            group_by_table_name: String::new(),
            group_exprs: Vec::new(),
            group_index_by_name: HashMap::new(),
            unnest_table_index: 0,
            unnest_table_name: String::new(),
            unnest_exprs: Vec::new(),
            unnest_index_by_name: HashMap::new(),
            aggregate_table_index: 0,
            aggregate_table_name: String::new(),
            aggregate_exprs: Vec::new(),
            aggregate_index_by_name: HashMap::new(),
            project_table_index: 0,
            project_table_name: String::new(),
            project_exprs: Vec::new(),
            project_index_by_name: HashMap::new(),
            result_index: 0,
            knn_table_index: Cell::new(0),
            bound_cte_set: HashSet::new(),
            bound_view_set: HashSet::new(),
            bound_table_set: HashSet::new(),
            bound_subquery_set: HashSet::new(),
            correlated_column_exprs: RefCell::new(Vec::new()),
            correlated_column_map: RefCell::new(HashMap::new()),
            binding_context_id: 0,
            single_row: false,
            allow_distance: false,
            allow_similarity: false,
            allow_score: false,
            next_logical_node_id: Cell::new(1),
            next_bind_context_index: Cell::new(1),
            next_table_index: Cell::new(1),
        };
        context.binding_context_id = context.generate_binding_context_index();
        context
    }

    /// C++ `BindContext::Make(nullptr)`.
    pub fn new() -> Self {
        Self::make(None)
    }

    /// The root of the bind contexts this query is bound in. Walks from the
    /// current context's own parents; a child never holds its own counters.
    pub fn root(&self) -> &BindContext {
        match &self.parent {
            Some(parent) => parent.root(),
            None => self,
        }
    }

    /// C++ `BindContext::Destroy`. Rust owns its memory, so the reference's
    /// explicit release of the `ParsedExpr`s it generated has nothing to do:
    /// the `Rc`/`Vec` fields are dropped with the context.
    pub fn destroy(&self) {}

    pub fn add_left_child(&mut self, left_child: BindContext) -> Result<()> {
        let left_child = Rc::new(left_child);
        self.add_bind_context(&left_child)?;
        self.left_child = Some(left_child);
        Ok(())
    }

    pub fn add_right_child(&mut self, right_child: BindContext) -> Result<()> {
        let right_child = Rc::new(right_child);
        self.add_bind_context(&right_child)?;
        self.right_child = Some(right_child);
        Ok(())
    }

    pub fn get_new_logical_node_id(&self) -> usize {
        match &self.parent {
            Some(parent) => parent.get_new_logical_node_id(),
            None => {
                let id = self.next_logical_node_id.get();
                self.next_logical_node_id.set(id + 1);
                id
            }
        }
    }

    pub fn generate_binding_context_index(&self) -> usize {
        match &self.parent {
            Some(parent) => parent.generate_binding_context_index(),
            None => {
                let index = self.next_bind_context_index.get();
                self.next_bind_context_index.set(index + 1);
                index
            }
        }
    }

    pub fn generate_table_index(&self) -> usize {
        let index = match &self.parent {
            Some(parent) => parent.generate_table_index(),
            None => {
                let index = self.next_table_index.get();
                self.next_table_index.set(index + 1);
                index
            }
        };
        self.knn_table_index.set(index);
        index
    }

    /// C++ `GetCTE`: the CTE is visible here only when this scope declares it
    /// and its `masked_name_set` names it; otherwise the enclosing scope answers.
    pub fn get_cte(&self, name: &str) -> Option<&CommonTableExpressionInfo> {
        if let Some(cte) = self.cte_map.get(name) {
            if cte.masked_name_set.contains(name) {
                return Some(cte);
            }
        }
        match &self.parent {
            Some(parent) => parent.get_cte(name),
            None => None,
        }
    }

    pub fn is_cte_bound(&self, cte: &CommonTableExpressionInfo) -> bool {
        if self.bound_cte_set.contains(&cte.alias) {
            return true;
        }
        match &self.parent {
            Some(parent) => parent.is_cte_bound(cte),
            None => false,
        }
    }

    pub fn bound_cte(&mut self, cte: &CommonTableExpressionInfo) {
        self.bound_cte_set.insert(cte.alias.clone());
    }

    pub fn is_view_bound(&self, view_name: &str) -> bool {
        if self.bound_view_set.contains(view_name) {
            return true;
        }
        match &self.parent {
            Some(parent) => parent.is_view_bound(view_name),
            None => false,
        }
    }

    pub fn bound_view(&mut self, view_name: &str) {
        self.bound_view_set.insert(view_name.to_string());
    }

    pub fn is_table_bound(&self, table_name: &str) -> bool {
        if self.bound_table_set.contains(table_name) {
            return true;
        }
        match &self.parent {
            Some(parent) => parent.is_table_bound(table_name),
            None => false,
        }
    }

    pub fn bound_table(&mut self, table_name: &str) {
        self.bound_table_set.insert(table_name.to_string());
    }

    /// C++ `BoundSearch`: decides which of the SEARCH pseudo-columns
    /// (`DISTANCE()`, `SIMILARITY()`, `SCORE()`) the plan may expose.
    ///
    /// A fusion expression is scored; otherwise the first KNN distance type
    /// chooses between distance and similarity, and a second KNN or sparse
    /// expression with a *different* type clears both (the reference's
    /// `conflict` branch).
    pub fn bound_search(&mut self, expr: Option<&SearchExpr>) -> Result<()> {
        let Some(search_expr) = expr else {
            return Ok(());
        };
        self.allow_score = search_expr.has_fusion;
        if search_expr.has_fusion {
            return Ok(());
        }

        let mut first_distance_type = KnnDistanceType::Invalid;
        let mut first_metric_type = SparseMetricType::Invalid;
        for match_expr in &search_expr.match_exprs {
            match match_expr {
                MatchExpr::Knn(distance_type) => {
                    if first_distance_type == KnnDistanceType::Invalid {
                        first_distance_type = *distance_type;
                        match first_distance_type {
                            KnnDistanceType::L2 | KnnDistanceType::Hamming => {
                                self.allow_distance = true;
                            }
                            KnnDistanceType::InnerProduct | KnnDistanceType::Cosine => {
                                self.allow_similarity = true;
                            }
                            KnnDistanceType::Invalid => {
                                return Err(Error::Unrecoverable("Invalid KNN metric type".into()))
                            }
                        }
                    } else if first_distance_type != *distance_type {
                        self.allow_distance = false;
                        self.allow_similarity = false;
                        break;
                    }
                }
                MatchExpr::MatchSparse(metric_type) => {
                    if first_metric_type == SparseMetricType::Invalid {
                        first_metric_type = *metric_type;
                        match first_metric_type {
                            SparseMetricType::InnerProduct => self.allow_similarity = true,
                            SparseMetricType::Invalid => {
                                return Err(Error::Unrecoverable(
                                    "Invalid sparse metric type".into(),
                                ))
                            }
                        }
                    } else if first_metric_type != *metric_type {
                        self.allow_distance = false;
                        self.allow_similarity = false;
                        break;
                    }
                }
                MatchExpr::MatchTensor | MatchExpr::Match => self.allow_score = true,
            }
        }
        Ok(())
    }

    pub fn add_subquery_binding(
        &mut self,
        name: &str,
        table_index: usize,
        column_names: Vec<String>,
    ) {
        let binding = Binding::make(BindingType::Subquery, name, table_index, column_names);
        self.add_binding(binding);
        // Consider the subquery as the table.
        self.add_table_entry(name, table_index);
    }

    pub fn add_cte_binding(&mut self, name: &str, table_index: usize, column_names: Vec<String>) {
        let binding = Binding::make(BindingType::Cte, name, table_index, column_names);
        self.add_binding(binding);
        // Consider the CTE as the table.
        self.add_table_entry(name, table_index);
    }

    /// C++ `AddViewBinding`: the reference registers a view only as a binding —
    /// unlike a subquery/CTE it is *not* added to `table_names_` or the two
    /// table-index maps. Copied verbatim, not "fixed".
    pub fn add_view_binding(&mut self, name: &str, table_index: usize, column_names: Vec<String>) {
        let binding = Binding::make(BindingType::View, name, table_index, column_names);
        self.add_binding(binding);
    }

    pub fn add_table_binding(
        &mut self,
        table_name: &str,
        table_index: usize,
        column_names: Vec<String>,
    ) {
        let binding = Binding::make(BindingType::Table, table_name, table_index, column_names);
        self.add_binding(binding);
        self.add_table_entry(table_name, table_index);
    }

    fn add_table_entry(&mut self, name: &str, table_index: usize) {
        self.table_names.push(name.to_string());
        self.table_name2table_index
            .insert(name.to_string(), table_index);
        self.table_table_index2table_name
            .insert(table_index, name.to_string());
    }

    /// Merge `other` into this bind context.
    ///
    /// A duplicate is an internal invariant violation, not a user error: C++
    /// raises `UnrecoverableError` here, which the SDKs report with code 1.
    pub fn add_bind_context(&mut self, other: &BindContext) -> Result<()> {
        for table_name in &other.table_names {
            self.table_names.push(table_name.clone());
        }

        for (table_name, table_index) in &other.table_name2table_index {
            if self.table_name2table_index.contains_key(table_name) {
                return Err(Error::Unrecoverable(format!(
                    "{table_name} was bound before"
                )));
            }
            self.table_name2table_index
                .insert(table_name.clone(), *table_index);
        }

        for (table_index, table_name) in &other.table_table_index2table_name {
            if self.table_table_index2table_name.contains_key(table_index) {
                return Err(Error::Unrecoverable(format!(
                    "Table index: {table_index} is bound before"
                )));
            }
            self.table_table_index2table_name
                .insert(*table_index, table_name.clone());
        }

        for (binding_name, binding) in &other.binding_by_name {
            if self.binding_by_name.contains_key(binding_name) {
                return Err(Error::Unrecoverable(format!(
                    "Table: {binding_name} was bound before"
                )));
            }
            self.binding_by_name
                .insert(binding_name.clone(), binding.clone());
        }

        for (column_name, binding_names) in &other.binding_names_by_column {
            match self.binding_names_by_column.get_mut(column_name) {
                Some(existing) => existing.extend(binding_names.iter().cloned()),
                None => {
                    self.binding_names_by_column
                        .insert(column_name.clone(), binding_names.clone());
                }
            }
        }

        for correlated_column in other.correlated_column_exprs.borrow().iter() {
            let mut map = self.correlated_column_map.borrow_mut();
            if map.contains_key(&correlated_column.binding()) {
                continue;
            }
            let index = map.len();
            map.insert(correlated_column.binding(), index);
            drop(map);
            self.correlated_column_exprs
                .borrow_mut()
                .push(correlated_column.clone());
        }

        Ok(())
    }

    /// C++ `ResolveColumnId`: resolves `column` or `table.column` against this
    /// scope and then the enclosing ones. `Ok(None)` means "not bound here or
    /// above"; a column that some binding in *this* scope produces but cannot
    /// yield is a hard error, as in the reference.
    pub fn resolve_column_id(
        &self,
        column_identifier: &ColumnIdentifier,
        depth: usize,
    ) -> Result<Option<ResolvedColumn>> {
        let column_name_ref = column_identifier.column_name.as_str();

        let bound_column = match &column_identifier.table_name {
            None => match self.binding_names_by_column.get(column_name_ref) {
                Some(binding_names) => {
                    if binding_names.len() > 1 {
                        return Err(Error::Invalid(format!(
                            "Ambiguous column table_name: {column_identifier}"
                        )));
                    }
                    let binding_name = &binding_names[0];
                    let Some(binding) = self.binding_by_name.get(binding_name) else {
                        return Err(Error::Invalid(format!(
                            "{column_identifier} doesn't exist."
                        )));
                    };
                    Some(self.make_resolved(
                        binding_name,
                        binding,
                        column_name_ref,
                        column_identifier,
                        depth,
                    )?)
                }
                // Not bound in this context; the enclosing one may have it.
                None => None,
            },
            Some(table_name_ref) => match self.binding_by_name.get(table_name_ref) {
                Some(binding) => Some(self.make_resolved(
                    table_name_ref,
                    binding,
                    column_name_ref,
                    column_identifier,
                    depth,
                )?),
                None => None,
            },
        };

        if bound_column.is_some() {
            return Ok(bound_column);
        }

        match &self.parent {
            Some(parent) => parent.resolve_column_id(column_identifier, depth + 1),
            None => Ok(None),
        }
    }

    fn make_resolved(
        &self,
        table_name: &str,
        binding: &Binding,
        column_name: &str,
        column_identifier: &ColumnIdentifier,
        depth: usize,
    ) -> Result<ResolvedColumn> {
        let Some(&column_index) = binding.name2index.get(column_name) else {
            return Err(Error::Invalid(format!(
                "{column_identifier} doesn't exist."
            )));
        };
        Ok(ResolvedColumn {
            table_name: table_name.to_string(),
            table_index: binding.table_index,
            column_name: column_name.to_string(),
            column_index,
            depth,
        })
    }

    /// C++ `AddCorrelatedColumnExpr`: the enclosing contexts record the column
    /// too, not just the current one.
    pub fn add_correlated_column_expr(&self, correlated_column: &ResolvedColumn) {
        if let Some(parent) = &self.parent {
            parent.add_correlated_column_expr(correlated_column);
        }
        let mut map = self.correlated_column_map.borrow_mut();
        if map.contains_key(&correlated_column.binding()) {
            return;
        }
        let index = map.len();
        map.insert(correlated_column.binding(), index);
        drop(map);
        self.correlated_column_exprs
            .borrow_mut()
            .push(correlated_column.clone());
    }

    pub fn has_correlated_column(&self) -> bool {
        !self.correlated_column_exprs.borrow().is_empty()
    }

    pub fn get_binding_from_current_or_parent_by_name(
        &self,
        binding_name: &str,
    ) -> Option<&Binding> {
        match self.binding_by_name.get(binding_name) {
            Some(binding) => Some(binding),
            None => match &self.parent {
                Some(parent) => parent.get_binding_from_current_or_parent_by_name(binding_name),
                None => None,
            },
        }
    }

    /// C++ `AddBinding`, plus its bookkeeping: every column name gains the
    /// binding's table name as a producer.
    fn add_binding(&mut self, binding: Binding) {
        let table_name = binding.table_name.clone();
        for column_name in &binding.column_names {
            self.binding_names_by_column
                .entry(column_name.clone())
                .or_default()
                .push(table_name.clone());
        }
        self.binding_by_name.entry(table_name).or_insert(binding);
    }

    // ---- Adapters, no C++ counterpart -------------------------------------

    /// The bind context a `FROM`/`JOIN` list binds in: the root context of the
    /// select statement with one table binding per entry, in FROM order.
    /// C++ does this in `QueryBinder::BuildFromClause`.
    pub fn from_tables(tables: &[(String, Vec<String>)]) -> Self {
        let mut context = Self::make(None);
        for (table, columns) in tables {
            let table_index = context.generate_table_index();
            context.add_table_binding(table, table_index, columns.clone());
        }
        context
    }

    /// Table index -> first position of that table's columns in the flat
    /// `FROM` schema (`combined_schema`). C++ has no such map: its
    /// `ColumnExpression` keeps the table index and the executor addresses
    /// columns per binding, while the Rust plan addresses one flat tuple.
    pub fn flat_offsets(&self) -> HashMap<usize, usize> {
        let mut offsets = HashMap::new();
        let mut offset = 0;
        for table_name in &self.table_names {
            let Some(&table_index) = self.table_name2table_index.get(table_name) else {
                continue;
            };
            let width = self
                .binding_by_name
                .get(table_name)
                .map_or(0, |binding| binding.column_names.len());
            offsets.entry(table_index).or_insert(offset);
            offset += width;
        }
        offsets
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn cte(alias: &str, masked: &[&str]) -> CommonTableExpressionInfo {
        let statement = infinity_parser::parse("SELECT id FROM t")
            .expect("the CTE body parses")
            .remove(0);
        CommonTableExpressionInfo::new(
            alias.to_string(),
            Box::new(statement),
            masked.iter().map(|m| m.to_string()).collect(),
        )
    }

    fn table_context(table: &str, index: usize, columns: &[&str]) -> BindContext {
        let mut context = BindContext::make(None);
        context.add_table_binding(
            table,
            index,
            columns.iter().map(|c| c.to_string()).collect(),
        );
        context
    }

    /// `GetNewLogicalNodeId` / `GenerateBindingContextIndex` /
    /// `GenerateTableIndex` hand out one sequence: a child and its parent never
    /// see the same index twice.
    #[test]
    fn index_generators_delegate_to_the_root() {
        let root = Rc::new(BindContext::make(None));
        let child = BindContext::make(Some(root.clone()));
        assert_eq!(root.generate_table_index(), 1);
        assert_eq!(child.generate_table_index(), 2);
        assert_eq!(root.generate_table_index(), 3);
        assert_eq!(child.knn_table_index.get(), 2);

        assert_eq!(root.get_new_logical_node_id(), 1);
        assert_eq!(child.get_new_logical_node_id(), 2);

        let first = child.binding_context_id;
        let second = BindContext::make(Some(root.clone())).binding_context_id;
        assert_eq!(root.binding_context_id, 1);
        assert_eq!(first, 2);
        assert_eq!(second, 3);
    }

    /// An unqualified name binds through the one binding that produces it; the
    /// same name in two bindings is ambiguous.
    #[test]
    fn unqualified_column_resolves_or_is_ambiguous() {
        let mut context = table_context("t1", 1, &["id", "name"]);
        assert_eq!(
            context
                .resolve_column_id(&ColumnIdentifier::parse("name"), 0)
                .unwrap(),
            Some(ResolvedColumn {
                table_name: "t1".to_string(),
                table_index: 1,
                column_name: "name".to_string(),
                column_index: 1,
                depth: 0,
            })
        );

        // `AddBinding` records the producer of every column name.
        assert_eq!(
            context.binding_names_by_column.get("id"),
            Some(&vec!["t1".to_string()])
        );

        context.add_table_binding("t2", 2, vec!["id".to_string()]);
        let err = context
            .resolve_column_id(&ColumnIdentifier::parse("id"), 0)
            .unwrap_err();
        assert_eq!(
            err.to_string(),
            "invalid argument: Ambiguous column table_name: id"
        );
        // Qualifying the name disambiguates it.
        assert_eq!(
            context
                .resolve_column_id(&ColumnIdentifier::parse("t2.id"), 0)
                .unwrap()
                .unwrap()
                .binding(),
            ColumnBinding::new(2, 0)
        );
    }

    /// A qualified name binds to that binding's `(table_index, column_index)`.
    #[test]
    fn qualified_column_binds_to_its_binding() {
        let mut context = table_context("t1", 3, &["id", "name"]);
        context.add_table_binding("t2", 4, vec!["id".to_string(), "name".to_string()]);
        let resolved = context
            .resolve_column_id(&ColumnIdentifier::parse("t2.name"), 0)
            .unwrap()
            .unwrap();
        assert_eq!(
            (resolved.table_index, resolved.column_index),
            (4, 1),
            "{resolved:?}"
        );
        assert_eq!(resolved.binding(), ColumnBinding::new(4, 1));
        // A database/schema-qualified reference names the same table.
        assert_eq!(
            context
                .resolve_column_id(&ColumnIdentifier::parse("db.t1.id"), 0)
                .unwrap()
                .unwrap()
                .binding(),
            ColumnBinding::new(3, 0)
        );
    }

    /// Unknown columns and unknown tables carry the reference's message.
    #[test]
    fn unknown_column_and_table_use_the_reference_message() {
        let context = table_context("t1", 1, &["id"]);
        // Nothing produces the name here, so the reference returns `nullptr`
        // for the enclosing scope (or for the planner) to report.
        assert_eq!(
            context
                .resolve_column_id(&ColumnIdentifier::parse("nope"), 0)
                .unwrap(),
            None
        );
        // A known table without that column is an error, not "not bound": the
        // reference raises it from inside the binding.
        assert_eq!(
            context
                .resolve_column_id(&ColumnIdentifier::parse("t1.nope"), 0)
                .unwrap_err()
                .to_string(),
            "invalid argument: t1.nope doesn't exist."
        );
        // An unknown qualifier is "not bound here", for the parent to answer.
        assert_eq!(
            context
                .resolve_column_id(&ColumnIdentifier::parse("t9.id"), 0)
                .unwrap(),
            None
        );
    }

    /// `ResolveColumnId` walks to the parent and reports the depth it bound at.
    #[test]
    fn resolution_walks_to_the_parent() {
        let root = Rc::new(table_context("t1", 1, &["id"]));
        let child = BindContext::make(Some(root));
        let resolved = child
            .resolve_column_id(&ColumnIdentifier::parse("id"), 0)
            .unwrap()
            .unwrap();
        assert_eq!(resolved.table_index, 1);
        assert_eq!(resolved.depth, 1);
    }

    /// `AddBindContext` merges the tables, both index maps and the bindings,
    /// appends the column producers, and rejects a duplicate table name.
    #[test]
    fn add_bind_context_merges_and_rejects_duplicates() {
        let mut merged = table_context("t1", 1, &["id", "k"]);
        let mut other = BindContext::make(None);
        other.generate_table_index(); // consumes index 1 of its own root
        other.add_table_binding("t2", 2, vec!["k".to_string()]);
        merged.add_bind_context(&other).unwrap();

        assert_eq!(merged.table_names, vec!["t1".to_string(), "t2".to_string()]);
        assert_eq!(merged.table_name2table_index.get("t2"), Some(&2));
        assert_eq!(
            merged.table_table_index2table_name.get(&2),
            Some(&"t2".to_string())
        );
        assert!(merged.binding_by_name.contains_key("t2"));
        // Appended, not replaced.
        assert_eq!(
            merged.binding_names_by_column.get("k"),
            Some(&vec!["t1".to_string(), "t2".to_string()])
        );
        assert_eq!(
            merged.flat_offsets(),
            [(1, 0), (2, 2)].into_iter().collect::<HashMap<_, _>>()
        );

        let mut duplicate = BindContext::make(None);
        duplicate.add_table_binding("t1", 9, vec!["id".to_string()]);
        let err = merged.add_bind_context(&duplicate).unwrap_err();
        // `UnrecoverableError` in C++: the bare message, code 1.
        assert_eq!(err.to_string(), "t1 was bound before");
        assert_eq!(err.status_code(), 1);

        // A distinct name reusing a bound table index.
        let mut duplicate_index = BindContext::make(None);
        duplicate_index.add_table_binding("t3", 1, vec!["id".to_string()]);
        assert_eq!(
            merged
                .add_bind_context(&duplicate_index)
                .unwrap_err()
                .to_string(),
            "Table index: 1 is bound before"
        );

        // A distinct table name reusing a bound binding name: a view registers a
        // binding without a table entry, so only `binding_by_name` collides.
        let mut duplicate_binding = BindContext::make(None);
        duplicate_binding.add_view_binding("t1", 50, vec!["id".to_string()]);
        assert_eq!(
            merged
                .add_bind_context(&duplicate_binding)
                .unwrap_err()
                .to_string(),
            "Table: t1 was bound before"
        );
    }

    /// `GetCTE` returns a CTE only when this scope's `masked_name_set` names it,
    /// and otherwise asks the parent.
    #[test]
    fn get_cte_respects_the_masked_name_set() {
        let root = Rc::new({
            let mut root = BindContext::make(None);
            root.cte_map
                .insert("visible".to_string(), Rc::new(cte("visible", &["visible"])));
            root.cte_map
                .insert("masked".to_string(), Rc::new(cte("masked", &[])));
            root
        });
        assert_eq!(
            root.get_cte("visible").map(|c| c.alias.clone()),
            Some("visible".to_string())
        );
        // Declared here but masked by a nearer scope: invisible.
        assert!(root.get_cte("masked").is_none());

        let child = BindContext::make(Some(root));
        assert_eq!(
            child.get_cte("visible").map(|c| c.alias.clone()),
            Some("visible".to_string())
        );
        assert!(child.get_cte("absent").is_none());
    }

    /// The CTE/view/table bound sets are visible from a child scope.
    #[test]
    fn bound_sets_are_visible_from_a_child() {
        let mut root = BindContext::make(None);
        let info = cte("c", &["c"]);
        root.bound_cte(&info);
        root.bound_view("v");
        root.bound_table("t");
        let child = BindContext::make(Some(Rc::new(root.clone())));
        assert!(child.is_cte_bound(&info));
        assert!(child.is_view_bound("v"));
        assert!(child.is_table_bound("t"));
        assert!(!child.is_table_bound("other"));
    }

    /// `AddViewBinding` only registers a binding (no table entry), exactly as
    /// `bind_context_impl.cpp:165` does.
    #[test]
    fn add_view_binding_registers_no_table() {
        let mut context = BindContext::make(None);
        context.add_view_binding("v", 7, vec!["a".to_string()]);
        assert!(context.binding_by_name.contains_key("v"));
        assert!(context.table_names.is_empty());
        assert!(context.table_name2table_index.is_empty());
        assert_eq!(context.binding_by_name["v"].binding_type, BindingType::View);
    }

    /// `BoundSearch`: fusion scores; the first KNN distance type picks
    /// distance vs similarity; a conflicting type clears both.
    #[test]
    fn bound_search_decides_the_allowed_pseudo_columns() {
        let mut context = BindContext::make(None);
        context
            .bound_search(Some(&SearchExpr::new(
                vec![MatchExpr::Knn(KnnDistanceType::L2)],
                false,
            )))
            .unwrap();
        assert!(context.allow_distance);
        assert!(!context.allow_similarity);
        assert!(!context.allow_score);

        let mut context = BindContext::make(None);
        context
            .bound_search(Some(&SearchExpr::new(
                vec![MatchExpr::Knn(KnnDistanceType::Cosine)],
                false,
            )))
            .unwrap();
        assert!(context.allow_similarity);

        // The same distance type twice is not a conflict.
        let mut context = BindContext::make(None);
        context
            .bound_search(Some(&SearchExpr::new(
                vec![
                    MatchExpr::Knn(KnnDistanceType::InnerProduct),
                    MatchExpr::Knn(KnnDistanceType::InnerProduct),
                ],
                false,
            )))
            .unwrap();
        assert!(context.allow_similarity);

        // Different types clear both.
        let mut context = BindContext::make(None);
        context
            .bound_search(Some(&SearchExpr::new(
                vec![
                    MatchExpr::Knn(KnnDistanceType::L2),
                    MatchExpr::Knn(KnnDistanceType::Cosine),
                ],
                false,
            )))
            .unwrap();
        assert!(!context.allow_distance);
        assert!(!context.allow_similarity);

        // A full-text MATCH scores.
        let mut context = BindContext::make(None);
        context
            .bound_search(Some(&SearchExpr::new(vec![MatchExpr::Match], false)))
            .unwrap();
        assert!(context.allow_score);

        // Fusion wins outright.
        let mut context = BindContext::make(None);
        context
            .bound_search(Some(&SearchExpr::new(
                vec![MatchExpr::Knn(KnnDistanceType::L2)],
                true,
            )))
            .unwrap();
        assert!(context.allow_score);
        assert!(!context.allow_distance);

        // `nullptr` changes nothing.
        let mut context = BindContext::make(None);
        context.bound_search(None).unwrap();
        assert!(!context.allow_score);
    }

    /// `AddCorrelatedColumnExpr` records the column in the current context *and*
    /// in every enclosing one, and dedupes by binding.
    #[test]
    fn correlated_columns_are_recorded_in_the_parent_chain() {
        let root = Rc::new(BindContext::make(None));
        let child = BindContext::make(Some(root.clone()));
        let column = ResolvedColumn {
            table_name: "t".to_string(),
            table_index: 1,
            column_name: "id".to_string(),
            column_index: 0,
            depth: 1,
        };
        child.add_correlated_column_expr(&column);
        child.add_correlated_column_expr(&column);
        assert!(child.has_correlated_column());
        assert!(root.has_correlated_column());
        assert_eq!(child.correlated_column_exprs.borrow().len(), 1);
        assert_eq!(
            child
                .correlated_column_map
                .borrow()
                .get(&ColumnBinding::new(1, 0)),
            Some(&0)
        );
    }
}

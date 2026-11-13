//! `PhysicalProject` (`physical_project_impl.cpp:53`): evaluates one expression
//! per output column, emitting one row per input row.

use ::common::Result;
use expression::{Expr, Value};

use super::{OperatorContext, OperatorStates, PhysicalOperator, PhysicalOperatorType};
use crate::chunk::DataChunk;

#[cfg(test)]
thread_local! {
    static PROJECTED_ROWS: std::cell::Cell<usize> = const { std::cell::Cell::new(0) };
}

/// Rows evaluated by projections, so a test can show that LIMIT stops the work
/// early.
#[cfg(test)]
pub fn projected_rows() -> usize {
    PROJECTED_ROWS.with(|rows| rows.get())
}

/// A projection operator.
#[derive(Debug, Clone)]
pub struct Projection {
    exprs: Vec<Expr>,
    names: Vec<String>,
}

impl Projection {
    pub fn new(exprs: Vec<Expr>, names: Vec<String>) -> Self {
        Self { exprs, names }
    }

    /// Whether this is a positional `SELECT *` over an equally wide input: the
    /// chunks can then be handed on untouched instead of copied cell by cell.
    fn is_identity(&self, block: &DataChunk) -> bool {
        self.exprs.len() == block.column_count()
            && self
                .exprs
                .iter()
                .enumerate()
                .all(|(i, expr)| matches!(expr, Expr::Column(j) if *j == i))
    }

    fn project(&self, block: DataChunk) -> Result<DataChunk> {
        if self.is_identity(&block) {
            return Ok(block);
        }
        #[cfg(test)]
        PROJECTED_ROWS.with(|rows| rows.set(rows.get() + block.row_count()));
        let mut out = DataChunk::with_capacity(self.exprs.len(), block.capacity());
        for row in 0..block.row_count() {
            let input: Vec<_> = (0..block.column_count())
                .map(|column| block.get(row, column).cloned().unwrap_or(Value::Null))
                .collect();
            let projected: Vec<_> = self.exprs.iter().map(|expr| expr.eval(&input)).collect();
            out.push_row(&projected)?;
        }
        out.finalize();
        Ok(out)
    }
}

impl PhysicalOperator for Projection {
    fn operator_type(&self) -> PhysicalOperatorType {
        PhysicalOperatorType::Projection
    }

    /// Row-local and stateless, so a tasklet can run its own copy.
    fn fork(&self) -> Option<Box<dyn PhysicalOperator>> {
        Some(Box::new(self.clone()))
    }

    fn output_width(&self, _input: usize) -> usize {
        self.exprs.len()
    }

    fn output_names(&self) -> Vec<String> {
        self.names.clone()
    }

    fn execute(
        &self,
        _ctx: &mut OperatorContext,
        states: &mut OperatorStates,
        id: usize,
    ) -> Result<bool> {
        let prev_complete = states.prev_complete(id);
        let empty_source = states.prev_empty_source(id);
        let blocks = states.take_prev(id);

        let mut output = Vec::with_capacity(blocks.len().max(1));
        if empty_source {
            // The reference appends one default block so the output schema
            // survives an empty relation (`physical_project_impl.cpp:56`).
            let mut block = DataChunk::new(self.exprs.len());
            block.finalize();
            output.push(block);
        } else {
            for block in blocks {
                output.push(self.project(block)?);
            }
        }

        let state = states.state_mut(id);
        state.empty_source = empty_source;
        state.data_blocks.extend(output);
        if prev_complete {
            state.set_complete();
        }
        Ok(true)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn expr_column(i: usize) -> Expr {
        Expr::Column(i)
    }

    fn run(
        projection: Projection,
        blocks: Vec<DataChunk>,
        empty_source: bool,
    ) -> (Vec<Vec<Value>>, bool) {
        let operators: Vec<Box<dyn PhysicalOperator>> = vec![Box::new(projection)];
        let mut ctx = OperatorContext::default();
        let mut states = OperatorStates::new(&operators);
        states.source.data_blocks = blocks;
        states.source.complete = true;
        states.source.empty_source = empty_source;
        let mut progressed = true;
        while !states.is_complete() && progressed {
            progressed = super::super::execute_task(&operators, &mut ctx, &mut states).unwrap();
        }
        let rows = std::mem::take(&mut states.state_mut(0).data_blocks)
            .into_iter()
            .flat_map(DataChunk::into_rows)
            .collect();
        (rows, states.state(0).empty_source)
    }

    #[test]
    fn projects_one_row_per_input_row() {
        let blocks = DataChunk::from_rows(
            2,
            vec![
                vec![Value::Int(1), Value::Int(10)],
                vec![Value::Int(2), Value::Int(20)],
            ],
        );
        let projection = Projection::new(
            vec![Expr::Binary(
                Box::new(expr_column(0)),
                parser::BinOp::Add,
                Box::new(expr_column(1)),
            )],
            vec!["sum".into()],
        );
        let (rows, _) = run(projection, blocks, false);
        assert_eq!(rows, vec![vec![Value::Int(11)], vec![Value::Int(22)]]);
    }

    #[test]
    fn an_empty_source_still_publishes_the_column_names() {
        let projection = Projection::new(vec![expr_column(0)], vec!["c1".into()]);
        let (rows, empty) = run(projection, Vec::new(), true);
        assert!(rows.is_empty());
        assert!(empty);
    }

    #[test]
    fn the_identity_projection_hands_the_chunk_on() {
        let blocks = DataChunk::from_rows(2, vec![vec![Value::Int(1), Value::Int(2)]]);
        let projection = Projection::new(
            vec![expr_column(0), expr_column(1)],
            vec!["a".into(), "b".into()],
        );
        let (rows, _) = run(projection, blocks, false);
        assert_eq!(rows, vec![vec![Value::Int(1), Value::Int(2)]]);
    }
}

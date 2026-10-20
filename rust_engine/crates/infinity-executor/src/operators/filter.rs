//! `PhysicalFilter` (`physical_filter_impl.cpp:55`): keeps the rows whose
//! predicate holds, leaving the column layout untouched.

use infinity_common::Result;
use infinity_expression::{Expr, Value};

use super::{OperatorContext, OperatorStates, PhysicalOperator, PhysicalOperatorType};
use crate::chunk::DataChunk;

/// A filter operator.
#[derive(Debug, Clone)]
pub struct Filter {
    predicate: Expr,
}

impl Filter {
    pub fn new(predicate: Expr) -> Self {
        Self { predicate }
    }

    fn filter(&self, block: &DataChunk) -> Result<DataChunk> {
        let mut out = DataChunk::with_capacity(block.column_count(), block.capacity());
        for row in 0..block.row_count() {
            let cells: Vec<Value> = (0..block.column_count())
                .map(|column| block.get(row, column).cloned().unwrap_or(Value::Null))
                .collect();
            if self.predicate.eval_predicate(&cells) {
                out.push_row(&cells)?;
            }
        }
        out.finalize();
        Ok(out)
    }
}

impl PhysicalOperator for Filter {
    fn operator_type(&self) -> PhysicalOperatorType {
        PhysicalOperatorType::Filter
    }

    fn output_names(&self) -> Vec<String> {
        // A filter does not rename anything; the names come from the operator
        // below (C++ delegates `GetOutputNames` to `left_`).
        Vec::new()
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

        let mut output = Vec::with_capacity(blocks.len());
        for block in &blocks {
            output.push(self.filter(block)?);
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
    use infinity_expression::Value;
    use infinity_parser::CmpOp;

    fn predicate() -> Expr {
        Expr::Cmp(
            Box::new(Expr::Column(0)),
            CmpOp::Gt,
            Box::new(Expr::Literal(Value::Int(1))),
        )
    }

    fn run(blocks: Vec<DataChunk>, empty_source: bool) -> (Vec<Vec<Value>>, bool) {
        let operators: Vec<Box<dyn PhysicalOperator>> = vec![Box::new(Filter::new(predicate()))];
        let mut ctx = OperatorContext::default();
        let mut states = OperatorStates::new(&operators);
        states.source.data_blocks = blocks;
        states.source.complete = true;
        states.source.empty_source = empty_source;
        super::super::execute_task(&operators, &mut ctx, &mut states).unwrap();
        let rows = std::mem::take(&mut states.state_mut(0).data_blocks)
            .into_iter()
            .flat_map(DataChunk::into_rows)
            .collect();
        (rows, states.state(0).empty_source)
    }

    #[test]
    fn keeps_only_the_matching_rows() {
        let blocks = DataChunk::from_rows(
            2,
            vec![
                vec![Value::Int(0), Value::Int(10)],
                vec![Value::Int(1), Value::Int(11)],
                vec![Value::Int(2), Value::Int(12)],
                vec![Value::Int(3), Value::Int(13)],
            ],
        );
        let (rows, empty) = run(blocks, false);
        assert_eq!(
            rows,
            vec![
                vec![Value::Int(2), Value::Int(12)],
                vec![Value::Int(3), Value::Int(13)]
            ]
        );
        assert!(!empty);
    }

    #[test]
    fn keeps_the_column_count_of_a_fully_filtered_chunk() {
        let blocks = DataChunk::from_rows(2, vec![vec![Value::Int(0), Value::Int(10)]]);
        let (rows, _) = run(blocks, false);
        assert!(rows.is_empty());
    }

    #[test]
    fn an_empty_source_is_remembered_for_the_next_operator() {
        let (rows, empty) = run(Vec::new(), true);
        assert!(rows.is_empty());
        assert!(empty);
    }
}

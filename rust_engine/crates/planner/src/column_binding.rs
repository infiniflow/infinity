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

//! A `(table index, column index)` pair.
//!
//! C++: `src/planner/column_binding.cppm`. Used as a stable column identity in
//! a logical plan and as a hash-map key.

/// C++ `ColumnBinding`.
#[derive(Debug, Clone, Copy, Default, PartialEq, Eq, PartialOrd, Ord, Hash)]
pub struct ColumnBinding {
    pub table_idx: usize,
    pub column_idx: usize,
}

impl ColumnBinding {
    pub fn new(table_idx: usize, column_idx: usize) -> Self {
        Self {
            table_idx,
            column_idx,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::ColumnBinding;

    /// C++ `src/unit_test/planner/column_binding_ut.cpp:33 ColumnBindingTest.test1`.
    #[test]
    fn column_binding_test1() {
        let column1 = ColumnBinding::new(0, 1);
        let column2 = ColumnBinding::new(0, 1);
        let column3 = ColumnBinding::new(1, 2);

        assert_eq!(column1, column2);
        assert_ne!(column1, column3);
    }
}

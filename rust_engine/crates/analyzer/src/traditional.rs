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

use super::*;

/// C++ `TraditionalChineseAnalyzer` — OpenCC trad→simp, then `ChineseAnalyzer`.
#[derive(Clone)]
pub struct TraditionalChineseAnalyzer {
    pub(crate) converter: Arc<OpenCcConverter>,
    pub(crate) chinese: ChineseAnalyzer,
}

impl TraditionalChineseAnalyzer {
    pub fn new(converter: Arc<OpenCcConverter>, chinese: ChineseAnalyzer) -> Self {
        Self { converter, chinese }
    }

    /// C++ `OpenCC::convert`.
    pub fn convert(&self, input: &str) -> String {
        self.converter.convert(input)
    }

    /// C++ `TraditionalChineseAnalyzer::SetCutGrain`.
    pub fn set_cut_grain(&mut self, cut_grain: CutGrain) {
        self.chinese.set_cut_grain(cut_grain);
    }

    /// C++ `TraditionalChineseAnalyzer::Parse`.
    pub fn analyze(&self, input: &str) -> Vec<Term> {
        self.chinese.analyze(&self.convert(input))
    }
}

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

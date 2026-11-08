use super::*;

/// C++: `SmallFloat::Float122ToUInt16` (`smallfloat.cppm`).
pub(crate) fn float122_to_uint16(f: f32) -> u16 {
    let bits = f.to_bits() as i32;
    let smallfloat = bits >> (23 - 12);
    let zero = (127 - 2) << 12;
    if smallfloat < zero {
        if bits <= 0 {
            0
        } else {
            1
        }
    } else if smallfloat >= zero + 0x10000 {
        0xFFFF
    } else {
        (smallfloat - zero) as u16
    }
}

/// C++: `RankFeaturesAnalyzer` (`rank_features_analyzer_impl.cpp`).
///
/// Emits every field of every object in a JSON array. The numeric value is
/// encoded into `Term::payload` exactly as the reference does.
#[derive(Debug, Clone, Copy, Default)]
pub struct RankFeaturesAnalyzer;

impl RankFeaturesAnalyzer {
    pub fn analyze(&self, input: &str) -> Vec<Term> {
        use expression::json::Json;

        let Some(Json::Arr(items)) = expression::json::parse(input) else {
            return Vec::new();
        };
        let mut out = Vec::new();
        let mut offset = 0u32;
        for item in items {
            let Json::Obj(fields) = item else {
                continue;
            };
            for (key, value) in fields {
                let value = match value {
                    Json::Int(value) => value as f32,
                    Json::Float(value) => value as f32,
                    _ => continue,
                };
                out.push(Term {
                    text: key,
                    word_offset: offset,
                    char_offset: 0,
                    end_offset: 0,
                    is_index: false,
                    payload: float122_to_uint16(value),
                });
                offset += 1;
            }
        }
        out
    }
}

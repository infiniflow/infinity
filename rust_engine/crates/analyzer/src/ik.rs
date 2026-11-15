use super::*;

/// C++: `CharacterUtil::Regularize(input, true)` — ideographic space and
/// full-width ASCII are normalised, then ASCII upper case is lowered.
pub(crate) fn ik_regularize(c: char) -> char {
    let code = c as u32;
    if code == 12288 {
        ' '
    } else if code > 65280 && code < 65375 {
        char::from_u32(code - 65248).unwrap_or(c)
    } else if c.is_ascii_uppercase() {
        c.to_ascii_lowercase()
    } else {
        c
    }
}

/// C++: `Dictionary::ParseProperties` for a single `<entry>` element.
pub(crate) fn ik_xml_entry(content: &str, key: &str) -> Option<String> {
    let needle = format!("key=\"{key}\">");
    let start = content.find(&needle)? + needle.len();
    let end = content[start..].find("</entry>")? + start;
    Some(content[start..end].trim().to_string())
}

/// C++: `GetExtDictionarys` / `GetExtStopWordDictionarys` — `;`-separated paths.
pub(crate) fn ik_paths(value: Option<String>) -> Vec<String> {
    value
        .map(|value| {
            value
                .split(';')
                .filter(|path| !path.is_empty())
                .map(str::to_string)
                .collect()
        })
        .unwrap_or_default()
}

/// C++: `Dictionary::WalkFileTree`.
pub(crate) fn ik_collect_files(path: &Path) -> Vec<std::path::PathBuf> {
    let mut files = Vec::new();
    ik_collect_files_into(path, &mut files);
    files
}

pub(crate) fn ik_collect_files_into(path: &Path, files: &mut Vec<std::path::PathBuf>) {
    if path.is_file() {
        files.push(path.to_path_buf());
    } else if path.is_dir() {
        if let Ok(entries) = std::fs::read_dir(path) {
            for entry in entries.flatten() {
                ik_collect_files_into(&entry.path(), files);
            }
        }
    }
}

/// C++: `IKAnalyzer` (`ik/ik_analyzer.cppm`).
#[derive(Clone)]
pub struct IKAnalyzer {
    pub(crate) dict: Arc<IkDict>,
    pub(crate) stopwords: Arc<HashSet<String>>,
    pub(crate) fine_grained: bool,
}

impl IKAnalyzer {
    /// C++: `IKAnalyzer::Load` — `Dictionary::Load` under `<resource>/ik`.
    pub fn load(resource_dir: impl AsRef<Path>) -> Result<Self, String> {
        let (dict, stopwords) = IkDict::load(&resource_dir.as_ref().join("ik"))?;
        Ok(Self {
            dict: Arc::new(dict),
            stopwords: Arc::new(stopwords),
            fine_grained: false,
        })
    }

    /// C++: `IKAnalyzer::SetFineGrained` — `ik_smart = !fine_grained`.
    pub fn set_fine_grained(&mut self, fine_grained: bool) {
        self.fine_grained = fine_grained;
    }

    /// C++: `IKAnalyzer::AnalyzeImpl` — stop words are filtered, tokens are
    /// regularized (lower-cased) and their offsets are token ordinals.
    pub fn analyze(&self, input: &str) -> Vec<Term> {
        let mut terms = Vec::new();
        let mut offset = 0u32;
        for token in self.dict.tokenize(input, !self.fine_grained) {
            if token.is_empty() || self.stopwords.contains(&token) {
                continue;
            }
            terms.push(Term {
                text: token,
                word_offset: offset,
                char_offset: 0,
                end_offset: 0,
                is_index: false,
                payload: 0,
            });
            offset += 1;
        }
        terms
    }
}

/// `src/unit_test/common/analyzer/ik_analyzer_ut.cpp` — `test1`.
///
/// The reference loads the IK dictionary and prints the token stream; the
/// only active query is kept here and the port asserts that analysis succeeds
/// with non-empty terms.
#[cfg(test)]
mod ik_analyzer_tests {
    use super::*;

    const RESOURCE_DIR: &str = "/usr/share/infinity/resource";

    #[test]
    fn test1() {
        let resource = Path::new(RESOURCE_DIR);
        if !resource.is_dir() {
            panic!("Resource directory doesn't exist: {}", resource.display());
        }
        let analyzer = IKAnalyzer::load(resource)
            .unwrap_or_else(|status| panic!("Failed to load IKAnalyzer: {status}"));
        // C++: the first five candidates are commented out; only this one runs.
        let queries = [r#"蓝月亮如何在外资夹击中生存,那是全宇宙最有意思的"#];
        for query in queries {
            let terms = analyzer.analyze(query);
            assert!(!terms.is_empty(), "no terms for input: {query}");
            assert!(
                terms.iter().all(|term| !term.text.is_empty()),
                "empty term for input: {query}"
            );
        }
    }
}

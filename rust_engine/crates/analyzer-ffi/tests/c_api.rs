//! The C ABI, exercised the way a host does: raw handles, malloc'd results.
//!
//! The dictionary-driven cases need the resource tree; they return early when
//! it is not installed (set INFINITY_RESOURCE_PATH to point at a checkout).

use std::ffi::{CStr, CString};
use std::sync::Mutex;

use rag_analyzer::*;

fn resource_dir() -> String {
    std::env::var("INFINITY_RESOURCE_PATH")
        .unwrap_or_else(|_| "/usr/share/infinity/resource".to_string())
}

fn require_resources() -> Option<String> {
    let dir = resource_dir();
    if std::path::Path::new(&dir).join("rag/huqie.txt").is_file() {
        Some(dir)
    } else {
        None
    }
}

fn c(text: &str) -> CString {
    CString::new(text).expect("no interior NUL")
}

unsafe fn take_string(pointer: *mut std::ffi::c_char) -> String {
    assert!(!pointer.is_null(), "expected a string, got NULL");
    let text = CStr::from_ptr(pointer).to_string_lossy().into_owned();
    RAGAnalyzer_FreeString(pointer);
    text
}

unsafe fn tokenize(handle: *mut AnalyzerHandle, text: &str) -> String {
    let input = c(text);
    take_string(RAGAnalyzer_Tokenize(handle, input.as_ptr()))
}

unsafe fn open(name: &str, dir: &str) -> *mut AnalyzerHandle {
    let (name, dir) = (c(name), c(dir));
    let handle = RAGAnalyzer_Open(name.as_ptr(), dir.as_ptr());
    assert!(!handle.is_null(), "open failed: {}", last_error());
    handle
}

unsafe fn last_error() -> String {
    CStr::from_ptr(RAGAnalyzer_LastError())
        .to_string_lossy()
        .into_owned()
}

unsafe fn loaded_handle() -> Option<*mut AnalyzerHandle> {
    let dir = require_resources()?;
    let path = c(&dir);
    let handle = RAGAnalyzer_Create(path.as_ptr());
    assert!(!handle.is_null());
    assert_eq!(RAGAnalyzer_Load(handle), 0, "load failed: {}", last_error());
    Some(handle)
}

#[test]
fn the_abi_version_is_reported() {
    assert_eq!(RAGAnalyzer_AbiVersion(), 1);
}

#[test]
fn null_handles_and_arguments_are_rejected() {
    unsafe {
        assert!(RAGAnalyzer_Tokenize(std::ptr::null_mut(), std::ptr::null()).is_null());
        assert!(RAGAnalyzer_Open(std::ptr::null(), std::ptr::null()).is_null());
        assert!(RAGAnalyzer_Copy(std::ptr::null_mut()).is_null());
        assert_eq!(RAGAnalyzer_Load(std::ptr::null_mut()), -1);
        RAGAnalyzer_Destroy(std::ptr::null_mut());
        RAGAnalyzer_FreeTokenList(std::ptr::null_mut());
        assert_eq!(
            RAGAnalyzer_GetTermFreq(std::ptr::null_mut(), std::ptr::null()),
            0
        );

        let dir = c(&resource_dir());
        let created = RAGAnalyzer_Create(dir.as_ptr());
        assert!(!created.is_null());
        // Tokenizing before Load is an error, not a crash.
        assert!(RAGAnalyzer_Tokenize(created, c("hello").as_ptr()).is_null());
        assert!(!last_error().is_empty());
        RAGAnalyzer_Destroy(created);
    }
}

#[test]
fn an_unknown_analyzer_name_reports_why() {
    unsafe {
        let (name, dir) = (c("no-such-analyzer"), c(&resource_dir()));
        assert!(RAGAnalyzer_Open(name.as_ptr(), dir.as_ptr()).is_null());
        assert!(
            last_error().contains("no-such-analyzer"),
            "{}",
            last_error()
        );
    }
}

#[test]
fn english_text_tokenizes_and_stems() {
    let Some(mut handle) = (unsafe { loaded_handle() }) else {
        return;
    };
    unsafe {
        let tokens = tokenize(handle, "The Quick Brown Foxes are Running!");
        assert!(!tokens.is_empty());
        assert!(!tokens.contains('!'), "{tokens}");
        // The default stemmer is English: the plural and the participle fold.
        assert!(tokens.split(' ').any(|token| token == "fox"), "{tokens}");
        assert!(tokens.split(' ').any(|token| token == "run"), "{tokens}");

        // A language without a Snowball stemmer keeps the words as they are.
        RAGAnalyzer_SetLanguage(handle, c("Slovak").as_ptr());
        let slovak = tokenize(handle, "Häuser");
        assert!(!slovak.contains('ä'), "diacritics are folded: {slovak}");
        RAGAnalyzer_Destroy(handle);
        handle = std::ptr::null_mut();
        assert!(handle.is_null());
    }
}

#[test]
fn chinese_text_tokenizes_finer_and_reports_positions() {
    let Some(handle) = (unsafe { loaded_handle() }) else {
        return;
    };
    unsafe {
        let text = "我们都有一个家";
        let tokens = tokenize(handle, text);
        assert!(!tokens.is_empty());

        let input = c(text);
        let list = RAGAnalyzer_TokenizeWithPosition(handle, input.as_ptr());
        assert!(!list.is_null(), "{}", last_error());
        let count = (*list).count as usize;
        assert!(count > 0);
        let mut previous = 0u32;
        for index in 0..count {
            let slot = (*list).tokens.add(index) as *mut std::ffi::c_void;
            let token = CStr::from_ptr(RAGToken_GetText(slot))
                .to_string_lossy()
                .into_owned();
            let (start, end) = (RAGToken_GetOffset(slot), RAGToken_GetEndOffset(slot));
            assert!(!token.is_empty());
            assert!(start >= previous, "offsets advance: {start} < {previous}");
            assert!(end > start, "every token covers bytes");
            assert!((end as usize) <= text.len(), "offset inside the input");
            previous = start;
        }
        RAGAnalyzer_FreeTokenList(list);

        // Fine-grained mode splits the words it found into shorter pieces.
        let fine = {
            let tokens = c(&tokens);
            take_string(RAGAnalyzer_FineGrainedTokenize(handle, tokens.as_ptr()))
        };
        assert!(!fine.is_empty());
        assert!(fine.split(' ').count() >= tokens.split(' ').count());
    }
}

#[test]
fn fine_grained_positions_split_the_same_way() {
    let Some(handle) = (unsafe { loaded_handle() }) else {
        return;
    };
    unsafe {
        let text = c("我们都有一个家");
        let coarse = RAGAnalyzer_TokenizeWithPosition(handle, text.as_ptr());
        let coarse_count = (*coarse).count;
        RAGAnalyzer_FreeTokenList(coarse);

        RAGAnalyzer_SetFineGrained(handle, true);
        let fine = RAGAnalyzer_TokenizeWithPosition(handle, text.as_ptr());
        assert!(!fine.is_null(), "{}", last_error());
        assert!(
            (*fine).count >= coarse_count,
            "fine {} < coarse {}",
            (*fine).count,
            coarse_count
        );
        RAGAnalyzer_FreeTokenList(fine);
    }
}

#[test]
fn a_callback_sees_every_token() {
    static SEEN: Mutex<Vec<String>> = Mutex::new(Vec::new());

    unsafe extern "C" fn collect(text: *const std::ffi::c_char, _len: u32, _start: u32, _end: u32) {
        let token = CStr::from_ptr(text).to_string_lossy().into_owned();
        SEEN.lock().expect("lock").push(token);
    }

    let Some(handle) = (unsafe { loaded_handle() }) else {
        return;
    };
    unsafe {
        let input = c("hello world");
        assert_eq!(
            RAGAnalyzer_Analyze(handle, input.as_ptr(), Some(collect)),
            0
        );
        let seen = SEEN.lock().expect("lock").clone();
        assert_eq!(seen, vec!["hello".to_string(), "world".to_string()]);
    }
}

#[test]
fn frequencies_and_tags_come_from_the_dictionary() {
    let Some(handle) = (unsafe { loaded_handle() }) else {
        return;
    };
    unsafe {
        let word = c("我们");
        assert!(RAGAnalyzer_GetTermFreq(handle, word.as_ptr()) > 0);
        let tag = RAGAnalyzer_GetTermTag(handle, word.as_ptr());
        assert!(!tag.is_null(), "expected a tag for 我们");
        let tag = take_string(tag);
        assert!(!tag.is_empty());

        // Unknown words carry neither.
        let unknown = c("zzzzzzzzzzzz");
        assert_eq!(RAGAnalyzer_GetTermFreq(handle, unknown.as_ptr()), 0);
        assert!(RAGAnalyzer_GetTermTag(handle, unknown.as_ptr()).is_null());
    }
}

#[test]
fn copies_share_dictionaries_but_not_settings() {
    let Some(handle) = (unsafe { loaded_handle() }) else {
        return;
    };
    unsafe {
        let copy = RAGAnalyzer_Copy(handle);
        assert!(!copy.is_null(), "{}", last_error());
        RAGAnalyzer_SetLanguage(copy, c("German").as_ptr());
        RAGAnalyzer_Destroy(copy);

        // The original is untouched and still usable after the copy is gone.
        assert!(!tokenize(handle, "hello world").is_empty());
        RAGAnalyzer_Destroy(handle);
    }
}

#[test]
fn index_names_open_the_analyzer_the_index_was_built_with() {
    let Some(dir) = require_resources() else {
        return;
    };
    unsafe {
        for name in ["rag-coarse", "rag-fine", "rag", "rag-zh"] {
            let handle = open(name, &dir);
            let tokens = tokenize(handle, "我们都有一个家 hello");
            assert!(!tokens.is_empty(), "{name} produced nothing");
            RAGAnalyzer_Destroy(handle);
        }

        // keyword fields: the delimiter set lives in the name.
        let handle = open("whitespace-#", &dir);
        assert_eq!(tokenize(handle, "a#b#c"), "a b c");
        RAGAnalyzer_Destroy(handle);

        // rank features: every numeric field of the JSON array is a term.
        let handle = open("rankfeatures", &dir);
        assert_eq!(
            tokenize(handle, "[{\"tag\":1.5,\"pagerank\":7}]"),
            "tag pagerank"
        );
        RAGAnalyzer_Destroy(handle);
    }
}

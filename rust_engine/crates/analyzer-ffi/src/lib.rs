//! The C ABI a host process links to tokenize exactly like the engine does.
//!
//! It mirrors the rag_tokenizer_c_api contract RAGFlow's Go services already
//! link (internal/binding/cpp/rag_analyzer_c_api.h): same symbol names, same
//! struct layouts, same ownership - every returned string comes from malloc
//! and is released with free(), and a RAGTokenList is released with
//! RAGAnalyzer_FreeTokenList. A caller can therefore swap the C++ static
//! library for this one without touching its code.
//!
//! Two ways in:
//!
//! * RAGAnalyzer_Create + RAGAnalyzer_Load + the setters, which is what the Go
//!   analyzer pool does: one base handle, RAGAnalyzer_Copy per pooled instance
//!   (the dictionaries are shared, the settings are not), and SetLanguage per
//!   request;
//! * RAGAnalyzer_Open, which takes an analyzer name from
//!   conf/infinity_mapping.json (rag-coarse, rag-fine, whitespace-#,
//!   rankfeatures, ...) so the client-side token stream is the one the
//!   matching full-text index was built with.
//!
//! Every entry point catches panics: a panic must never unwind across the FFI
//! boundary.

#![allow(non_camel_case_types)]

use std::ffi::{c_char, c_int, c_uint, CStr, CString};
use std::panic::{catch_unwind, AssertUnwindSafe};
use std::path::PathBuf;
use std::ptr;

use analyzer::{Analyzer, AnalyzerPool, RAGAnalyzer};

/// Bumped whenever a struct layout or an ownership rule changes.
pub const ABI_VERSION: u32 = 1;

thread_local! {
    static LAST_ERROR: std::cell::RefCell<CString> =
        std::cell::RefCell::new(CString::new("").expect("empty"));
}

fn clear_error() {
    LAST_ERROR.with(|slot| {
        *slot.borrow_mut() = CString::new("").expect("empty");
    });
}

fn set_error(message: impl AsRef<str>) {
    let text = message.as_ref().replace('\0', " ");
    LAST_ERROR.with(|slot| {
        *slot.borrow_mut() =
            CString::new(text).unwrap_or_else(|_| CString::new("error").expect("error"));
    });
}

/// Runs body, turning a panic into fallback plus a LastError message.
fn guard<T>(fallback: T, body: impl FnOnce() -> T) -> T {
    match catch_unwind(AssertUnwindSafe(body)) {
        Ok(value) => value,
        Err(_) => {
            set_error("panic in the analyzer");
            fallback
        }
    }
}

/// A C string handed to the caller: allocated with malloc, freed with free.
fn export_string(text: &str) -> *mut c_char {
    let bytes = text.as_bytes();
    // SAFETY: the allocation is checked, sized to the string plus its NUL.
    unsafe {
        let raw = libc::malloc(bytes.len() + 1) as *mut u8;
        if raw.is_null() {
            set_error("out of memory");
            return ptr::null_mut();
        }
        ptr::copy_nonoverlapping(bytes.as_ptr(), raw, bytes.len());
        *raw.add(bytes.len()) = 0;
        raw as *mut c_char
    }
}

/// Borrows a NUL-terminated UTF-8 argument; None on NULL or invalid UTF-8.
///
/// # Safety
/// text must be NULL or point at a NUL-terminated string.
unsafe fn input_text<'a>(text: *const c_char) -> Option<&'a str> {
    if text.is_null() {
        return None;
    }
    match CStr::from_ptr(text).to_str() {
        Ok(value) => Some(value),
        Err(_) => {
            set_error("input is not valid UTF-8");
            None
        }
    }
}

/// One token with its byte offsets in the input.
#[repr(C)]
pub struct RAGTokenWithPosition {
    /// malloc'd token text; released by RAGAnalyzer_FreeTokenList.
    pub text: *mut c_char,
    pub offset: u32,
    pub end_offset: u32,
}

/// An array of tokens, malloc'd like the reference's C API.
#[repr(C)]
pub struct RAGTokenList {
    pub tokens: *mut RAGTokenWithPosition,
    pub count: u32,
}

/// What a handle tokenizes with.
enum Backend {
    /// A RAGAnalyzer_Create handle whose Load has not run yet.
    Unloaded(PathBuf),
    /// The RAG family: the analyzers rag / rag-<lang> / rag-fine name.
    Rag(RAGAnalyzer),
    /// Any other family from the name grammar (whitespace-#, standard-en,
    /// ngram-3, rankfeatures, ...), used through Analyzer::analyze.
    Named(Box<Analyzer>),
}

/// The opaque handle behind RAGAnalyzerHandle.
pub struct AnalyzerHandle {
    backend: Backend,
    fine_grained: bool,
    enable_position: bool,
    language: Option<String>,
}

impl AnalyzerHandle {
    fn new(backend: Backend) -> Box<Self> {
        Box::new(Self {
            backend,
            fine_grained: false,
            enable_position: false,
            language: None,
        })
    }

    /// Applies the settings this handle carries to the loaded RAG analyzer.
    fn apply_settings(&mut self) {
        if let Backend::Rag(analyzer) = &mut self.backend {
            analyzer.set_fine_grained(self.fine_grained);
            analyzer.set_enable_position(self.enable_position);
            if let Some(language) = &self.language {
                analyzer.set_language(language);
            }
        }
    }

    fn rag(&self) -> Option<&RAGAnalyzer> {
        match &self.backend {
            Backend::Rag(analyzer) => Some(analyzer),
            _ => None,
        }
    }

    /// The space-joined token stream.
    fn tokenize(&self, text: &str) -> Result<String, String> {
        match &self.backend {
            Backend::Rag(analyzer) => Ok(analyzer.tokenize(text)),
            Backend::Named(analyzer) => Ok(join(
                analyzer.analyze(text).iter().map(|term| term.text.as_str()),
            )),
            Backend::Unloaded(_) => Err("analyzer is not loaded".to_string()),
        }
    }

    /// Tokens plus their byte offsets; the RAG backend applies the handle's
    /// fine-grained mode the way RAGAnalyzer_TokenizeWithPosition does.
    fn tokens_with_position(&self, text: &str) -> Result<Vec<(String, u32, u32)>, String> {
        match &self.backend {
            Backend::Rag(analyzer) => {
                let (tokens, positions) = analyzer.tokenize_with_position(text);
                let (tokens, positions) = if self.fine_grained {
                    analyzer.fine_grained_tokenize_with_position(&tokens.join(" "), &positions)
                } else {
                    (tokens, positions)
                };
                Ok(tokens
                    .into_iter()
                    .zip(positions)
                    .map(|(token, (start, end))| (token, start, end))
                    .collect())
            }
            Backend::Named(analyzer) => Ok(analyzer
                .analyze(text)
                .into_iter()
                .map(|term| (term.text, term.word_offset, term.end_offset))
                .collect()),
            Backend::Unloaded(_) => Err("analyzer is not loaded".to_string()),
        }
    }
}

fn join<'a>(texts: impl Iterator<Item = &'a str>) -> String {
    let mut out = String::new();
    for text in texts {
        if !out.is_empty() {
            out.push(' ');
        }
        out.push_str(text);
    }
    out
}

/// # Safety
/// handle must come from RAGAnalyzer_Create/Open/Copy.
unsafe fn handle_mut<'a>(handle: *mut AnalyzerHandle) -> Option<&'a mut AnalyzerHandle> {
    handle.as_mut()
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

/// Creates an analyzer rooted at path (the directory holding rag/, wordnet/
/// and opencc/); dictionaries load on RAGAnalyzer_Load.
///
/// # Safety
/// path must be NULL or a NUL-terminated string.
#[no_mangle]
pub unsafe extern "C" fn RAGAnalyzer_Create(path: *const c_char) -> *mut AnalyzerHandle {
    clear_error();
    guard(ptr::null_mut(), || {
        let Some(path) = input_text(path) else {
            set_error("RAGAnalyzer_Create: path is NULL or not UTF-8");
            return ptr::null_mut();
        };
        Box::into_raw(AnalyzerHandle::new(Backend::Unloaded(PathBuf::from(path))))
    })
}

/// Opens the analyzer an index name selects (rag-coarse, rag-fine,
/// whitespace-#, rankfeatures, standard-en, ngram-3, ...), loading its
/// dictionaries from resource_dir.
///
/// # Safety
/// Both arguments must be NULL or NUL-terminated strings.
#[no_mangle]
pub unsafe extern "C" fn RAGAnalyzer_Open(
    name: *const c_char,
    resource_dir: *const c_char,
) -> *mut AnalyzerHandle {
    clear_error();
    guard(ptr::null_mut(), || {
        let (Some(name), Some(resource)) = (input_text(name), input_text(resource_dir)) else {
            set_error("RAGAnalyzer_Open: name/resource_dir is NULL or not UTF-8");
            return ptr::null_mut();
        };
        let pool = AnalyzerPool::with_resource_path(resource);
        let analyzer = match pool.get_analyzer(name) {
            Ok(analyzer) => analyzer,
            Err(error) => {
                set_error(error);
                return ptr::null_mut();
            }
        };
        let mut handle = AnalyzerHandle::new(match analyzer {
            Analyzer::Rag(analyzer) => Backend::Rag(analyzer),
            other => Backend::Named(Box::new(other)),
        });
        if let Some(rag) = handle.rag() {
            let (fine_grained, enable_position) = (rag.fine_grained(), rag.enable_position());
            handle.fine_grained = fine_grained;
            handle.enable_position = enable_position;
        }
        Box::into_raw(handle)
    })
}

/// Releases a handle. Passing NULL is a no-op.
///
/// # Safety
/// handle must come from this library and must not be used afterwards.
#[no_mangle]
pub unsafe extern "C" fn RAGAnalyzer_Destroy(handle: *mut AnalyzerHandle) {
    if handle.is_null() {
        return;
    }
    guard((), || drop(Box::from_raw(handle)));
}

/// Loads the dictionaries. Returns 0 on success, negative on failure.
///
/// # Safety
/// handle must be a live handle.
#[no_mangle]
pub unsafe extern "C" fn RAGAnalyzer_Load(handle: *mut AnalyzerHandle) -> c_int {
    clear_error();
    guard(-1, || {
        let Some(handle) = handle_mut(handle) else {
            set_error("RAGAnalyzer_Load: handle is NULL");
            return -1;
        };
        if let Backend::Unloaded(path) = &handle.backend {
            match RAGAnalyzer::load(path) {
                Ok(analyzer) => {
                    handle.backend = Backend::Rag(analyzer);
                    handle.apply_settings();
                }
                Err(error) => {
                    set_error(&error);
                    return -1;
                }
            }
        }
        0
    })
}

/// Copies a handle: the dictionaries are shared, the settings are the copy's
/// own (that is what the Go pool does to get one analyzer per goroutine).
///
/// # Safety
/// handle must be a live handle.
#[no_mangle]
pub unsafe extern "C" fn RAGAnalyzer_Copy(handle: *mut AnalyzerHandle) -> *mut AnalyzerHandle {
    clear_error();
    guard(ptr::null_mut(), || {
        let Some(source) = handle.as_ref() else {
            set_error("RAGAnalyzer_Copy: handle is NULL");
            return ptr::null_mut();
        };
        let backend = match &source.backend {
            Backend::Unloaded(path) => Backend::Unloaded(path.clone()),
            Backend::Rag(analyzer) => Backend::Rag(analyzer.clone()),
            // The non-RAG families are not Clone and do not carry the name they
            // were opened with; a host that needs several of them opens one per
            // worker, the way it did with the C++ library.
            Backend::Named(_) => {
                set_error("RAGAnalyzer_Copy: only RAG handles can be copied");
                return ptr::null_mut();
            }
        };
        Box::into_raw(Box::new(AnalyzerHandle {
            backend,
            fine_grained: source.fine_grained,
            enable_position: source.enable_position,
            language: source.language.clone(),
        }))
    })
}

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

/// # Safety
/// handle must be a live handle.
#[no_mangle]
pub unsafe extern "C" fn RAGAnalyzer_SetFineGrained(
    handle: *mut AnalyzerHandle,
    fine_grained: bool,
) {
    guard((), || {
        if let Some(handle) = handle_mut(handle) {
            handle.fine_grained = fine_grained;
            if let Backend::Rag(analyzer) = &mut handle.backend {
                analyzer.set_fine_grained(fine_grained);
            }
        }
    });
}

/// # Safety
/// handle must be a live handle.
#[no_mangle]
pub unsafe extern "C" fn RAGAnalyzer_SetEnablePosition(
    handle: *mut AnalyzerHandle,
    enable_position: bool,
) {
    guard((), || {
        if let Some(handle) = handle_mut(handle) {
            handle.enable_position = enable_position;
            if let Backend::Rag(analyzer) = &mut handle.backend {
                analyzer.set_enable_position(enable_position);
            }
        }
    });
}

/// Sets the stemming language (English, Dutch, ...) for the RAG analyzer.
///
/// # Safety
/// Both arguments must be NULL or NUL-terminated strings.
#[no_mangle]
pub unsafe extern "C" fn RAGAnalyzer_SetLanguage(
    handle: *mut AnalyzerHandle,
    language: *const c_char,
) {
    clear_error();
    guard((), || {
        let Some(handle) = handle_mut(handle) else {
            return;
        };
        let Some(language) = input_text(language) else {
            set_error("RAGAnalyzer_SetLanguage: language is NULL or not UTF-8");
            return;
        };
        handle.language = Some(language.to_string());
        if let Backend::Rag(analyzer) = &mut handle.backend {
            analyzer.set_language(language);
        }
    });
}

// ---------------------------------------------------------------------------
// Tokenizing
// ---------------------------------------------------------------------------

/// Calls callback once per token. Returns 0 on success, negative on failure.
///
/// # Safety
/// handle must be a live handle, text a NUL-terminated string and callback a
/// valid function pointer.
#[no_mangle]
pub unsafe extern "C" fn RAGAnalyzer_Analyze(
    handle: *mut AnalyzerHandle,
    text: *const c_char,
    callback: Option<unsafe extern "C" fn(*const c_char, u32, u32, u32)>,
) -> c_int {
    clear_error();
    guard(-1, || {
        let (Some(handle), Some(callback)) = (handle.as_ref(), callback) else {
            set_error("RAGAnalyzer_Analyze: handle/callback is NULL");
            return -1;
        };
        let Some(text) = input_text(text) else {
            set_error("RAGAnalyzer_Analyze: text is NULL or not UTF-8");
            return -1;
        };
        let terms = match &handle.backend {
            Backend::Rag(analyzer) => analyzer.analyze(text),
            Backend::Named(analyzer) => analyzer.analyze(text),
            Backend::Unloaded(_) => {
                set_error("analyzer is not loaded");
                return -1;
            }
        };
        for term in &terms {
            // The C++ callback hands over c_str(): the token must be
            // NUL-terminated, or a Go host reading it with C.GoString runs
            // past the end.
            let text = CString::new(term.text.replace('\0', ""))
                .unwrap_or_else(|_| CString::new("").expect("empty"));
            callback(
                text.as_ptr(),
                term.text.len() as u32,
                term.word_offset,
                term.end_offset,
            );
        }
        0
    })
}

/// Returns the space-joined token stream, malloc'd; free it with free().
///
/// # Safety
/// handle must be a live handle and text a NUL-terminated string.
#[no_mangle]
pub unsafe extern "C" fn RAGAnalyzer_Tokenize(
    handle: *mut AnalyzerHandle,
    text: *const c_char,
) -> *mut c_char {
    clear_error();
    guard(ptr::null_mut(), || {
        let Some(handle) = handle.as_ref() else {
            set_error("RAGAnalyzer_Tokenize: handle is NULL");
            return ptr::null_mut();
        };
        let Some(text) = input_text(text) else {
            set_error("RAGAnalyzer_Tokenize: text is NULL or not UTF-8");
            return ptr::null_mut();
        };
        match handle.tokenize(text) {
            Ok(result) => export_string(&result),
            Err(error) => {
                set_error(error);
                ptr::null_mut()
            }
        }
    })
}

/// Returns the tokens with their byte offsets; free with
/// RAGAnalyzer_FreeTokenList.
///
/// # Safety
/// handle must be a live handle and text a NUL-terminated string.
#[no_mangle]
pub unsafe extern "C" fn RAGAnalyzer_TokenizeWithPosition(
    handle: *mut AnalyzerHandle,
    text: *const c_char,
) -> *mut RAGTokenList {
    clear_error();
    guard(ptr::null_mut(), || {
        let Some(handle) = handle.as_ref() else {
            set_error("RAGAnalyzer_TokenizeWithPosition: handle is NULL");
            return ptr::null_mut();
        };
        let Some(text) = input_text(text) else {
            set_error("RAGAnalyzer_TokenizeWithPosition: text is NULL or not UTF-8");
            return ptr::null_mut();
        };
        let tokens = match handle.tokens_with_position(text) {
            Ok(tokens) => tokens,
            Err(error) => {
                set_error(error);
                return ptr::null_mut();
            }
        };

        let list = libc::malloc(std::mem::size_of::<RAGTokenList>()) as *mut RAGTokenList;
        if list.is_null() {
            set_error("out of memory");
            return ptr::null_mut();
        }
        (*list).tokens = ptr::null_mut();
        (*list).count = tokens.len() as u32;
        if tokens.is_empty() {
            return list;
        }
        let array = libc::malloc(std::mem::size_of::<RAGTokenWithPosition>() * tokens.len())
            as *mut RAGTokenWithPosition;
        if array.is_null() {
            libc::free(list as *mut libc::c_void);
            set_error("out of memory");
            return ptr::null_mut();
        }
        for (index, (token, start, end)) in tokens.iter().enumerate() {
            let slot = array.add(index);
            (*slot).text = export_string(token);
            (*slot).offset = *start;
            (*slot).end_offset = *end;
        }
        (*list).tokens = array;
        list
    })
}

/// Frees a list from RAGAnalyzer_TokenizeWithPosition; NULL is a no-op.
///
/// # Safety
/// list must come from RAGAnalyzer_TokenizeWithPosition and be freed once.
#[no_mangle]
pub unsafe extern "C" fn RAGAnalyzer_FreeTokenList(list: *mut RAGTokenList) {
    if list.is_null() {
        return;
    }
    guard((), || {
        let tokens = (*list).tokens;
        if !tokens.is_null() {
            for index in 0..(*list).count as usize {
                let text = (*tokens.add(index)).text;
                if !text.is_null() {
                    libc::free(text as *mut libc::c_void);
                }
            }
            libc::free(tokens as *mut libc::c_void);
        }
        libc::free(list as *mut libc::c_void);
    });
}

/// # Safety
/// token must point at an element of a live RAGTokenList.
#[no_mangle]
pub unsafe extern "C" fn RAGToken_GetText(token: *mut libc::c_void) -> *const c_char {
    if token.is_null() {
        return ptr::null();
    }
    (*(token as *mut RAGTokenWithPosition)).text
}

/// # Safety
/// token must point at an element of a live RAGTokenList.
#[no_mangle]
pub unsafe extern "C" fn RAGToken_GetOffset(token: *mut libc::c_void) -> u32 {
    if token.is_null() {
        return 0;
    }
    (*(token as *mut RAGTokenWithPosition)).offset
}

/// # Safety
/// token must point at an element of a live RAGTokenList.
#[no_mangle]
pub unsafe extern "C" fn RAGToken_GetEndOffset(token: *mut libc::c_void) -> u32 {
    if token.is_null() {
        return 0;
    }
    (*(token as *mut RAGTokenWithPosition)).end_offset
}

/// Splits space-separated tokens into fine-grained ones (CJK words further
/// segmented); returns a malloc'd space-joined string.
///
/// # Safety
/// handle must be a live handle and tokens a NUL-terminated string.
#[no_mangle]
pub unsafe extern "C" fn RAGAnalyzer_FineGrainedTokenize(
    handle: *mut AnalyzerHandle,
    tokens: *const c_char,
) -> *mut c_char {
    clear_error();
    guard(ptr::null_mut(), || {
        let Some(handle) = handle.as_ref() else {
            set_error("RAGAnalyzer_FineGrainedTokenize: handle is NULL");
            return ptr::null_mut();
        };
        let Some(tokens) = input_text(tokens) else {
            set_error("RAGAnalyzer_FineGrainedTokenize: tokens is NULL or not UTF-8");
            return ptr::null_mut();
        };
        match handle.rag() {
            Some(analyzer) => export_string(&analyzer.fine_grained_tokenize(tokens).join(" ")),
            None => {
                set_error("RAGAnalyzer_FineGrainedTokenize: only the rag family splits finer");
                ptr::null_mut()
            }
        }
    })
}

/// The dictionary frequency of term (Python rag_tokenizer.freq); 0 when the
/// word is unknown or the handle is not a RAG analyzer.
///
/// # Safety
/// handle must be a live handle and term a NUL-terminated string.
#[no_mangle]
pub unsafe extern "C" fn RAGAnalyzer_GetTermFreq(
    handle: *mut AnalyzerHandle,
    term: *const c_char,
) -> i32 {
    guard(0, || {
        let (Some(handle), Some(term)) = (handle.as_ref(), input_text(term)) else {
            return 0;
        };
        handle.rag().map_or(0, |analyzer| analyzer.freq(term))
    })
}

/// The POS tag of term (Python rag_tokenizer.tag), malloc'd; NULL when the
/// word is unknown or carries no tag.
///
/// # Safety
/// handle must be a live handle and term a NUL-terminated string.
#[no_mangle]
pub unsafe extern "C" fn RAGAnalyzer_GetTermTag(
    handle: *mut AnalyzerHandle,
    term: *const c_char,
) -> *mut c_char {
    clear_error();
    guard(ptr::null_mut(), || {
        let (Some(handle), Some(term)) = (handle.as_ref(), input_text(term)) else {
            return ptr::null_mut();
        };
        match handle.rag().and_then(|analyzer| analyzer.tag(term)) {
            Some(tag) => export_string(&tag),
            None => ptr::null_mut(),
        }
    })
}

/// The C ABI version of this library.
#[no_mangle]
pub extern "C" fn RAGAnalyzer_AbiVersion() -> c_uint {
    ABI_VERSION
}

/// The error of the last failed call on this thread, or "".
#[no_mangle]
pub extern "C" fn RAGAnalyzer_LastError() -> *const c_char {
    LAST_ERROR.with(|slot| slot.borrow().as_ptr())
}

/// Frees a string this library returned, for hosts that would rather not call
/// free themselves.
///
/// # Safety
/// text must come from this library and be freed once.
#[no_mangle]
pub unsafe extern "C" fn RAGAnalyzer_FreeString(text: *mut c_char) {
    if !text.is_null() {
        libc::free(text as *mut libc::c_void);
    }
}

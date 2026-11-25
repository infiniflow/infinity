//! C ABI over `model2vec-rs`, for the Go load generator to call through cgo.
//!
//! Why a C ABI rather than a subprocess: the generator embeds 8.34M rows for the
//! L scenario, and both of the obvious alternatives are wrong here. A
//! subprocess-per-batch pays process startup and re-loads the 125 MB model, and
//! `model2vec-rs`'s CLI serialises every vector as JSON text (34 GB for L). An
//! in-process call keeps the model resident, returns the vectors as a flat f32
//! buffer, and lets the Go side own batching, files and concurrency.
//!
//! Contract, from the Go side:
//!
//!   h := m2v_load(dir, dim)            // nil on failure, reason from m2v_error
//!   n := m2v_encode(h, texts, out)     // out must hold len(texts)*dim f32
//!   m2v_free(h)
//!
//! `texts` is one NUL-separated UTF-8 blob; `n` is the number of vectors written
//! (0 on error). Vectors are L2-normalised iff the model config says so, which
//! is the model's own default and what Q1/Q3 assume for cosine distance.
//!
//! Threading: a handle is read-only after load and may be used from several
//! goroutines at once. `model2vec-rs` parallelises inside a call with its own
//! pool, so the Go side should not make encode calls concurrently on one handle
//! for throughput — it should make them large.

use std::ffi::{c_char, c_int, CStr};
use std::panic::{catch_unwind, AssertUnwindSafe};
use std::sync::{Mutex, OnceLock};

use model2vec_rs::model::StaticModel;

/// Last error, per process. Only ever read right after a call returned a failure
/// sentinel, which is why a single global is enough.
static LAST_ERROR: OnceLock<Mutex<String>> = OnceLock::new();

fn set_error(msg: impl Into<String>) {
    let slot = LAST_ERROR.get_or_init(|| Mutex::new(String::new()));
    if let Ok(mut guard) = slot.lock() {
        *guard = msg.into();
    }
}

/// Opaque handle. Boxed on the Rust side, owned by the caller in Go.
pub struct M2vHandle {
    model: StaticModel,
    dim: usize,
}

/// Load a Model2Vec model from a local directory.
///
/// Returns null on failure. The directory is the one holding either
/// `config.json` or `config_sentence_transformers.json` plus
/// `0_StaticEmbedding/` — `StaticModel::from_pretrained` resolves both layouts.
///
/// `expect_dim` is checked against the model's own dimension: a mismatch is an
/// error rather than a silent reshape, because the table column is
/// `q_<dim>_vec` and a wrong width is rejected at import.
#[no_mangle]
pub extern "C" fn m2v_load(path: *const c_char, expect_dim: usize) -> *mut M2vHandle {
    let result = catch_unwind(AssertUnwindSafe(|| -> Result<*mut M2vHandle, String> {
        if path.is_null() {
            return Err("m2v_load: path is null".into());
        }
        let path = unsafe { CStr::from_ptr(path) }
            .to_str()
            .map_err(|e| format!("m2v_load: path is not valid UTF-8: {e}"))?;

        let model = StaticModel::from_pretrained(path, None, None, None)
            .map_err(|e| format!("m2v_load({path}): {e}"))?;
        let dim = model.dim();
        if expect_dim != 0 && dim != expect_dim {
            return Err(format!(
                "m2v_load({path}): model produces {dim}-d vectors but {expect_dim} was requested; \
                 the table column is q_{expect_dim}_vec and the import would be rejected"
            ));
        }
        Ok(Box::into_raw(Box::new(M2vHandle { model, dim })))
    }));

    match result {
        Ok(Ok(h)) => h,
        Ok(Err(msg)) => {
            set_error(msg);
            std::ptr::null_mut()
        }
        Err(_) => {
            set_error("m2v_load: panicked (see stderr)");
            std::ptr::null_mut()
        }
    }
}

/// Embed `count` NUL-separated UTF-8 texts held in `texts[0..texts_len]` into
/// `out`, which must have room for `count * dim` f32 values.
///
/// `texts_len` is explicit because the blob contains NUL separators: a
/// NUL-terminated C string would stop after the first text. (That was the first
/// bug this ABI had — `m2v_encode` reported "blob holds 1 texts but count is 3"
/// for every multi-text batch.)
///
/// Returns the number of vectors written, or 0 on error. `max_length` caps the
/// tokens per text (0 = the model's default); the generator passes 512.
#[no_mangle]
pub extern "C" fn m2v_encode(
    handle: *mut M2vHandle,
    texts: *const u8,
    texts_len: usize,
    count: usize,
    max_length: usize,
    out: *mut f32,
) -> usize {
    let result = catch_unwind(AssertUnwindSafe(|| -> Result<usize, String> {
        if handle.is_null() {
            return Err("m2v_encode: handle is null".into());
        }
        if texts.is_null() || out.is_null() {
            return Err("m2v_encode: texts or out is null".into());
        }
        let handle = unsafe { &*handle };

        let blob = unsafe { std::slice::from_raw_parts(texts, texts_len) };
        let mut parts: Vec<&[u8]> = blob.split(|b| *b == 0).collect();
        // A trailing separator yields one empty tail, which is not a text.
        if matches!(parts.last(), Some(last) if last.is_empty()) {
            parts.pop();
        }
        if parts.len() != count {
            return Err(format!(
                "m2v_encode: blob holds {} texts but count is {count}",
                parts.len()
            ));
        }

        let sentences: Vec<String> = parts
            .iter()
            .map(|p| String::from_utf8_lossy(p).into_owned())
            .collect();

        let mut model = handle.model.clone();
        if max_length != 0 {
            model = model.with_max_length(Some(max_length));
        }
        let flat = model.encode_flat(&sentences);
        debug_assert_eq!(flat.len(), count * handle.dim);

        unsafe {
            std::ptr::copy_nonoverlapping(flat.as_ptr(), out, flat.len());
        }
        Ok(count)
    }));

    match result {
        Ok(Ok(n)) => n,
        Ok(Err(msg)) => {
            set_error(msg);
            0
        }
        Err(_) => {
            set_error("m2v_encode: panicked (see stderr)");
            0
        }
    }
}

/// Embedding width of a loaded model, or 0 for a null handle.
#[no_mangle]
pub extern "C" fn m2v_dim(handle: *const M2vHandle) -> usize {
    if handle.is_null() {
        0
    } else {
        unsafe { &*handle }.dim
    }
}

/// Release a handle from `m2v_load`. Null is a no-op.
#[no_mangle]
pub extern "C" fn m2v_free(handle: *mut M2vHandle) {
    if handle.is_null() {
        return;
    }
    drop(unsafe { Box::from_raw(handle) });
}

/// Copy the last error message into `buf` (NUL-terminated, truncated to
/// `buf_len`). Returns 0 when there is no error.
#[no_mangle]
pub extern "C" fn m2v_error(buf: *mut c_char, buf_len: usize) -> c_int {
    let slot = LAST_ERROR.get_or_init(|| Mutex::new(String::new()));
    let guard = match slot.lock() {
        Ok(g) => g,
        Err(_) => return -1,
    };
    if guard.is_empty() {
        return 0;
    }
    if buf.is_null() || buf_len == 0 {
        return guard.len() as c_int;
    }
    let bytes = guard.as_bytes();
    let n = bytes.len().min(buf_len - 1);
    unsafe {
        std::ptr::copy_nonoverlapping(bytes.as_ptr(), buf as *mut u8, n);
        *buf.add(n) = 0;
    }
    n as c_int
}

// Package loadtest is the Infinity / RAGFlow production load test.
//
// It is deliberately one Go program with the embedder compiled in:
//
//   - corpus reading, wikitext cleaning, row shaping and batching are Go;
//   - vectors come from model2vec-rs through a C ABI (tools/loadtest/m2v), so a
//     single static binary can go from wiki extract to populated tables without
//     a Python generator, a Rust CLI subprocess, or an intermediate vector file;
//   - all load phases (dense / text / hybrid / filtered / point lookups) run on
//     the Go SDK, one goroutine per client.
//
// The Rust cdylib exports m2v_load / m2v_encode / m2v_dim / m2v_free /
// m2v_error; see tools/loadtest/m2v/src/lib.rs. Build it with
// `tools/loadtest/build_m2v.sh` (the cgo LDFLAGS below point at its output).
package loadtest

/*
#cgo LDFLAGS: -L${SRCDIR}/m2v/target/release -lm2v -Wl,-rpath,${SRCDIR}/m2v/target/release

#include <stdint.h>
#include <stdlib.h>

typedef struct M2vHandle M2vHandle;

M2vHandle* m2v_load(const char* path, size_t expect_dim);
size_t     m2v_encode(M2vHandle* h, const unsigned char* texts, size_t texts_len,
                      size_t count, size_t max_length, float* out);
size_t     m2v_dim(const M2vHandle* h);
void       m2v_free(M2vHandle* h);
int        m2v_error(char* buf, size_t buf_len);
*/
import "C"

import (
	"crypto/sha256"
	"encoding/hex"
	"fmt"
	"io"
	"os"
	"path/filepath"
	"sync"
	"unsafe"
)

// PinnedModelFiles lists the expected SHA-256 hashes for the pinned
// sentence-transformers/static-retrieval-mrl-en-v1 model (plan §4.2.6).
var PinnedModelFiles = []struct {
	RelPath string
	SHA256  string
}{
	{"0_StaticEmbedding/model.safetensors", "164fc63ee9f9267be7378fcbd7df99d09788a2f45244c92aa99ae5a574925716"},
	{"0_StaticEmbedding/tokenizer.json", "d241a60d5e8f04cc1b2b3e9ef7a4921b27bf526d9f6050ab90f9267a1f9e5c66"},
	{"config_sentence_transformers.json", "5f6e0d37fdb5cc3d7812a204acd8fa26298ce75a4e5d99b903e8068d1a007653"},
}

// ValidatePinnedModel verifies that all pinned model files in dir match their
// Section 4.2.6 SHA-256 hashes and returns the validated hashes by relative path.
func ValidatePinnedModel(dir string) (map[string]string, error) {
	hashes := make(map[string]string, len(PinnedModelFiles))
	for _, spec := range PinnedModelFiles {
		p := filepath.Join(dir, spec.RelPath)
		f, err := os.Open(p)
		if err != nil {
			return nil, fmt.Errorf("validate model %s: %w", spec.RelPath, err)
		}
		h := sha256.New()
		_, copyErr := io.Copy(h, f)
		closeErr := f.Close()
		if copyErr != nil {
			return nil, fmt.Errorf("hash model %s: %w", spec.RelPath, copyErr)
		}
		if closeErr != nil {
			return nil, fmt.Errorf("close model %s: %w", spec.RelPath, closeErr)
		}
		got := hex.EncodeToString(h.Sum(nil))
		if got != spec.SHA256 {
			return nil, fmt.Errorf("model %s sha256 mismatch: %s != %s", spec.RelPath, got, spec.SHA256)
		}
		hashes[spec.RelPath] = got
	}
	return hashes, nil
}

// Embedder is a loaded static embedding model.
//
// Encode is safe to call from several goroutines: the handle is read-only after
// load. Throughput comes from encoding large batches, not from concurrency —
// model2vec-rs already parallelises inside one call — so concurrent callers
// should prefer bigger batches over more goroutines.
type Embedder struct {
	mu     sync.Mutex // guards use of the handle; see Encode
	handle *C.M2vHandle
	dim    int
	loaded bool
}

// Dim is the model's embedding width (1024 for the pinned model).
func (e *Embedder) Dim() int { return e.dim }

// LoadEmbedder loads a Model2Vec model directory and checks its width against
// expectDim. Pass 0 to accept whatever the model produces.
func LoadEmbedder(dir string, expectDim int) (*Embedder, error) {
	cPath := C.CString(dir)
	defer C.free(unsafe.Pointer(cPath))

	h := C.m2v_load(cPath, C.size_t(expectDim))
	if h == nil {
		return nil, fmt.Errorf("load embedder: %s", lastM2vError())
	}
	dim := int(C.m2v_dim(h))
	if dim <= 0 {
		C.m2v_free(h)
		return nil, fmt.Errorf("load embedder: model reports dimension %d", dim)
	}
	return &Embedder{handle: h, dim: dim, loaded: true}, nil
}

// Close releases the model. Safe to call more than once.
func (e *Embedder) Close() {
	e.mu.Lock()
	defer e.mu.Unlock()
	if e.loaded {
		C.m2v_free(e.handle)
		e.handle = nil
		e.loaded = false
	}
}

// Encode embeds texts and returns them row-major in a flat slice of len*Dim.
//
// The C blob is one NUL-separated buffer. Newlines inside a text would still be
// fine here (the separator is NUL, not '\n'), unlike a line-oriented CLI.
//
// maxTokens caps tokens per text (0 = the model default). 512 matches the
// model's own config and the generator's <=1500-char windows.
func (e *Embedder) Encode(texts []string, maxTokens int) ([]float32, error) {
	if len(texts) == 0 {
		return nil, nil
	}

	// Build the NUL-separated blob. Reject embedded NULs rather than silently
	// truncating a row, which would shift every following vector by one.
	blob := make([]byte, 0, 256*len(texts))
	for i, t := range texts {
		for j := 0; j < len(t); j++ {
			if t[j] == 0 {
				return nil, fmt.Errorf("text %d contains a NUL byte", i)
			}
		}
		blob = append(blob, t...)
		blob = append(blob, 0)
	}
	// cgo requires a NUL-terminated C string; the trailing one is already there.
	cBlob := C.CBytes(blob)
	defer C.free(cBlob)

	out := make([]float32, len(texts)*e.dim)

	e.mu.Lock()
	defer e.mu.Unlock()
	if !e.loaded {
		return nil, fmt.Errorf("encode: embedder is closed")
	}
	n := C.m2v_encode(
		e.handle,
		(*C.uchar)(cBlob),
		C.size_t(len(blob)),
		C.size_t(len(texts)),
		C.size_t(maxTokens),
		(*C.float)(unsafe.Pointer(&out[0])),
	)
	if int(n) != len(texts) {
		return nil, fmt.Errorf("encode: %s", lastM2vError())
	}
	return out, nil
}

// EncodeOne is Encode for a single text.
func (e *Embedder) EncodeOne(text string, maxTokens int) ([]float32, error) {
	vec, err := e.Encode([]string{text}, maxTokens)
	if err != nil {
		return nil, err
	}
	return vec, nil
}

func lastM2vError() string {
	buf := make([]byte, 1024)
	n := C.m2v_error((*C.char)(unsafe.Pointer(&buf[0])), C.size_t(len(buf)))
	if n <= 0 {
		return "unknown error"
	}
	if int(n) >= len(buf) {
		n = C.int(len(buf) - 1)
	}
	return string(buf[:n])
}

/* C ABI for the Rust text analyzers.
 *
 * Drop-in for internal/binding/cpp/rag_analyzer_c_api.h in RAGFlow: same
 * symbols, same struct layouts, same ownership. Every char* returned here is
 * allocated with malloc() and must be released with free() (or
 * RAGAnalyzer_FreeString); a RAGTokenList is released with
 * RAGAnalyzer_FreeTokenList.
 *
 * Not covered: the ThincNER_* / ThincParser_* / ThincTagger_* entry points of
 * the C++ library. Hosts that use them keep linking the C++ archive for those
 * symbols, or move them to their own library.
 *
 * ABI version: RAGAnalyzer_AbiVersion().
 */

#ifndef RAG_ANALYZER_C_API_H
#define RAG_ANALYZER_C_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

/* Opaque pointer to an analyzer. */
typedef void *RAGAnalyzerHandle;

/* Callback receiving one token. */
typedef void (*RAGTokenCallback)(const char *text, uint32_t len, uint32_t offset, uint32_t end_offset);

/* ------------------------------------------------------------------ */
/* Lifecycle                                                          */
/* ------------------------------------------------------------------ */

/* Create an analyzer rooted at the given path (the directory holding rag/,
 * wordnet/ and opencc/). Dictionaries are not loaded until RAGAnalyzer_Load.
 * Returns NULL on failure (see RAGAnalyzer_LastError). */
RAGAnalyzerHandle RAGAnalyzer_Create(const char *path);

/* Open the analyzer an index name selects - rag-coarse, rag-fine,
 * whitespace-#, rankfeatures, standard-en, ngram-3, ... - loading its
 * dictionaries from resource_dir. Returns NULL on failure. */
RAGAnalyzerHandle RAGAnalyzer_Open(const char *name, const char *resource_dir);

/* Copy a handle: the dictionaries are shared, the settings are the copy's own.
 * Only handles created by RAGAnalyzer_Create can be copied. */
RAGAnalyzerHandle RAGAnalyzer_Copy(RAGAnalyzerHandle handle);

/* Destroy a handle (NULL is a no-op). */
void RAGAnalyzer_Destroy(RAGAnalyzerHandle handle);

/* Load the dictionaries. Returns 0 on success, negative on failure. */
int RAGAnalyzer_Load(RAGAnalyzerHandle handle);

/* ------------------------------------------------------------------ */
/* Settings                                                           */
/* ------------------------------------------------------------------ */

void RAGAnalyzer_SetFineGrained(RAGAnalyzerHandle handle, bool fine_grained);
void RAGAnalyzer_SetEnablePosition(RAGAnalyzerHandle handle, bool enable_position);

/* Set the stemming language ("English", "Dutch", "German", ...). Unknown
 * languages keep the current stemmer, like the reference. */
void RAGAnalyzer_SetLanguage(RAGAnalyzerHandle handle, const char *language);

/* ------------------------------------------------------------------ */
/* Tokenizing                                                         */
/* ------------------------------------------------------------------ */

/* Invoke the callback once per token. Returns 0 on success, negative on
 * failure. */
int RAGAnalyzer_Analyze(RAGAnalyzerHandle handle, const char *text, RAGTokenCallback callback);

/* Space-joined token stream (coarse; use RAGAnalyzer_FineGrainedTokenize for
 * the fine-grained variant). Returns a malloc'd string, or NULL. */
char *RAGAnalyzer_Tokenize(RAGAnalyzerHandle handle, const char *text);

typedef struct {
    char *text;          /* malloc'd token text */
    uint32_t offset;     /* byte offset in the input */
    uint32_t end_offset; /* byte end offset in the input */
} RAGTokenWithPosition;

typedef struct {
    RAGTokenWithPosition *tokens;
    uint32_t count;
} RAGTokenList;

/* Tokens with byte offsets. When fine-grained mode is on the tokens are
 * already split finer. Returns a malloc'd list, or NULL. */
RAGTokenList *RAGAnalyzer_TokenizeWithPosition(RAGAnalyzerHandle handle, const char *text);

void RAGAnalyzer_FreeTokenList(RAGTokenList *token_list);

/* Field accessors for CGO (Go cannot index a C struct field of a void*). */
const char *RAGToken_GetText(void *token);
uint32_t RAGToken_GetOffset(void *token);
uint32_t RAGToken_GetEndOffset(void *token);

/* Fine-grained tokenize: space-separated tokens in, space-separated tokens out.
 * Only the rag family implements this; NULL otherwise. */
char *RAGAnalyzer_FineGrainedTokenize(RAGAnalyzerHandle handle, const char *tokens);

/* Dictionary frequency of the term (Python rag_tokenizer.freq); 0 when
 * unknown. */
int32_t RAGAnalyzer_GetTermFreq(RAGAnalyzerHandle handle, const char *term);

/* POS tag of the term (Python rag_tokenizer.tag); NULL when unknown or
 * untagged. */
char *RAGAnalyzer_GetTermTag(RAGAnalyzerHandle handle, const char *term);

/* ------------------------------------------------------------------ */
/* Diagnostics (extensions over the C++ header)                       */
/* ------------------------------------------------------------------ */

/* 1 for this revision. Check it at startup. */
uint32_t RAGAnalyzer_AbiVersion(void);

/* The error of the last failed call on this thread, or "". */
const char *RAGAnalyzer_LastError(void);

/* Free a string this library returned. */
void RAGAnalyzer_FreeString(char *text);

#ifdef __cplusplus
}
#endif

#endif /* RAG_ANALYZER_C_API_H */

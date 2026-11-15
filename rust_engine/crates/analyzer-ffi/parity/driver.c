#include "rag_analyzer_c_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *TEXTS[] = {
    "The Quick Brown Foxes are Running!",
    "We are testing the RAG analyzer's tokenization, aren't we?",
    "\xe6\x88\x91\xe4\xbb\xac\xe9\x83\xbd\xe6\x9c\x89\xe4\xb8\x80\xe4\xb8\xaa\xe5\xae\xb6",
    "\xe4\xb8\xad\xe6\x96\x87 English \xe6\xb7\xb7\xe5\x90\x88\xe7\x9a\x84 text 123",
    "\xe7\xb9\x81\xe9\xab\x94\xe5\xad\x97\xe8\xbd\x89\xe6\x8f\x9b\xe6\xb8\xac\xe8\xa9\xa6",
    "H\xc3\xa4user Stra"
    "\xc3\x9f"
    "e und Gr\xc3\xb6"
    "\xc3\x9f"
    "e",
    "a/b/c and x_y-z",
    "Hello, World! \xe4\xbd\xa0\xe5\xa5\xbd\xe3\x80\x82",
    "2024\xe5\xb9\xb4 3 \xe6\x9c\x88 15 \xe6\x97\xa5",
    "   leading and trailing   ",
};
static const int TEXT_COUNT = sizeof(TEXTS) / sizeof(TEXTS[0]);

static const char *WORDS[] = {
    "\xe6\x88\x91\xe4\xbb\xac",
    "fox",
    "the",
    "H\xc3\xa4user",
    "zzzzzzzz",
    "\xe4\xb8\xad\xe5\x9b\xbd",
};
static const int WORD_COUNT = sizeof(WORDS) / sizeof(WORDS[0]);

static void dump(RAGAnalyzerHandle h, const char *tag, const char *text) {
    char *tokens = RAGAnalyzer_Tokenize(h, text);
    printf("R tok %s | %s\n", tag, tokens ? tokens : "(null)");
    free(tokens);

    RAGTokenList *list = RAGAnalyzer_TokenizeWithPosition(h, text);
    printf("R pos %s |", tag);
    if (list) {
        for (uint32_t i = 0; i < list->count; ++i) {
            void *slot = (void *)&list->tokens[i];
            printf(" %s@%u-%u", RAGToken_GetText(slot), RAGToken_GetOffset(slot), RAGToken_GetEndOffset(slot));
        }
    }
    RAGAnalyzer_FreeTokenList(list);
    printf("\n");
}

int main(int argc, char **argv) {
    if (argc < 2)
        return 2;
    RAGAnalyzerHandle h = RAGAnalyzer_Create(argv[1]);
    if (!h) {
        printf("R create failed\n");
        return 3;
    }
    if (RAGAnalyzer_Load(h) != 0) {
        printf("R load failed\n");
        return 4;
    }

    for (int i = 0; i < TEXT_COUNT; ++i) {
        char tag[32];
        snprintf(tag, sizeof tag, "coarse-%d", i);
        dump(h, tag, TEXTS[i]);
    }
    RAGAnalyzer_SetEnablePosition(h, 1);
    for (int i = 0; i < TEXT_COUNT; ++i) {
        char tag[32];
        snprintf(tag, sizeof tag, "position-%d", i);
        dump(h, tag, TEXTS[i]);
    }
    RAGAnalyzer_SetFineGrained(h, 1);
    for (int i = 0; i < TEXT_COUNT; ++i) {
        char tag[32];
        snprintf(tag, sizeof tag, "fine-%d", i);
        dump(h, tag, TEXTS[i]);
    }
    RAGAnalyzer_SetFineGrained(h, 0);
    RAGAnalyzer_SetEnablePosition(h, 0);

    for (int i = 0; i < TEXT_COUNT; ++i) {
        char *tokens = RAGAnalyzer_Tokenize(h, TEXTS[i]);
        char tag[32];
        snprintf(tag, sizeof tag, "fgt-%d", i);
        char *fine = RAGAnalyzer_FineGrainedTokenize(h, tokens ? tokens : "");
        printf("R fine-tokenize %s | %s\n", tag, fine ? fine : "(null)");
        free(tokens);
        free(fine);
    }

    const char *langs[] = {"English", "Dutch", "German", "Slovak", "Czech", "French", "Turkish"};
    for (int i = 0; i < 7; ++i) {
        RAGAnalyzer_SetLanguage(h, langs[i]);
        for (int t = 0; t < TEXT_COUNT; ++t) {
            char tag[64];
            snprintf(tag, sizeof tag, "%s-%d", langs[i], t);
            dump(h, tag, TEXTS[t]);
        }
    }
    RAGAnalyzer_SetLanguage(h, "English");

    for (int i = 0; i < WORD_COUNT; ++i) {
        char *tag = RAGAnalyzer_GetTermTag(h, WORDS[i]);
        printf("R word %d | freq=%d tag=%s\n", i, RAGAnalyzer_GetTermFreq(h, WORDS[i]), tag ? tag : "(null)");
        free(tag);
    }

    RAGAnalyzerHandle copy = RAGAnalyzer_Copy(h);
    char *copied = copy ? RAGAnalyzer_Tokenize(copy, TEXTS[2]) : NULL;
    printf("R copy | %s\n", copied ? copied : "(null)");
    free(copied);
    RAGAnalyzer_Destroy(copy);
    RAGAnalyzer_Destroy(h);
    return 0;
}

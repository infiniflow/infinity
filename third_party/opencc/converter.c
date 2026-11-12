/*
 * Open Chinese Convert
 *
 * Copyright 2010 BYVoid <byvoid.kcp@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "converter.h"
#include "dictionary_set.h"
#include "encoding.h"
#include "utils.h"

#define DELIMITER ' '
#define SEGMENT_MAXIMUM_LENGTH 0
#define SEGMENT_SHORTEST_PATH 1
#define SEGMENT_METHOD SEGMENT_SHORTEST_PATH

#if SEGMENT_METHOD == SEGMENT_SHORTEST_PATH

/* Number of UCS-4 units served by the on-stack scratch block of sp_seg(). A
   span is one ambiguous run of a single text, so keeping the common case
   allocation-free is worth a few KB of stack. Longer spans fall back to one
   heap block that is freed before sp_seg() returns. */
#define OPENCC_SP_SEG_STACK_LENGTH 128

/* Scratch arrays of a single sp_seg() call.
 *
 * They are deliberately NOT cached in converter_desc. One opencc_t is shared by
 * all worker threads of a fulltext indexer (RAGAnalyzer clones share the same
 * ::OpenCC), and caching the scratch made concurrent conversions reset and
 * reallocate each other's min_len/parent/path. The visible symptom was a wild
 * write in the path reconstruction below, where j starts at min_len[length] and
 * a buffer reset by another thread left it at INFINITY_INT. */
typedef struct {
    size_t *match_length;
    size_t *min_len;
    size_t *parent;
    size_t *path;
    size_t *heap_block;
} spseg_buffer_desc;

#endif

typedef struct {
    dictionary_set_t dictionary_set;
    opencc_conversion_mode conversion_mode;
} converter_desc;

/* Everything mutable during a conversion now lives on the stack or in a per-call
 * heap block, and the dictionary set is only read after open, so one opencc_t
 * may be converted from several threads at once without external locking.
 * The error code is per thread to keep it meaningful under that sharing. */
static OPENCC_THREAD_LOCAL converter_error errnum = CONVERTER_ERROR_VOID;

#if SEGMENT_METHOD == SEGMENT_SHORTEST_PATH
/* Shortest-path segmentation over one ambiguous span. Takes the scratch
 * explicitly so that no per-converter state is touched (see the note above). */
static size_t sp_seg_convert(converter_desc *converter,
                             dictionary_group_t group,
                             spseg_buffer_desc *ossb,
                             ucs4_t **inbuf,
                             size_t *inbuf_left,
                             ucs4_t **outbuf,
                             size_t *outbuf_left,
                             size_t length) {
    /* 最短路徑分詞 */
    {
        size_t i, j;

        for (i = 0; i <= length; i++)
            ossb->min_len[i] = INFINITY_INT;

        ossb->min_len[0] = ossb->parent[0] = 0;

        for (i = 0; i < length; i++) {
            /* 獲取所有匹配長度 */
            size_t match_count = dictionary_group_get_all_match_lengths(group, (*inbuf) + i, ossb->match_length);

            /* match_count == 0 means no prefix of the remaining text is in the
             * dictionary, and ossb->match_length[0] then still holds the value
             * of an earlier call. Inspecting it used to skip the
             * single-character fallback, leaving min_len[i + 1] at INFINITY_INT
             * and producing the wild write guarded below. */
            if (match_count == 0 || ossb->match_length[0] != 1)
                ossb->match_length[match_count++] = 1;

            /* 動態規劃求最短分割路徑 */
            for (j = 0; j < match_count; j++) {
                size_t k = ossb->match_length[j];
                ossb->match_length[j] = 0;

                if (k > 1 && ossb->min_len[i] + 1 <= ossb->min_len[i + k]) {
                    ossb->min_len[i + k] = ossb->min_len[i] + 1;
                    ossb->parent[i + k] = i;
                } else if (k == 1 && ossb->min_len[i] + 1 < ossb->min_len[i + k]) {
                    ossb->min_len[i + k] = ossb->min_len[i] + 1;
                    ossb->parent[i + k] = i;
                }
            }
        }

        if (ossb->min_len[length] == (size_t)INFINITY_INT) {
            /* Cannot happen while every position offers the single-character
             * fallback. Kept as a hard stop so that a future dictionary or
             * algorithm change fails the conversion instead of writing through
             * path[INFINITY_INT - 1]. */
            errnum = CONVERTER_ERROR_OUTBUF;
            return (size_t)-1;
        }

        /* 取得最短分割路徑 */
        for (i = length, j = ossb->min_len[length]; i != 0; i = ossb->parent[i])
            ossb->path[--j] = i;
    }

    size_t inbuf_left_start = *inbuf_left;
    size_t begin, end;

    /* 根據最短分割路徑轉換 */
    for (size_t i = begin = 0; i < ossb->min_len[length]; i++) {
        end = ossb->path[i];

        size_t match_len;
        const ucs4_t *const *match_rs = dictionary_group_match_longest(group, *inbuf, end - begin, &match_len);

        if (match_rs == NULL) {
            **outbuf = **inbuf;
            (*outbuf)++, (*outbuf_left)--;
            (*inbuf)++, (*inbuf_left)--;
        } else {
            if (converter->conversion_mode == OPENCC_CONVERSION_FAST) {
                if (match_rs == NULL) {
                    **outbuf = **inbuf;
                    (*outbuf)++, (*outbuf_left)--;
                    (*inbuf)++, (*inbuf_left)--;
                } else {
                    const ucs4_t *result = match_rs[0];

                    /* 輸出緩衝區剩餘空間小於分詞長度 */
                    if (ucs4len(result) > *outbuf_left) {
                        if (inbuf_left_start - *inbuf_left > 0)
                            break;
                        errnum = CONVERTER_ERROR_OUTBUF;
                        return (size_t)-1;
                    }

                    for (; *result; result++) {
                        **outbuf = *result;
                        (*outbuf)++, (*outbuf_left)--;
                    }

                    *inbuf += match_len;
                    *inbuf_left -= match_len;
                }
            } else if (converter->conversion_mode == OPENCC_CONVERSION_LIST_CANDIDATES) {
                if (match_rs == NULL) {
                    **outbuf = **inbuf;
                    (*outbuf)++, (*outbuf_left)--;
                    (*inbuf)++, (*inbuf_left)--;
                } else {
                    size_t i;
                    for (i = 0; match_rs[i] != NULL; i++) {
                        const ucs4_t *result = match_rs[i];
                        int show_delimiter = match_rs[i + 1] != NULL ? 1 : 0;

                        /* 輸出緩衝區剩餘空間小於分詞長度 */
                        if (ucs4len(result) + show_delimiter > *outbuf_left) {
                            if (inbuf_left_start - *inbuf_left > 0)
                                break;
                            errnum = CONVERTER_ERROR_OUTBUF;
                            return (size_t)-1;
                        }

                        for (; *result; result++) {
                            **outbuf = *result;
                            (*outbuf)++, (*outbuf_left)--;
                        }

                        if (show_delimiter) {
                            **outbuf = DELIMITER;
                            (*outbuf)++, (*outbuf_left)--;
                        }
                    }
                    *inbuf += match_len;
                    *inbuf_left -= match_len;
                }
            } else if (converter->conversion_mode == OPENCC_CONVERSION_SEGMENT_ONLY) {
                if (match_rs == NULL) {
                    **outbuf = **inbuf;
                    (*outbuf)++, (*outbuf_left)--;
                    (*inbuf)++, (*inbuf_left)--;
                } else {
                    /* 輸出緩衝區剩餘空間小於分詞長度 */
                    if (match_len + 1 > *outbuf_left) {
                        if (inbuf_left_start - *inbuf_left > 0)
                            break;
                        errnum = CONVERTER_ERROR_OUTBUF;
                        return (size_t)-1;
                    }

                    size_t i;
                    for (i = 0; i < match_len; i++) {
                        **outbuf = **inbuf;
                        (*outbuf)++, (*outbuf_left)--;
                        (*inbuf)++, (*inbuf_left)--;
                    }
                }
                **outbuf = DELIMITER;
                (*outbuf)++, (*outbuf_left)--;
            } else
                debug_should_not_be_here();
        }

        begin = end;
    }

    return inbuf_left_start - *inbuf_left;
}

static size_t sp_seg(converter_desc *converter, dictionary_group_t group, ucs4_t **inbuf, size_t *inbuf_left, ucs4_t **outbuf, size_t *outbuf_left, size_t length) {
    /* 最短路徑分詞 */

    /* 對長度爲1時特殊優化 */
    if (length == 1) {
        const ucs4_t *const *match_rs = dictionary_group_match_longest(group, *inbuf, 1, NULL);

        size_t match_len = 1;
        if (converter->conversion_mode == OPENCC_CONVERSION_FAST) {
            if (match_rs == NULL) {
                **outbuf = **inbuf;
                (*outbuf)++, (*outbuf_left)--;
                (*inbuf)++, (*inbuf_left)--;
            } else {
                const ucs4_t *result = match_rs[0];

                /* 輸出緩衝區剩餘空間小於分詞長度 */
                if (ucs4len(result) > *outbuf_left) {
                    errnum = CONVERTER_ERROR_OUTBUF;
                    return (size_t)-1;
                }

                for (; *result; result++) {
                    **outbuf = *result;
                    (*outbuf)++, (*outbuf_left)--;
                }

                *inbuf += match_len;
                *inbuf_left -= match_len;
            }
        } else if (converter->conversion_mode == OPENCC_CONVERSION_LIST_CANDIDATES) {
            if (match_rs == NULL) {
                **outbuf = **inbuf;
                (*outbuf)++, (*outbuf_left)--;
                (*inbuf)++, (*inbuf_left)--;
            } else {
                size_t i;
                for (i = 0; match_rs[i] != NULL; i++) {
                    const ucs4_t *result = match_rs[i];
                    int show_delimiter = match_rs[i + 1] != NULL ? 1 : 0;

                    /* 輸出緩衝區剩餘空間小於分詞長度 */
                    if (ucs4len(result) + show_delimiter > *outbuf_left) {
                        errnum = CONVERTER_ERROR_OUTBUF;
                        return (size_t)-1;
                    }

                    for (; *result; result++) {
                        **outbuf = *result;
                        (*outbuf)++, (*outbuf_left)--;
                    }

                    if (show_delimiter) {
                        **outbuf = DELIMITER;
                        (*outbuf)++, (*outbuf_left)--;
                    }
                }
                *inbuf += match_len;
                *inbuf_left -= match_len;
            }
        } else if (converter->conversion_mode == OPENCC_CONVERSION_SEGMENT_ONLY) {
            if (match_rs == NULL) {
                **outbuf = **inbuf;
                (*outbuf)++, (*outbuf_left)--;
                (*inbuf)++, (*inbuf_left)--;
            } else {
                /* 輸出緩衝區剩餘空間小於分詞長度 */
                if (match_len + 1 > *outbuf_left) {
                    errnum = CONVERTER_ERROR_OUTBUF;
                    return (size_t)-1;
                }

                size_t i;
                for (i = 0; i < match_len; i++) {
                    **outbuf = **inbuf;
                    (*outbuf)++, (*outbuf_left)--;
                    (*inbuf)++, (*inbuf_left)--;
                }
            }
            **outbuf = DELIMITER;
            (*outbuf)++, (*outbuf_left)--;
        } else
            debug_should_not_be_here();
        /* 必須保證有一個字符空間 */
        return match_len;
    }

    /* Scratch is per call, never cached in the converter: it is shared between
     * threads, so a cached buffer turns concurrent conversions into a data race
     * on min_len/parent/path. See the note on spseg_buffer_desc. */
    size_t stack_block[OPENCC_SP_SEG_STACK_LENGTH * 4];
    spseg_buffer_desc ossb_desc;
    spseg_buffer_desc *ossb = &ossb_desc;
    ossb->heap_block = NULL;

    if (length + 2 <= OPENCC_SP_SEG_STACK_LENGTH) {
        ossb->match_length = stack_block;
        ossb->min_len = stack_block + OPENCC_SP_SEG_STACK_LENGTH;
        ossb->parent = stack_block + 2 * OPENCC_SP_SEG_STACK_LENGTH;
        ossb->path = stack_block + 3 * OPENCC_SP_SEG_STACK_LENGTH;
    } else {
        /* One block for all four arrays: match_length needs length + 2 entries,
         * the other three need length + 1, so the same stride is used. */
        size_t block_len = length + 2;
        ossb->heap_block = (size_t *)malloc(sizeof(size_t) * 4 * block_len);
        if (ossb->heap_block == NULL) {
            errnum = CONVERTER_ERROR_OUTBUF;
            return (size_t)-1;
        }
        ossb->match_length = ossb->heap_block;
        ossb->min_len = ossb->heap_block + block_len;
        ossb->parent = ossb->heap_block + 2 * block_len;
        ossb->path = ossb->heap_block + 3 * block_len;
    }

    size_t ret = sp_seg_convert(converter, group, ossb, inbuf, inbuf_left, outbuf, outbuf_left, length);
    free(ossb->heap_block);
    return ret;
}

static size_t
segment(converter_desc *converter, dictionary_group_t group, ucs4_t **inbuf, size_t *inbuf_left, ucs4_t **outbuf, size_t *outbuf_left) {
    /* 歧義分割最短路徑分詞 */
    size_t i, start, bound;
    const ucs4_t *inbuf_start = *inbuf;
    size_t inbuf_left_start = *inbuf_left;
    size_t sp_seg_length;

    bound = 0;

    for (i = start = 0; inbuf_start[i] && *inbuf_left > 0 && *outbuf_left > 0; i++) {
        if (i != 0 && i == bound) {
            /* 對歧義部分進行最短路徑分詞 */
            sp_seg_length = sp_seg(converter, group, inbuf, inbuf_left, outbuf, outbuf_left, bound - start);
            if (sp_seg_length == (size_t)-1)
                return (size_t)-1;
            if (sp_seg_length == 0) {
                if (inbuf_left_start - *inbuf_left > 0)
                    return inbuf_left_start - *inbuf_left;
                /* 空間不足 */
                errnum = CONVERTER_ERROR_OUTBUF;
                return (size_t)-1;
            }
            start = i;
        }

        size_t match_len;
        dictionary_group_match_longest(group, inbuf_start + i, 0, &match_len);

        if (match_len == 0)
            match_len = 1;

        if (i + match_len > bound)
            bound = i + match_len;
    }

    if (*inbuf_left > 0 && *outbuf_left > 0) {
        sp_seg_length = sp_seg(converter, group, inbuf, inbuf_left, outbuf, outbuf_left, bound - start);
        if (sp_seg_length == (size_t)-1)
            return (size_t)-1;
        if (sp_seg_length == 0) {
            if (inbuf_left_start - *inbuf_left > 0)
                return inbuf_left_start - *inbuf_left;
            /* 空間不足 */
            errnum = CONVERTER_ERROR_OUTBUF;
            return (size_t)-1;
        }
    }

    if (converter->conversion_mode == OPENCC_CONVERSION_SEGMENT_ONLY) {
        (*outbuf)--;
        (*outbuf_left)++;
    }

    return inbuf_left_start - *inbuf_left;
}

#endif

#if SEGMENT_METHOD == SEGMENT_MAXIMUM_LENGTH
static size_t
segment(converter_desc *converter, dictionary_group_t group, ucs4_t **inbuf, size_t *inbuf_left, ucs4_t **outbuf, size_t *outbuf_left) {
    /* 正向最大分詞 */
    size_t inbuf_left_start = *inbuf_left;

    for (; **inbuf && *inbuf_left > 0 && *outbuf_left > 0;) {
        size_t match_len;
        const ucs4_t *const *match_rs = dictionary_group_match_longest(group, *inbuf, *inbuf_left, &match_len);

        if (converter->conversion_mode == OPENCC_CONVERSION_FAST) {
            if (match_rs == NULL) {
                **outbuf = **inbuf;
                (*outbuf)++, (*outbuf_left)--;
                (*inbuf)++, (*inbuf_left)--;
            } else {
                const ucs4_t *result = match_rs[0];

                /* 輸出緩衝區剩餘空間小於分詞長度 */
                if (ucs4len(result) > *outbuf_left) {
                    if (inbuf_left_start - *inbuf_left > 0)
                        break;
                    errnum = CONVERTER_ERROR_OUTBUF;
                    return (size_t)-1;
                }

                for (; *result; result++) {
                    **outbuf = *result;
                    (*outbuf)++, (*outbuf_left)--;
                }

                *inbuf += match_len;
                *inbuf_left -= match_len;
            }
        } else if (converter->conversion_mode == OPENCC_CONVERSION_LIST_CANDIDATES) {
            if (match_rs == NULL) {
                **outbuf = **inbuf;
                (*outbuf)++, (*outbuf_left)--;
                (*inbuf)++, (*inbuf_left)--;
            } else {
                size_t i;
                for (i = 0; match_rs[i] != NULL; i++) {
                    const ucs4_t *result = match_rs[i];
                    int show_delimiter = match_rs[i + 1] != NULL ? 1 : 0;

                    /* 輸出緩衝區剩餘空間小於分詞長度 */
                    if (ucs4len(result) + show_delimiter > *outbuf_left) {
                        if (inbuf_left_start - *inbuf_left > 0)
                            break;
                        errnum = CONVERTER_ERROR_OUTBUF;
                        return (size_t)-1;
                    }

                    for (; *result; result++) {
                        **outbuf = *result;
                        (*outbuf)++, (*outbuf_left)--;
                    }

                    if (show_delimiter) {
                        **outbuf = DELIMITER;
                        (*outbuf)++, (*outbuf_left)--;
                    }
                }

                *inbuf += match_len;
                *inbuf_left -= match_len;
            }
        } else if (converter->conversion_mode == OPENCC_CONVERSION_SEGMENT_ONLY) {
            if (match_rs == NULL) {
                **outbuf = **inbuf;
                (*outbuf)++, (*outbuf_left)--;
                (*inbuf)++, (*inbuf_left)--;
            } else {
                /* 輸出緩衝區剩餘空間小於分詞長度 */
                if (match_len + 1 > *outbuf_left) {
                    if (inbuf_left_start - *inbuf_left > 0)
                        break;
                    errnum = CONVERTER_ERROR_OUTBUF;
                    return (size_t)-1;
                }

                size_t i;
                for (i = 0; i < match_len; i++) {
                    **outbuf = **inbuf;
                    (*outbuf)++, (*outbuf_left)--;
                    (*inbuf)++, (*inbuf_left)--;
                }
            }
            **outbuf = DELIMITER;
            (*outbuf)++, (*outbuf_left)--;
        } else
            debug_should_not_be_here();
    }

    if (converter->conversion_mode == OPENCC_CONVERSION_SEGMENT_ONLY) {
        (*outbuf)--;
        (*outbuf_left)++;
    }

    return inbuf_left_start - *inbuf_left;
}
#endif

size_t converter_convert(converter_t t_converter, ucs4_t **inbuf, size_t *inbuf_left, ucs4_t **outbuf, size_t *outbuf_left) {
    converter_desc *converter = (converter_desc *)t_converter;

    if (converter->dictionary_set == NULL) {
        errnum = CONVERTER_ERROR_NODICT;
        return (size_t)-1;
    }

    if (dictionary_set_count_group(converter->dictionary_set) == 1) {
        /* 只有一個辭典，直接輸出 */
        return segment(converter, dictionary_set_get_group(converter->dictionary_set, 0), inbuf, inbuf_left, outbuf, outbuf_left);
    }

    // 啓用辭典轉換鏈
    size_t inbuf_size = *inbuf_left;
    size_t outbuf_size = *outbuf_left;
    size_t retval = (size_t)-1;
    size_t cinbuf_left, coutbuf_left;
    size_t coutbuf_delta = 0;
    size_t i, cur;

    ucs4_t *tmpbuf = (ucs4_t *)malloc(sizeof(ucs4_t) * outbuf_size);
    ucs4_t *orig_outbuf = *outbuf;
    ucs4_t *cinbuf, *coutbuf;

    cinbuf_left = inbuf_size;
    coutbuf_left = outbuf_size;
    cinbuf = *inbuf;
    coutbuf = tmpbuf;

    for (i = cur = 0; i < dictionary_set_count_group(converter->dictionary_set); ++i, cur = 1 - cur) {
        if (i > 0) {
            cinbuf_left = coutbuf_delta;
            coutbuf_left = outbuf_size;
            if (cur == 1) {
                cinbuf = tmpbuf;
                coutbuf = orig_outbuf;
            } else {
                cinbuf = orig_outbuf;
                coutbuf = tmpbuf;
            }
        }

        /* The active group is passed down instead of being stored on the
         * converter: two threads converting the same opencc_t would otherwise
         * overwrite each other's group and convert with the wrong dictionary. */
        dictionary_group_t group = dictionary_set_get_group(converter->dictionary_set, i);

        size_t ret = segment(converter, group, &cinbuf, &cinbuf_left, &coutbuf, &coutbuf_left);
        if (ret == (size_t)-1) {
            free(tmpbuf);
            return (size_t)-1;
        }
        coutbuf_delta = outbuf_size - coutbuf_left;
        if (i == 0) {
            retval = ret;
            *inbuf = cinbuf;
            *inbuf_left = cinbuf_left;
        }
    }

    if (cur == 1) {
        // 結果在緩衝區
        memcpy(*outbuf, tmpbuf, coutbuf_delta * sizeof(ucs4_t));
    }

    *outbuf += coutbuf_delta;
    *outbuf_left = coutbuf_left;
    free(tmpbuf);

    return retval;
}

void converter_assign_dictionary(converter_t t_converter, dictionary_set_t dictionary_set) {
    converter_desc *converter = (converter_desc *)t_converter;
    converter->dictionary_set = dictionary_set;
}

converter_t converter_open(void) {
    converter_desc *converter = (converter_desc *)malloc(sizeof(converter_desc));

    converter->dictionary_set = NULL;

    return (converter_t)converter;
}

void converter_close(converter_t t_converter) {
    converter_desc *converter = (converter_desc *)t_converter;

    free(converter);
}

void converter_set_conversion_mode(converter_t t_converter, opencc_conversion_mode conversion_mode) {
    converter_desc *converter = (converter_desc *)t_converter;
    converter->conversion_mode = conversion_mode;
}

converter_error converter_errno(void) { return errnum; }

void converter_perror(const char *spec) {
    perr(spec);
    perr("\n");
    switch (errnum) {
        case CONVERTER_ERROR_VOID:
            break;
        case CONVERTER_ERROR_NODICT:
            perr(_("No dictionary loaded"));
            break;
        case CONVERTER_ERROR_OUTBUF:
            perr(_("Output buffer not enough for one segment"));
            break;
        default:
            perr(_("Unknown"));
    }
}

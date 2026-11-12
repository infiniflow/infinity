// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

#include "unit_test/gtest_expand.h"

#include "opencc.h"

#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

module infinity_core:ut.opencc_thread_safety;

import :ut.base_test;

using namespace infinity;

namespace fs = std::filesystem;

// One ::OpenCC is shared by every copy of a RAGAnalyzer, i.e. by every fulltext
// indexing worker that tokenizes a column value, and several workers convert at
// the same time. The conversion used to keep its shortest-path segmentation
// scratch inside the converter and its lookup key inside the dictionary, so
// concurrent conversions overwrote each other's min_len/parent/path arrays.
// Reported symptom: SIGSEGV in sp_seg() (converter.c, path reconstruction) from
// a MemoryIndexer::AsyncInsertBottom worker.
//
// The dictionaries below are plain TEXT dictionaries on purpose: unlike the
// shipped .ocd ones they answer multi-character phrases from
// dictionary_group_match_longest(), which is what makes sp_seg() run with
// length > 1 and touch that scratch at all.
class OpenCCThreadSafetyTest : public BaseTest {
public:
    void SetUp() override {
        BaseTest::SetUp();
        dict_dir_ = fs::temp_directory_path() / ("infinity_ut_opencc_reentrancy_" + std::to_string(getpid()));
        std::error_code ec;
        fs::remove_all(dict_dir_, ec);
        ASSERT_TRUE(fs::create_directories(dict_dir_, ec)) << dict_dir_ << ": " << ec.message();
    }

    void TearDown() override {
        std::error_code ec;
        fs::remove_all(dict_dir_, ec);
        BaseTest::TearDown();
    }

    static bool WriteTextFile(const fs::path &path, const char *content) {
        FILE *fp = fopen(path.string().c_str(), "wb");
        if (fp == nullptr) {
            return false;
        }
        const bool ok = fputs(content, fp) >= 0;
        fclose(fp);
        return ok;
    }

    static std::string MakeRepeated(const char *unit, size_t repeat) {
        std::string text;
        text.reserve(strlen(unit) * repeat);
        for (size_t i = 0; i < repeat; ++i) {
            text += unit;
        }
        return text;
    }

    // Returns the conversion result, or nullptr when the conversion failed.
    static char *Convert(opencc_t od, const char *input) {
        char *out = opencc_convert_utf8(od, input, (size_t)-1);
        return out == (char *)-1 ? nullptr : out;
    }

    struct WorkerArgs {
        opencc_t od;
        const char *input;
        const char *expected;
        size_t iterations;
        size_t mismatches;
    };

    static void *ConvertLoop(void *arg) {
        auto *args = static_cast<WorkerArgs *>(arg);
        for (size_t i = 0; i < args->iterations; ++i) {
            char *out = Convert(args->od, args->input);
            if (out == nullptr || strcmp(out, args->expected) != 0) {
                ++args->mismatches;
            }
            free(out);
        }
        return nullptr;
    }

protected:
    fs::path dict_dir_;

    // Every phrase is longer than one character, so the segmentation has to run
    // the shortest-path pass over spans of several code points.
    static constexpr const char *PHRASES = "中華人民共和國\t中华人民共和国\n"
                                           "這個時候\t这个时候\n"
                                           "我們發現\t我们发现\n"
                                           "另外一個問題\t另外一个问题\n"
                                           "臺灣的經濟發展\t台湾的经济发展\n"
                                           "後來怎麼樣了呢\t后来怎么样了呢\n";

    static constexpr const char *CONFIG = "title = reentrancy\n"
                                          "dict0 = TEXT phrases.txt\n";

    static constexpr const char *UNIT = "中華人民共和國這個時候我們發現另外一個問題臺灣的經濟發展後來怎麼樣了呢";

    static constexpr size_t THREAD_COUNT = 8;
    static constexpr size_t ITERATIONS = 200;
};

TEST_F(OpenCCThreadSafetyTest, shared_converter_is_reentrant) {
    const fs::path config_path = dict_dir_ / "zht2zhs.ini";
    ASSERT_TRUE(WriteTextFile(config_path, CONFIG));
    ASSERT_TRUE(WriteTextFile(dict_dir_ / "phrases.txt", PHRASES));

    opencc_t od = opencc_open(config_path.string().c_str(), dict_dir_.string().c_str());
    ASSERT_NE(od, (opencc_t)-1) << "cannot open " << config_path;

    // Different lengths so that concurrent conversions disagree on the scratch
    // size sp_seg() needs.
    const std::vector<std::string> inputs{MakeRepeated(UNIT, 1), MakeRepeated(UNIT, 8), MakeRepeated(UNIT, 40)};

    std::vector<std::string> expected;
    expected.reserve(inputs.size());
    for (const std::string &input : inputs) {
        char *out = Convert(od, input.c_str());
        ASSERT_NE(out, nullptr) << "single-threaded reference conversion failed";
        expected.emplace_back(out);
        free(out);
    }
    ASSERT_NE(expected[0].find("这个时候"), std::string::npos) << "phrase dictionary is not effective: " << expected[0].substr(0, 24);

    std::vector<WorkerArgs> args(THREAD_COUNT);
    std::vector<pthread_t> threads(THREAD_COUNT);
    for (size_t t = 0; t < THREAD_COUNT; ++t) {
        const size_t idx = t % inputs.size();
        args[t] = WorkerArgs{od, inputs[idx].c_str(), expected[idx].c_str(), ITERATIONS, 0};
        ASSERT_EQ(pthread_create(&threads[t], nullptr, ConvertLoop, &args[t]), 0);
    }

    size_t mismatches = 0;
    for (size_t t = 0; t < THREAD_COUNT; ++t) {
        pthread_join(threads[t], nullptr);
        mismatches += args[t].mismatches;
    }
    opencc_close(od);

    // Before the fix this test either crashed inside sp_seg() or returned
    // corrupted conversions, depending on how the racing threads interleaved.
    EXPECT_EQ(mismatches, 0u) << "concurrent conversions of one shared converter disagree with the single-threaded result";
}

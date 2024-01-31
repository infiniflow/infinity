// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
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

#include "base_profiler.h"
#include <errno.h>
#include <fcntl.h>
#include <filesystem>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
import stl;
import third_party;
import fst;

using namespace infinity;
namespace fs = std::filesystem;

static BaseProfiler profiler;

int MmapFile(const String &fp, u8 *&data_ptr, SizeT &data_len) {
    data_ptr = nullptr;
    data_len = 0;
    long len_f = fs::file_size(fp);
    if (len_f == 0)
        return -1;
    int f = open(fp.c_str(), O_RDONLY);
    void *tmpd = mmap(NULL, len_f, PROT_READ, MAP_SHARED, f, 0);
    if (tmpd == MAP_FAILED)
        return -1;
    close(f);
    int rc = madvise(tmpd, len_f, MADV_RANDOM | MADV_DONTDUMP);
    if (rc < 0)
        return -1;
    data_ptr = (u8 *)tmpd;
    data_len = len_f;
    return 0;
}

int MunmapFile(u8 *&data_ptr, SizeT &data_len) {
    if (data_ptr != nullptr) {
        int rc = munmap(data_ptr, data_len);
        if (rc < 0)
            return -1;
        data_ptr = nullptr;
        data_len = 0;
    }
    return 0;
}

// unescape a string, refers to https://en.cppreference.com/w/cpp/language/escape
String unescape(const String &escaped) {
    std::stringstream result;
    for (std::size_t i = 0; i < escaped.size(); ++i) {
        if (escaped[i] == '\\' && i + 1 < escaped.size()) {
            switch (escaped[i + 1]) {
                case '\'':
                    result << '\''; // single quote
                    break;
                case '"':
                    result << '"'; // double quote
                    break;
                case '?':
                    result << '\?'; // question mark
                    break;
                case '\\':
                    result << '\\'; // backslash
                    break;
                case 'a':
                    result << '\a'; // audible bell
                    break;
                case 'b':
                    result << '\b'; // backspace
                    break;
                case 'f':
                    result << '\f'; // 	form feed - new page
                    break;
                case 'n':
                    result << '\n'; // line feed - new line
                    break;
                case 'r':
                    result << '\r'; // carriage return
                    break;
                case 't':
                    result << '\t'; // horizontal tab
                    break;
                case 'v':
                    result << '\v'; // vertical tab
                    break;
                // Add more cases for other escape sequences as needed
                default:
                    result << escaped[i + 1];
                    break;
            }
            ++i; // Skip the next character as it was part of an escape sequence
        } else {
            result << escaped[i];
        }
    }
    return result.str();
}

int main(int argc, char *argv[]) {
    CLI::App app{
        "fst 0.1\nZhichang Yu <yuzhichang@gmail.com>\nA command line tool for building, searching and inspecting FSTs. A simplified C++ "
        "reimplementing of "
        "[BurntSushi/fst](https://github.com/BurntSushi/fst) library. The FST file format should be binary compatible with the original one."};
    app.require_subcommand(1);

    CLI::App *app_map = app.add_subcommand(
        "map",
        "Creates an ordered map backed by a finite state transducer.\nThe input to this command should be a CSV file with exactly two columns and no "
        "headers. The first column should be the key and the second column should be a value that can be interpreted as an unsigned 64 bit integer. "
        "The input shall be already sorted in lexicographic order by the key. Otherwise this will return an error when it sees an out-of-order key.");
    bool force;
    String delimiter, fp_keys, fp_fst;
    app_map->add_flag("-f,--force", force, "Overwrites the output if the destination file already exists.");
    app_map->add_option("-d,--delimiter", delimiter, "The delimiter used in the CSV file to separate key and value in.")->default_val(",");
    app_map->add_option("keys", fp_keys, "A file containing a key per line.")->required();
    app_map->add_option("fst", fp_fst, "The destination file path to write the FST.")->required();

    CLI::App *app_range =
        app.add_subcommand("range",
                           "Issues a range query against the given transducer.\nA range query returns all search results within a particular "
                           "range.\nIf neither the start or the end of the range is specified, then all entries in the transducer are shown.");
    bool show_outputs;
    String start, end;
    app_range->add_flag("-o,--outputs", show_outputs, "When set, output values are shown as CSV data.");
    app_range->add_option("-e,--end", end, "Only show results less than or equal to this.");
    app_range->add_option("-s,--start", start, "Only show results greater than or equal to this.");
    app_range->add_option("fst", fp_fst, "The FST to run a range query against.")->required();

    CLI::App *app_verify = app.add_subcommand("verify",
                                              "Performs verification on the FST to check its integrity. This works by computing a checksum of the "
                                              "FST's underlying data and comparing it to an expected checksum. If the checksums do not match, then "
                                              "it's likely that the FST is corrupt in some fashion and must be re-generated.\n");
    app_verify->add_option("fst", fp_fst, "The FST to verify.")->required();

    CLI::App *app_bench = app.add_subcommand("benchmark", "Query benchmark with given keys against the given transducer.");
    app_bench->add_option("-d,--delimiter", delimiter, "The delimiter used in the CSV file to separate key and value in.")->default_val(",");
    app_bench->add_option("keys", fp_keys, "A file containing a key per line.")->required();
    app_bench->add_option("fst", fp_fst, "The FST to run query benchmark against.")->required();

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return app.exit(e);
    }
    if (app_map->parsed()) {
        profiler.Begin();
        std::filesystem::path output_path(fp_fst);
        output_path = std::filesystem::absolute(output_path);
        std::filesystem::path output_dir = output_path.parent_path();
        std::filesystem::create_directories(output_dir);
        if (std::filesystem::exists(output_path) && !force) {
            return app.exit(CLI::FileError("Output file already exists. Use --force to overwrite."));
        }
        std::ofstream ofs(output_path.c_str(), std::ios::binary | std::ios::trunc);
        if (!ofs.is_open()) {
            return app.exit(CLI::FileError("Failed to open output file: " + output_path.string()));
        }
        OstreamWriter wtr(ofs);
        FstBuilder builder(wtr);

        std::ifstream ifs(fp_keys);
        String line;
        delimiter = unescape(delimiter);
        SizeT del_len = delimiter.length();
        while (std::getline(ifs, line)) {
            auto pos = line.find(delimiter);
            if (pos == String::npos) {
                return app.exit(
                    CLI::FileError("Delimiter [" + delimiter + "]" + FormatBytes((u8 *)delimiter.c_str(), del_len) + " not found in line: " + line));
            }
            String key = line.substr(0, pos);
            String value = line.substr(pos + del_len);
            builder.Insert((u8 *)key.c_str(), key.length(), std::stoull(value));
        }
        builder.Finish();
        ofs.close();
        profiler.End();
        std::cout << "FST written to " << output_path << std::endl;
        std::cout << "time cost: " << profiler.Elapsed() / 1000000 << " ms" << std::endl;
    } else if (app_range->parsed()) {
        profiler.Begin();
        u8 *data_ptr = nullptr;
        SizeT data_len = 0;
        if (MmapFile(fp_fst, data_ptr, data_len) < 0) {
            return app.exit(CLI::FileError("Failed to mmap file: " + fp_fst));
        }
        Fst f(data_ptr, data_len);
        Bound min, max;
        if (!start.empty()) {
            min = Bound(Bound::kIncluded, (u8 *)start.c_str(), start.length());
        }
        if (!end.empty()) {
            max = Bound(Bound::kIncluded, (u8 *)end.c_str(), end.length());
        }
        FstStream it(f, min, max);
        Vector<u8> key;
        u64 val;
        while (it.Next(key, val)) {
            String str_key(key.data(), key.data() + key.size());
            if (show_outputs) {
                std::cout << str_key << "," << val << std::endl;
            } else {
                std::cout << str_key << std::endl;
            }
        }
        MunmapFile(data_ptr, data_len);
        profiler.End();
        std::cout << "time cost: " << profiler.Elapsed() / 1000000 << " ms" << std::endl;
    } else if (app_verify->parsed()) {
        profiler.Begin();
        u8 *data_ptr = nullptr;
        SizeT data_len = 0;
        if (MmapFile(fp_fst, data_ptr, data_len) < 0) {
            return app.exit(CLI::FileError("Failed to mmap file: " + fp_fst));
        }
        Fst f(data_ptr, data_len);
        f.Verify();
        MunmapFile(data_ptr, data_len);
        std::cout << "FST checksum is correct: " << fp_fst << std::endl;
        profiler.End();
        std::cout << "time cost: " << profiler.Elapsed() / 1000000 << " ms" << std::endl;
    } else if (app_bench->parsed()) {
        Vector<Pair<String, SizeT>> queries;
        std::ifstream ifs(fp_keys);
        String line;
        delimiter = unescape(delimiter);
        SizeT del_len = delimiter.length();
        while (std::getline(ifs, line)) {
            auto pos = line.find(delimiter);
            if (pos == String::npos) {
                return app.exit(
                    CLI::FileError("Delimiter [" + delimiter + "]" + FormatBytes((u8 *)delimiter.c_str(), del_len) + " not found in line: " + line));
            }
            String key = line.substr(0, pos);
            String value = line.substr(pos + del_len);
            queries.emplace_back(key, std::stoull(value));
        }

        profiler.Begin();
        u8 *data_ptr = nullptr;
        SizeT data_len = 0;
        if (MmapFile(fp_fst, data_ptr, data_len) < 0) {
            return app.exit(CLI::FileError("Failed to mmap file: " + fp_fst));
        }
        Fst f(data_ptr, data_len);
        SizeT cnt = 0;
        bool running = true;
        while (running) {
            for (auto &[key, val] : queries) {
                Optional<u64> res = f.Get((u8 *)key.c_str(), key.length());
                if (res.has_value()) {
                    if (res.value() != val) {
                        // -1 means expecting missing
                        String exp = val == SizeT(-1) ? "missing" : std::to_string(val);
                        return app.exit(CLI::FileError("Value of key " + key + " mismatch, expect " + exp + ", got " + std::to_string(res.value())));
                    }
                } else {
                    if (val != SizeT(-1)) {
                        return app.exit(CLI::FileError("Value of key " + key + " mismatch, expect " + std::to_string(val) + ", got missing"));
                    }
                }
                cnt++;
                if ((cnt & 0xFFFF) == 0) {
                    if (profiler.Elapsed() >= 60LL * 1000000000LL) {
                        // ensure benchmark last at least 60 seconds
                        running = false;
                        break;
                    }
                }
            }
        }
        MunmapFile(data_ptr, data_len);
        profiler.End();
        std::cout << "query count: " << cnt << std::endl;
        std::cout << "time cost: " << profiler.Elapsed() / 1000000 << " ms" << std::endl;
        std::cout << "qps: " << cnt / (profiler.Elapsed() / 1000000000) << std::endl;
    } else {
        return app.exit(CLI::RequiresError("fst", "a subcommand of [map, range, verify]"));
    }

    return 0;
}
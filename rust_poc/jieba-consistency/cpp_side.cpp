// PoC G (C++ side): cppjieba with Infinity's dictionaries.
// Mirrors rust_poc/jieba-consistency/src/main.rs output format:
//   <mode> \t <line_no> \t <space-joined words>
// Build:
//   clang++ -std=c++17 -O2 -I <repo>/third_party/cppjieba/include \
//           -I <repo>/third_party/cppjieba/deps/limonp/include \
//           cpp_side.cpp -o cpp_side -lpthread
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "cppjieba/Jieba.hpp"

int main(int argc, char **argv) {
    std::string dict_dir = argc > 1 ? argv[1] : "/usr/share/infinity/resource/jieba/dict";
    std::string sf = argc > 2 ? argv[2] : "sentences.txt";

    cppjieba::Jieba jieba(dict_dir + "/jieba.dict.utf8", dict_dir + "/hmm_model.utf8", dict_dir + "/user.dict.utf8",
                         dict_dir + "/idf.utf8", dict_dir + "/stop_words.utf8");

    std::ifstream in(sf);
    std::string line;
    int i = 0;
    while (std::getline(in, line)) {
        if (line.empty()) {
            ++i;
            continue;
        }
        for (int m = 0; m < 2; ++m) {
            bool hmm = (m == 1);
            std::vector<std::string> words;
            jieba.Cut(line, words, hmm);
            std::cout << (hmm ? "hmm" : "mp") << "\t" << i << "\t";
            for (size_t k = 0; k < words.size(); ++k) {
                if (k)
                    std::cout << " ";
                std::cout << words[k];
            }
            std::cout << "\n";
        }
        ++i;
    }
    return 0;
}

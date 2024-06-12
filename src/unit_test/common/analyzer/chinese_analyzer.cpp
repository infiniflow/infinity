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

#include "unit_test/base_test.h"

import stl;
import term;
import analyzer;
import chinese_analyzer;
using namespace infinity;

namespace fs = std::filesystem;

class ChineseAnalyzerTest : public BaseTest {};

TEST_F(ChineseAnalyzerTest, test1) {
    // Get the path to the executable using the /proc/self/exe symlink
    fs::path executablePath = "/proc/self/exe";
    std::error_code ec;
    // Resolve the symlink to get the actual path
    executablePath = fs::canonical(executablePath, ec);
    if (ec) {
        std::cerr << "Error resolving the path: " << executablePath << " " << ec.message() << std::endl;
        return;
    }
    std::cerr << "/proc/self/exe: " << executablePath << std::endl;

    fs::path ROOT_PATH = executablePath.parent_path().parent_path().parent_path().parent_path() / "resource";
    std::cerr << "ROOT_PATH: " << ROOT_PATH << std::endl;

    if (!fs::exists(ROOT_PATH)) {
        std::cerr << "Resource directory doesn't exist: " << ROOT_PATH << std::endl;
        return;
    }

    ChineseAnalyzer analyzer(ROOT_PATH.string());
    analyzer.Load();
    Vector<String> queries = {
        "graphic card",
        "graphics card",
        "南京市长江大桥",
        "小明硕士毕业于中国科学院计算所，后在日本京都大学深造",
        "会徽整体形似运动中的羽毛球，球头绑带部分演化为“城墙”的图形元素，极具南京的地域特征，凸显出举办地的历史底蕴和人文气息。尾羽部分图形则巧妙融入"
        "了举办年份“2018”和南京的首字母“NJ”，结合中国传统书法笔触的表现形式，传递出羽毛球运动的速度感。会徽红黑配色鲜艳明快，契合了体育运动的活力与朝"
        "气[3]"
        "2018年世界羽毛球锦标赛吉祥物南京羽毛球世锦赛吉祥物2018年道达尔羽毛球世锦赛吉祥物在南京发布。造型简洁、形态生动、富有亲和力的“羽宝”拔得头筹，"
        "成为2018年世界羽毛球锦标赛吉祥物。比赛将于7月30日在宁举行，赛程7天，预计近340名顶尖运动员参赛。吉祥物“羽宝”头部由羽毛球外形变化而来，手持球"
        "拍，拟人化的设计再现了羽毛球运动员比赛时的接击球动作，胸前佩戴的梅花造型的金牌，代表着在南京举办的世锦赛将向世界献上精彩的羽毛球盛宴。同时黄"
        "蓝两色为主色调，在视觉冲击中体现了羽毛球运动动静转换的速度感和竞技魅力[6]"
        "2018年世界羽毛球锦标赛抽签结果7月17日，2018年南京羽毛球世锦赛抽签出炉。男单中国获得满额席位，石宇奇、谌龙、林丹和黄宇翔全部被分到了上半区"
        "。"};

    ChineseAnalyzer analyzer2(analyzer);
    analyzer2.SetCutGrain(CutGrain::kFine);

    for (auto &query : queries) {
        TermList term_list;
        analyzer.Analyze(query, term_list);
        std::cout << "Text #" << query << "# parsed as:" << std::endl;
        for (unsigned i = 0; i < term_list.size(); ++i) {
            std::cout << "\t" << i << "#" << term_list[i].text_ << "@" << term_list[i].word_offset_ << "#";
        }
        std::cout << std::endl;
        TermList term_list2;
        analyzer2.Analyze(query, term_list2);
        for (unsigned i = 0; i < term_list2.size(); ++i) {
            std::cout << "\t" << i << "#" << term_list2[i].text_ << "@" << term_list2[i].word_offset_ << "#";
        }
        std::cout << std::endl;
    }
}

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
#include "gtest/gtest.h"
#include <filesystem>
#include <vector>

import base_test;

import stl;
import term;
import analyzer;
import rag_analyzer;
import darts_trie;
import darts;
import lemmatizer;

using namespace infinity;

namespace fs = std::filesystem;

class RAGAnalyzerTest : public BaseTest {};

TEST_F(RAGAnalyzerTest, test1) {
    // Get the path to the executable using the /proc/self/exe symlink
    fs::path executablePath = "/proc/self/exe";
    std::error_code ec;
    // Resolve the symlink to get the actual path
    executablePath = fs::canonical(executablePath, ec);
    if (ec) {
        std::cerr << "Error resolving the path: " << executablePath << " " << ec.message() << std::endl;
        return;
    }

    fs::path ROOT_PATH = executablePath.parent_path().parent_path().parent_path().parent_path() / "resource";

    if (!fs::exists(ROOT_PATH)) {
        std::cerr << "Resource directory doesn't exist: " << ROOT_PATH << std::endl;
        return;
    }
#if 1

    RAGAnalyzer analyzer(ROOT_PATH.string());
    analyzer.Load();

    Vector<String> queries = {
        R"#(哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈)#",
        R"#(公开征求意见稿提出，境外投资者可使用自有人民币或外汇投资。使用外汇投资的，可通过债券持有人在香港人民币业务清算行及香港地区经批准可进入境内银行间外汇市场进行交易的境外人民币业务参加行（以下统称香港结算行）办理外汇资金兑换。香港结算行由此所产生的头寸可到境内银行间外汇市场平盘。使用外汇投资的，在其投资的债券到期或卖出后，原则上应兑换回外汇。)#",
        R"#(多校划片就是一个小区对应多个小学初中，让买了学区房的家庭也不确定到底能上哪个学校。目的是通过这种方式为学区房降温，把就近入学落到实处。南京市长江大桥)#",
        R"#(实际上当时他们已经将业务中心偏移到安全部门和针对政府企业的部门 Scripts are compiled and cached aaaaaaaaa)#",
        R"#(虽然我不怎么玩)#",
        R"#(蓝月亮如何在外资夹击中生存,那是全宇宙最有意思的)#",
        R"#(涡轮增压发动机num最大功率,不像别的共享买车锁电子化的手段,我们接过来是否有意义,黄黄爱美食,不过，今天阿奇要讲到的这家农贸市场，说实话，还真蛮有特色的！不仅环境好，还打出了)#",
        R"#(这周日你去吗？这周日你有空吗？)#",
        R"#(Unity3D开发经验 测试开发工程师 c++双11双11 985 211)#",
        R"#(The encoder structure we selected is VitDet [17] (base version with about 80M parameters) due to its local attention can greatly reduce the computational cost of high-resolution images. We follow the Vary-tiny setting [46] to design the last two layers of the encoder, which will transfer a 1024×1024×3 input image to 256×1024 image tokens. Then, these image tokens are projected into language model (OPT-125M [53]) dimension via a 1024×768 linear layer. Unlike the Vary encoder which only focuses on a single document task under a relatively unitary input shape, we incorporated natural scenes and cropped slices during our pre-training. In the pre-processing stage, images of each shape are directly resized to 1024×1024 squares, as square shapes can be used to adapt to images of various aspect ratios with a compromise.)#"
        R"#(数据分析项目经理|数据分析挖掘|数据分析方向|商品数据分析|搜索数据分析 sql python hive tableau Cocos2d-)#"};

    for (auto &query : queries) { // TermList term_list;
        // analyzer.Analyze(query, term_list);
        Vector<String> tokens;
        String ret = analyzer.Tokenize(query, tokens);
        ret = analyzer.FineGrainedTokenize(ret);
        std::cout << ret << std::endl;
        //         for (unsigned i = 0; i < term_list.size(); ++i) {
        //             std::cout << "\t" << i << "#" << term_list[i].text_ << "@" << term_list[i].word_offset_ << "#";
        //         }
        //         std::cout << std::endl;
    }
#endif
}

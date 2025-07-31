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

#ifdef CI
#include "gtest/gtest.h"
#include <filesystem>
#include <vector>
import infinity_core;
import base_test;
#else
module;

#include "gtest/gtest.h"
#include <filesystem>
#include <vector>

module infinity_core:ut.ik_analyzer;

import :ut.base_test;
import :stl;
import :term;
import :analyzer;
import :ik_analyzer;
#endif

using namespace infinity;

namespace fs = std::filesystem;

class IKAnalyzerTest : public BaseTest {};

TEST_F(IKAnalyzerTest, test1) {
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
#if 0    
    IKAnalyzer analyzer(ROOT_PATH.string());
    analyzer.Load();
    // analyzer.SetFineGrained(true);
    Vector<String> queries = {
        // R"#(哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈)#",
        // R"#(公开征求意见稿提出，境外投资者可使用自有人民币或外汇投资。使用外汇投资的，可通过债券持有人在香港人民币业务清算行及香港地区经批准可进入境内银行间外汇市场进行交易的境外人民币业务参加行（以下统称香港结算行）办理外汇资金兑换。香港结算行由此所产生的头寸可到境内银行间外汇市场平盘。使用外汇投资的，在其投资的债券到期或卖出后，原则上应兑换回外汇。)#",
        // R"#(多校划片就是一个小区对应多个小学初中，让买了学区房的家庭也不确定到底能上哪个学校。目的是通过这种方式为学区房降温，把就近入学落到实处。南京市长江大桥)#",
        // R"#(实际上当时他们已经将业务中心偏移到安全部门和针对政府企业的部门 Scripts are compiled and cached aaaaaaaaa)#",
        // R"#(虽然我不怎么玩)#",
        R"#(蓝月亮如何在外资夹击中生存,那是全宇宙最有意思的)#",
        // R"#(涡轮增压发动机num最大功率,不像别的共享买车锁电子化的手段,我们接过来是否有意义,黄黄爱美食,不过，今天阿奇要讲到的这家农贸市场，说实话，还真蛮有特色的！不仅环境好，还打出了)#",
        // R"#(这周日你去吗？这周日你有空吗？)#",
        // R"#(Unity3D开发经验 测试开发工程师 c++双11双11 985 211)#",

        // R"#(The encoder structure we selected is VitDet [17] (base version with about 80M parameters) due to its local attention can greatly
        // reducethe computational cost of high-resolution images. We follow the Vary-tiny setting [46] to design the last two layers of the encoder,
        // which will transfer a 1024×1024×3 input image to 256×1024 image tokens. Then, these image tokens are projected into language model
        //(OPT-125M [53]) dimension via a 1024×768 linear layer. Unlike the Vary encoder which only focuses on a single document task under a
        // relatively unitary input shape, we incorporated natural scenes and cropped slices during our pre-training. In the pre-processing stage,
        // images of each shape are directly resized to 1024×1024 squares, as square shapes can be used to adapt to images of various aspect ratios
        // with a compromise.)#",

        // R"#(耶律阿保机将其更名为\"东丹国,并放置由耶律倍作为东丹国的统治者,同时还封爵耶律倍为\"人皇王。阿保机本人的尊号是\"天天子,皇后述律平的尊号是\"地皇后,耶律倍被封为\"人皇王,如许就进一步确立了他的职位在天子、皇后之下,其余全部人之上。同年9月,耶律阿保机在从东丹国凯旅回朝的途中病逝。后契丹的政局就产生了猛烈变更。阿保机生前固然对秉承人问题有过夷由,但他终照旧是对峙,让耶律倍成为来日的天子。不过,阿保机的妃耦、皇后述律平却更偏心二儿子。述律平固然是个女人,但却是契丹政权中的名实相符的二号人物,是以她的决意,天然很是关键重要。述律平不赞许宗子耶律倍秉承皇位。她尽其所能鞭策耶律倍摒弃秉承权,并以种种\"捏词\"拔除支撑耶律倍的契丹贵族。此中有很多人乃至被强行拉去\"服待\"殉葬先帝了。不过,耶律倍事实已做了十年皇太子,在野廷中的支撑者也良多,是以,他也有很强的政治血本,能够同母亲与二弟相对抗。在经由了一年多时分的明枪暗箭后,身心俱疲的耶律倍决意摒弃权柄,支撑他的弟弟登位称帝。公元927年12月11日,耶律德光正式登位,成为契丹汗青上的第二位天子,史称辽太宗。耶律倍主动摒弃了权柄,不过却并无换来安宁。)#",
        // R"#(世界遗产委员会一般指联合国教科文组织世界遗产委员会联合国教科文组织世界遗产同义词)#",
    };

    for (auto &query : queries) {
        TermList term_list;
        analyzer.Analyze(query, term_list);
        // String ret = analyzer.Tokenize(query);
        // ret = analyzer.FineGrainedTokenize(ret);
        // std::cout << ret << std::endl;
        for (unsigned i = 0; i < term_list.size(); ++i) {
            std::cout << term_list[i].text_ << " ";
        }
        std::cout << std::endl;
    }
#endif
}

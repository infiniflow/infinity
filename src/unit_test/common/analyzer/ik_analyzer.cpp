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
import ik_analyzer;

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

        // R"#(重大疫情防控和突发公共卫生事件监测预警和应急处置体系全面建立，救治和应对能力显著提升，管理体制和运行机制进一步理顺。建成能够有效应对重大疫情和突发公共卫生事件、满足公共安全需要的强大公共卫生体系。（二）加快建设优质均衡医疗服务体系。积极支持川北医学院附属医院和南充市中心医院参与国家级医学中心、国家区域医疗中心建设，大力支持川北医学院附属医院建设省级医学中心、南充市中心医院建设省区域医疗中心。推进县级医院能力提升和创等达标。完善乡村卫生服务体系，改善乡镇卫生院、社区卫生服务中心服务条件和服务能力，满足基层人民群众健康服务需求。（三）初步建立特色鲜明的中医药服务体系。积极整合中医优质医疗资源，独立设置市级中医医院，建强县级中医医院，推动基层中医药服务网络进一步完善，其他医疗机构中医药科室设置进一步健全，补齐补强中医药服务体系短板。推动“三名工程”在中医医疗服务体系建设中齐头并进，形成一批中医“龙头医院”、“骨干医院”、“中医名科”“名老中医”。（四）逐步建立全方位全生命周期健康服务体系。加强医防融合体系建设，推动公共卫生服务优质供给。)#",
        // R"#(原文地址:名师最激发潜能的课堂提问艺术作者:西双小学李转莲名师最激发潜能的课堂提问艺术一、高效课堂的提问原则1.提问一定要有效分层设问,有效切入有效提问、有效参与能促进师生问的互动,及时得到教学信息反馈,不断提出问题、解决问题,从而增强课堂教学的实效性。2.提问应把握好度准确认知,难易适度课堂提问是师生间信息双向交流的过程,把握提问的适度性能激发学生们的兴趣,启发他们的思维,提高课堂教学效率。3.对课堂答问多做鼓励性评价赏识鼓励,激发自信教师们对学生们的回答应及时给予评价,使不同层次的学生都对本学科产生兴趣,让学生们积极主动地学习知识。4.提问要坚持平等性平等对待,全面参与课堂提问对象过于集中,会对其他学生产生不良的心理影响,应该尽量使每个学生都有回答问题的机会。二、高效课堂的提问切入点1.在知识的重点处提问印象最深重点设计,举一反三通过在知识的重点处提问,让学生们不仅理解了知识的内容,更对知识的重点处加深了印象,极大地提高了学生们的学习质量。)#",
        // R"#(要高质高效加快生产。企业要锚定全年目标,抢抓生产黄金期,开足马力“加速跑”,掀起生产攻坚热潮。在华美特,李胜走进生产车间,查看生产经营状况,详细了解企业产品规划、生产周期、市场订单等情况。他鼓励企业要坚定发展信心。围绕全年目标不动摇,大干一百天,冲刺四季度,进一步强化时不我待、只争朝夕的紧迫感,压实责任,咬定目标,强化使命担当,奋力完成全年目标任务。要适应新发展格局。在构建新发展格局上走在前是习近平总书记赋予我省的重大使命,要充分利用现有技术优势、品牌优势,把握发展先机,着力拓宽国内国际两个市场,不断推进企业高质量发展。要坚持绿色发展、安全生产。进一步加大科技投入,强化技术合作,根据市场消费趋势推进新产品研发,提高产品附加值。严守环保红线,压紧压实主体责任,放大发展目标,守牢安全环保生命线。调研中,李胜强调,安全生产工作宁可百日紧,不可一日松。要深入贯彻落实习近平总书记关于安全生产工作的重要指示精神,坚持人民至上、生命至上,树牢安全发展理念,深刻汲取各类生产安全事故教训,拧紧安全生产责任链条,切实维护人民群众生命财产安全。)#",
        // R"#(을내밀었더니보나마나그건사치라니요즘나살찐것같다는말에시큰둥하게그런것같다해살빼기위해참고있는데계속옆에서뭘먹네요새로산옷을입어도몰라요긴머릴잘라도몰라요널뚫어져라쳐다봐도몰라몰라내가왜화내는지몰라요정말왜그런지몰라요우~늑대들은아무것도몰라요머릴바꾸고화장을하고예쁜척하는것도너때문이야바보야말한마디가참어렵네요여잔다그래요넌왜내맘을몰라우~기념일도몰라우~말해줘도몰라StupidStupid아무것도넌몰라우~여자맘을몰라우~다른건다모르면서화장안한건또아네요늑대들은몰라요남잔말안해주면몰라요여자맘을몰라요뭘원하는지몰라몰라화내지말아요이러다큰일나요여자들은남자맘을몰라요)#",
        // R"#(耶律阿保机将其更名为\"东丹国,并放置由耶律倍作为东丹国的统治者,同时还封爵耶律倍为\"人皇王。阿保机本人的尊号是\"天天子,皇后述律平的尊号是\"地皇后,耶律倍被封为\"人皇王,如许就进一步确立了他的职位在天子、皇后之下,其余全部人之上。同年9月,耶律阿保机在从东丹国凯旅回朝的途中病逝。后契丹的政局就产生了猛烈变更。阿保机生前固然对秉承人问题有过夷由,但他终照旧是对峙,让耶律倍成为来日的天子。不过,阿保机的妃耦、皇后述律平却更偏心二儿子。述律平固然是个女人,但却是契丹政权中的名实相符的二号人物,是以她的决意,天然很是关键重要。述律平不赞许宗子耶律倍秉承皇位。她尽其所能鞭策耶律倍摒弃秉承权,并以种种\"捏词\"拔除支撑耶律倍的契丹贵族。此中有很多人乃至被强行拉去\"服待\"殉葬先帝了。不过,耶律倍事实已做了十年皇太子,在野廷中的支撑者也良多,是以,他也有很强的政治血本,能够同母亲与二弟相对抗。在经由了一年多时分的明枪暗箭后,身心俱疲的耶律倍决意摒弃权柄,支撑他的弟弟登位称帝。公元927年12月11日,耶律德光正式登位,成为契丹汗青上的第二位天子,史称辽太宗。耶律倍主动摒弃了权柄,不过却并无换来安宁。)#",
        // R"#(世界遗产委员会一般指联合国教科文组织世界遗产委员会联合国教科文组织世界遗产同义词)#",
        // R"#(州长：龙晓华副州长：向恩明、向清平、何益群、李平、麻超、王学武、刘珍瑜州委常委、州人民政府副州长挂职：房卫、董怀敏秘书长：包太洋湘西土家族苗族自治州人民政府办公室湘西土家族苗族自治州发展和改革委员会湘西土家族苗族自治州财政局湘西土家族苗族自治州经信委湘西土家族苗族自治州教育局湘西土家族苗族自治州科技局湘西土家族苗族自治州民委湘西土家族苗族自治州公安局湘西土家族苗族自治州监察局湘西土家族苗族自治州民政局湘西土家族苗族自治州司法局湘西土家族苗族自治州人力资源和社会保障局湘西土家族苗族自治州国土资源局湘西土家族苗族自治州住建局湘西土家族苗族自治州交通运输局湘西土家族苗族自治州水利局湘西土家族苗族自治州农业局湘西土家族苗族自治州林业局湘西土家族苗族自治州商务局湘西土家族苗族自治州文广新局湘西土家族苗族自治州卫生局湘西土家族苗族自治州计划生育委员会湘西土家族苗族自治州审计局湘西土家族苗族自治州环保局湘西土家族苗族自治州统计局湘西土家族苗族自治州物价局湘西土家族苗族自治州安监局湘西土家族苗族自治州旅游局湘西土家族苗族自治州食品药品监督局湘西土家族苗族自治州体育局湘西土家族苗族自治州无线电管理处湘西)#",
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
}
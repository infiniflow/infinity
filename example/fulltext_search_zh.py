# Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""
This example is to connect local infinity instance, create table, insert data, search the data which Chinese words
"""

import infinity
import sys

"""
Checkout https://github.com/infiniflow/resource.git under /var/infinity (defined by 'resource_dir' of config file). The jieba dict is
/var/infinity/resource/jieba/dict/jieba.dict.utf8
"""

try:
    # open a local directory to store the data
    infinity_instance = infinity.connect("/var/infinity")

    # connect to server with 127.0.0.1
    # infinity_instance = infinity.connect(infinity.common.LOCAL_HOST)

    # 'default_db' is the default database
    db_instance = infinity_instance.get_database("default_db")

    # Drop my_table if it already exists
    db_instance.drop_table("my_table", infinity.common.ConflictType.Ignore)

    # Create a table named "my_table"
    table_instance = db_instance.create_table("my_table", {
        "num": {"type": "integer"},
        "body": {"type": "varchar"},
        "vec": {"type": "vector, 4, float"},
    })

    # Insert 3 rows of data into the 'my_table'
    table_instance.insert(
        [
            {
                "num": 1,
                "body": r"据Wccftech报道，苹果正在开发一种定制芯片，旨在为人工智能（AI）服务器提供动力。暂时还不清楚这款芯片的具体规格，以及具体的实现目标。传闻苹果已选择台积电（TSMC）的3nm制程节点来制造这款芯片，预计2025年下半年量产。如果按照量产时间和台积电的半导体工艺进度，那么对应的很可能是N3E工艺。",
                "vec": [1.0, 1.2, 0.8, 0.9],
            },
            {
                "num": 2,
                "body": r"两个月前有报道称，苹果已正式放弃了努力超过十年、投下海量资金的“泰坦计划（Project Titan）”电动车项目。苹果随后解散了大约2000人的开发团队，各人会被分配到其他地方，其中一个很重要的去处就是人工智能部门。有传言称，苹果已经将注意力转向生成式AI，希望能够为业务找到新的增长动力。",
                "vec": [1.2, 0.8, 0.9, 1.0],
            },
            {
                "num": 3,
                "body": r"爬行动物是一类包括蛇、蜥蜴、龟鳖和鳄鱼等的脊椎动物。它们的特点是体表覆盖着角质鳞片，而不是羽毛。羽毛是鸟类的特征，鸟类是从爬行动物中的某一类演化而来的，但它们已经发展出了独特的特征，包括羽毛。因此，爬行动物没有羽毛。",
                "vec": [0.8, 0.9, 1.0, 1.2],
            },
            {
                "num": 4,
                "body": r"会徽整体形似运动中的羽毛球，球头绑带部分演化为“城墙”的图形元素，极具南京的地域特征，凸显出举办地的历史底蕴和人文气息。尾羽部分图形则巧妙融入了举办年份“2018”和南京的首字母“NJ”，结合中国传统书法笔触的表现形式，传递出羽毛球运动的速度感。会徽红黑配色鲜艳明快，契合了体育运动的活力与朝气[3]2018年世界羽毛球锦标赛吉祥物南京羽毛球世锦赛吉祥物2018年道达尔羽毛球世锦赛吉祥物在南京发布。造型简洁、形态生动、富有亲和力的“羽宝”拔得头筹，成为2018年世界羽毛球锦标赛吉祥物。比赛将于7月30日在宁举行，赛程7天，预计近340名顶尖运动员参赛。吉祥物“羽宝”头部由羽毛球外形变化而来，手持球拍，拟人化的设计再现了羽毛球运动员比赛时的接击球动作，胸前佩戴的梅花造型的金牌，代表着在南京举办的世锦赛将向世界献上精彩的羽毛球盛宴。同时黄蓝两色为主色调，在视觉冲击中体现了羽毛球运动动静转换的速度感和竞技魅力[6]2018年世界羽毛球锦标赛抽签结果7月17日，2018年南京羽毛球世锦赛抽签出炉。男单中国获得满额席位，石宇奇、谌龙、林丹和黄宇翔全部被分到了上半区。",
                "vec": [0.9, 1.0, 1.2, 0.8],
            },
            {
                "num": 5,
                "body": r"周末我和朋友一起去“电子城”，想挑选一些新的“电脑配件”。那里有各种各样的“hardware”，如“motherboard”、“graphics card”等。我们还看到了一些很“awesome”的“peripheral devices”，像“keyboard”和“mouse”。我朋友说他需要一个新的“power supply”，而我则对那些“high-tech”的“storage devices”比较感兴趣。逛了一会儿后，我们都买到了自己心仪的东西，然后就“happily”回家了。",
                "vec": [1.0, 0.9, 0.8, 0.9],
            },
            {
                "num": 6,
                "body": r"便携式计算机  Model Name型号：ThinkBook 16 G5+ ARP  输入电压/电流：20V=5A  CMIT ID：2023AP123456  MO: DS-K3AJ303/Dm140",
                "vec": [0.9, 0.8, 0.9, 1.0],
            },
            {
                "num": 7,
                "body": r"Office for Harmful Blooms",
                "vec": [4.0, 4.2, 4.3, 4.5],
            },
            {
                "num": 8,
                "body": r"A Bloom filter is a space-efficient probabilistic data structure, conceived by Burton Howard Bloom in 1970, that is used to test whether an element is a member of a set.",
                "vec": [4.0, 4.2, 4.3, 4.5],
            },
        ]
    )

    # Create index on varchar column for full-text search and multiple way fusion.
    res = table_instance.create_index(
        "my_index",
        infinity.index.IndexInfo("body", infinity.index.IndexType.FullText, {"ANALYZER": "chinese"}), infinity.common.ConflictType.Error)

    questions = [
        r"芯片",  # single term
        r"苹果",  # single term
        r"羽毛",  # single term
        r"羽毛球",  # single term
        r'"羽毛球锦标赛"',  # phrase: adjacent multiple terms
        r"2018年世界羽毛球锦标赛在哪个城市举办？",  # OR multiple terms
        r"high\-tech",  # Escape reserved character '-'
        r'"high tech"',  # phrase: adjacent multiple terms
        r'"high-tech"',  # phrase: adjacent multiple terms
        r"graphics card",  # OR multiple terms
        r'"graphics card"',  # phrase: adjacent multiple terms
        r'"DS-K3AJ303/Dm140"',  # phrase: adjacent multiple terms
        r"Bloom filter",  # OR multiple terms
        r'"Bloom filter"',  # phrase: adjacent multiple terms
    ]
    for question in questions:
        qb_result = table_instance.output(["num", "body", "_score"]).match_text("body", question, 10).to_pl()
        print(f"question: {question}")
        print(qb_result)

    infinity_instance.disconnect()

    print('test done')
    sys.exit(0)
except Exception as e:
    print(str(e))
    sys.exit(-1)
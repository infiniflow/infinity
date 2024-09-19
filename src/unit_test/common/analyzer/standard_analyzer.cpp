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
import base_test;

import stl;
import term;
import stemmer;
import standard_analyzer;
using namespace infinity;

namespace fs = std::filesystem;

class StandardAnalyzerTest : public BaseTest {};

TEST_F(StandardAnalyzerTest, test1) {
    StandardAnalyzer analyzer;
    TermList term_list;
    String input("Boost unit tests.");
    analyzer.InitStemmer(STEM_LANG_ENGLISH);
    analyzer.SetExtractEngStem(false);
    analyzer.Analyze(input, term_list);

    ASSERT_EQ(term_list.size(), 3U);
    ASSERT_EQ(term_list[0].text_, String("boost"));
    ASSERT_EQ(term_list[0].word_offset_, 0U);
    ASSERT_EQ(term_list[1].text_, String("unit"));
    ASSERT_EQ(term_list[1].word_offset_, 1U);
    ASSERT_EQ(term_list[2].text_, String("tests"));
    ASSERT_EQ(term_list[2].word_offset_, 2U);
    //    ASSERT_EQ(term_list[3].text_, PLACE_HOLDER);
    //    ASSERT_EQ(term_list[3].word_offset_, 3U);
}

TEST_F(StandardAnalyzerTest, test2) {
    StandardAnalyzer analyzer;
    TermList term_list;
    String input("Boost unit tests.");
    analyzer.InitStemmer(STEM_LANG_ENGLISH);
    analyzer.SetExtractEngStem(false);
    analyzer.SetCaseSensitive(true, false);
    analyzer.Analyze(input, term_list);

    ASSERT_EQ(term_list.size(), 3U);
    ASSERT_EQ(term_list[0].text_, String("Boost"));
    ASSERT_EQ(term_list[0].word_offset_, 0U);
    ASSERT_EQ(term_list[1].text_, String("unit"));
    ASSERT_EQ(term_list[1].word_offset_, 1U);
    ASSERT_EQ(term_list[2].text_, String("tests"));
    ASSERT_EQ(term_list[2].word_offset_, 2U);
    //    ASSERT_EQ(term_list[3].text_, PLACE_HOLDER);
    //    ASSERT_EQ(term_list[3].word_offset_, 3U);
}

TEST_F(StandardAnalyzerTest, test3) {
    StandardAnalyzer analyzer;
    TermList term_list;
    String input("Boost unit tests.");
    analyzer.InitStemmer(STEM_LANG_ENGLISH);
    analyzer.Analyze(input, term_list);

    ASSERT_EQ(term_list.size(), 4U);
    ASSERT_EQ(term_list[0].text_, String("boost"));
    ASSERT_EQ(term_list[0].word_offset_, 0U);
    ASSERT_EQ(term_list[1].text_, String("unit"));
    ASSERT_EQ(term_list[1].word_offset_, 1U);
    ASSERT_EQ(term_list[2].text_, String("tests"));
    ASSERT_EQ(term_list[2].word_offset_, 2U);
    ASSERT_EQ(term_list[3].text_, String("test"));
    ASSERT_EQ(term_list[3].word_offset_, 2U);
    //    ASSERT_EQ(term_list[3].text_, PLACE_HOLDER);
    //    ASSERT_EQ(term_list[3].word_offset_, 3U);
}

TEST_F(StandardAnalyzerTest, test4) {
    StandardAnalyzer analyzer;
    TermList term_list;
    String input("Boost unit tests.");
    analyzer.InitStemmer(STEM_LANG_ENGLISH);
    analyzer.SetExtractEngStem(false);
    analyzer.SetCaseSensitive(true, true);
    analyzer.Analyze(input, term_list);

    ASSERT_EQ(term_list.size(), 4U);
    ASSERT_EQ(term_list[0].text_, String("Boost"));
    ASSERT_EQ(term_list[0].word_offset_, 0U);
    ASSERT_EQ(term_list[1].text_, String("boost"));
    ASSERT_EQ(term_list[1].word_offset_, 0U);
    ASSERT_EQ(term_list[2].text_, String("unit"));
    ASSERT_EQ(term_list[2].word_offset_, 1U);
    ASSERT_EQ(term_list[3].text_, String("tests"));
    ASSERT_EQ(term_list[3].word_offset_, 2U);
    //    ASSERT_EQ(term_list[3].text_, PLACE_HOLDER);
    //    ASSERT_EQ(term_list[3].word_offset_, 3U);
}

TEST_F(StandardAnalyzerTest, test5) {
    StandardAnalyzer analyzer;
    TermList term_list;
    analyzer.InitStemmer(STEM_LANG_ENGLISH);
    analyzer.SetExtractEngStem(false);
    String input("BoostBoostboostBoostboost unit tests.");
    analyzer.Analyze(input, term_list);

    ASSERT_EQ(term_list.size(), 3U);
    ASSERT_EQ(term_list[0].text_, String("boostboostboostboostboost"));
    ASSERT_EQ(term_list[0].word_offset_, 0U);
    ASSERT_EQ(term_list[1].text_, String("unit"));
    ASSERT_EQ(term_list[1].word_offset_, 1U);
    ASSERT_EQ(term_list[2].text_, String("tests"));
    ASSERT_EQ(term_list[2].word_offset_, 2U);
    //    ASSERT_EQ(term_list[3].text_, PLACE_HOLDER);
    //    ASSERT_EQ(term_list[3].word_offset_, 3U);
}

TEST_F(StandardAnalyzerTest, test6) {
    StandardAnalyzer analyzer;
    analyzer.InitStemmer(STEM_LANG_ENGLISH);
    analyzer.SetExtractEngStem(true);
    analyzer.SetCharOffset(true);
    TermList term_list;
    Vector<String> queries = {
        R"#({{Redirect|Anarchist|the fictional character|Anarchist (comics)}} {{Redirect|Anarchists}} {{Anarchism sidebar}} {{Libertarianism sidebar}}  '''Anarchism''' is generally defined as the [[political philosophy]] which holds the [[state (polity)|state]] to be undesirable, unnecessary, and harmful,<ref name="definition"> {{Cite journal|last=Malatesta|first=Errico|title=Towards Anarchism|journal=MAN!|publisher=International Group of San Francisco|location=Los Angeles|oclc=3930443|url=http://www.marxists.org/archive/malatesta/1930s/xx/toanarchy.htm|authorlink=Errico Malatesta}} {{Cite journal|url=http://www.theglobeandmail.com/servlet/story/RTGAM.20070514.wxlanarchist14/BNStory/lifeWork/home/ |title=Working for The Man |journal=[[The Globe and Mail]] |accessdate=2008-04-14 |last=Agrell |first=Siri |date=2007-05-14}} {{cite web|url=http://www.britannica.com/eb/article-9117285|title=Anarchism|year=2006|work=Encyclopædia Britannica|publisher=Encyclopædia Britannica Premium Service|accessdate=2006-08-29| archiveurl=)#",
        R"#(http://web.archive.org/web/20061214085638/http://www.britannica.com/eb/article-9117285| archivedate= 14 December 2006<!--Added by DASHBot-->}} {{Cite journal|year=2005|title=Anarchism|journal=The Shorter [[Routledge Encyclopedia of Philosophy]]|page=14|quote=Anarchism is the view that a society without the state, or government, is both possible and desirable.}} The following sources cite anarchism as a political philosophy: {{Cite book| last = Mclaughlin | first = Paul | title = Anarchism and Authority | publisher = Ashgate | location = Aldershot | year = 2007 | isbn = 0-7546-6196-2 |page=59}} {{Cite book| last = Johnston | first = R. | title = The Dictionary of Human Geography | publisher = Blackwell Publishers | location = Cambridge | year = 2000 | isbn = 0-631-20561-6 |page=24}}</ref><ref name=slevin>Slevin, Carl. "Anarchism." ''The Concise Oxford Dictionary of Politics''. Ed. Iain McLean and Alistair McMillan. Oxford University Press, 2003.</ref> or alternatively as opposing [[authority]] and)#",
        R"#([[hierarchical organization]] in the conduct of human relations.<ref name="iaf-ifa.org">"The [[International of Anarchist Federations|IAF - IFA]] fights for : the abolition of all forms of authority whether economical, political, social, religious, cultural or sexual."[http://www.iaf-ifa.org/principles/english.html "Principles of The [[International of Anarchist Federations]]"]</ref><ref>"Anarchism, then, really stands for the liberation of the human mind from the dominion of religion; the liberation of the human body from the dominion of property; liberation from the shackles and restraint of government. Anarchism stands for a social order based on the free grouping of individuals for the purpose of producing real social wealth; an order that will guarantee to every human being free access to the earth and full enjoyment of the necessities of life, according to individual desires, tastes, and inclinations." [[Emma Goldman]]. "What it Really Stands for Anarchy" in ''[[Anarchism and Other)#",
        R"#(Essays]]''.</ref><ref>Individualist anarchist Benjamin Tucker defined anarchism as opposition to authority as follows "They found that they must turn either to the right or to the left, — follow either the path of Authority or the path of Liberty. Marx went one way; Warren and Proudhon the other. Thus were born State Socialism and Anarchism...Authority, takes many shapes, but, broadly speaking, her enemies divide themselves into three classes: first, those who abhor her both as a means and as an end of progress, opposing her openly, avowedly, sincerely, consistently, universally; second, those who profess to believe in her as a means of progress, but who accept her only so far as they think she will subserve their own selfish interests, denying her and her blessings to the rest of the world; third, those who distrust her as a means of progress, believing in her only as an end to be obtained by first trampling upon, violating, and outraging her. These three phases of opposition to Liberty are met in almost)#",
        R"#(every sphere of thought and human activity. Good representatives of the first are seen in the Catholic Church and the Russian autocracy; of the second, in the Protestant Church and the Manchester school of politics and political economy; of the third, in the atheism of Gambetta and the socialism of the socialism off Karl Marg." [[Benjamin Tucker]]. [http://www.theanarchistlibrary.org/HTML/Benjamin_Tucker__Individual_Liberty.html ''Individual Liberty.'']</ref><ref name="Ward 1966">{{cite web|url=http://www.panarchy.org/ward/organization.1966.html|last=Ward|first=Colin|year=1966|title=Anarchism as a Theory of Organization|accessdate=1 March 2010| archiveurl= http://web.archive.org/web/20100325081119/http://www.panarchy.org/ward/organization.1966.html| archivedate= 25 March 2010<!--Added by DASHBot-->}}</ref><ref>Anarchist historian [[George Woodcock]] report of [[Mikhail Bakunin]]'s anti-authoritarianism and shows opposition to both state and non-state forms of authority as follows: "All anarchists deny)#"};

    for (auto &query : queries) {
        TermList term_list;
        analyzer.Analyze(query, term_list);
        // std::cout << "Text #" << query << "# parsed as:" << std::endl;
        // for (unsigned i = 0; i < term_list.size(); ++i) {
        //     std::cout << "\t" << i << "#" << term_list[i].text_ << "@" << term_list[i].word_offset_ << "#";
        // }
        // std::cout << std::endl;
    }
}

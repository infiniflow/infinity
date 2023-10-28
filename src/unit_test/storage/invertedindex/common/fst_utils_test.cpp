#include "unit_test/base_test.h"

#include "storage/invertedindex/common/fst/automaton.h"
#include "storage/invertedindex/common/fst/sorted_range_matcher.h"
#include "storage/invertedindex/common/fst/table_matcher.h"
#include <string>

class FSTUtilsTest : public BaseTest {
    void SetUp() override {}

    void TearDown() override {}
};

using namespace infinity;
using namespace fst;

TEST_F(FSTUtilsTest, test1) {
    static_assert(fst::MATCH_INPUT == fst::TableMatcher<fst::Automaton<fst::BooleanWeight>>::MATCH_TYPE);
    static_assert(fst::kNoIEpsilons | fst::kIDeterministic | fst::kAcceptor == fst::TableMatcher<fst::Automaton<fst::BooleanWeight>>::MATCH_TYPE);
    static_assert(fst::MATCH_OUTPUT == fst::TableMatcher<fst::Automaton<fst::BooleanWeight>, 256, false>::MATCH_TYPE);
    static_assert(fst::kNoOEpsilons | fst::kODeterministic |
                  fst::kAcceptor == fst::TableMatcher<fst::Automaton<fst::BooleanWeight>, 256, false>::MATCH_TYPE);
    static_assert(std::is_same<fst::Automaton<fst::BooleanWeight>, fst::TableMatcher<fst::Automaton<fst::BooleanWeight>>::FST>::value);
    static_assert(std::is_same<fst::Transition<fst::BooleanWeight>, fst::TableMatcher<fst::Automaton<fst::BooleanWeight>>::Arc>::value);
    static_assert(std::is_same<int64_t, fst::TableMatcher<fst::Automaton<fst::BooleanWeight>>::Label>::value);
    static_assert(std::is_same<int32_t, fst::TableMatcher<fst::Automaton<fst::BooleanWeight>>::StateId>::value);
    static_assert(std::is_same<fst::BooleanWeight, fst::TableMatcher<fst::Automaton<fst::BooleanWeight>>::Weight>::value);
}

TEST_F(FSTUtilsTest, test2) {

    // non-deterministic
    {
        fst::Automaton<fst::BooleanWeight> a;
        a.AddState(); // 0
        a.AddState(); // 1
        a.EmplaceArc(1, fst::RangeLabel(1), 0);
        a.EmplaceArc(1, fst::RangeLabel(1), 0);
        ASSERT_EQ(fst::kILabelSorted, a.Properties(fst::kIDeterministic | fst::kILabelSorted, true));
        fst::TableMatcher<decltype(a)> matcher(a, true);
        ASSERT_EQ(fst::kError, matcher.Properties(0));
    }

    // non-deterministic
    {
        fst::Automaton<fst::BooleanWeight> a;
        a.AddState(); // 0
        a.AddState(); // 1
        a.EmplaceArc(1, fst::RangeLabel(1, 21), 0);
        a.EmplaceArc(1, fst::RangeLabel(1, 21), 0);
        ASSERT_EQ(fst::kILabelSorted, a.Properties(fst::kIDeterministic | fst::kILabelSorted, true));
        fst::TableMatcher<decltype(a)> matcher(a, true);
        ASSERT_EQ(fst::kError, matcher.Properties(0));
    }

    // non-deterministic
    {
        fst::Automaton<fst::BooleanWeight> a;
        a.AddState(); // 0
        a.AddState(); // 1
        a.EmplaceArc(1, fst::RangeLabel(1, 21), 0);
        a.EmplaceArc(1, fst::RangeLabel(21, 22), 0);
        ASSERT_EQ(fst::kILabelSorted, a.Properties(fst::kIDeterministic | fst::kILabelSorted, true));
        fst::TableMatcher<decltype(a)> matcher(a, true);
        ASSERT_EQ(fst::kError, matcher.Properties(0));
    }

    // non-deterministic
    {
        fst::Automaton<fst::BooleanWeight> a;
        a.AddState(); // 0
        a.AddState(); // 1
        a.EmplaceArc(1, fst::RangeLabel(1, 21), 0);
        a.EmplaceArc(1, fst::RangeLabel(19, 22), 0);
        ASSERT_EQ(fst::kILabelSorted, a.Properties(fst::kIDeterministic | fst::kILabelSorted, true));
        fst::TableMatcher<decltype(a)> matcher(a, true);
        ASSERT_EQ(fst::kError, matcher.Properties(0));
    }

    // non-deterministic
    {
        fst::Automaton<fst::BooleanWeight> a;
        a.AddState(); // 0
        a.AddState(); // 1
        a.EmplaceArc(1, fst::RangeLabel(1, 21), 0);
        a.EmplaceArc(1, fst::RangeLabel(19, 22), 0);
        ASSERT_EQ(fst::kILabelSorted, a.Properties(fst::kIDeterministic | fst::kILabelSorted, true));
        fst::TableMatcher<decltype(a)> matcher(a, true);
        ASSERT_EQ(fst::kError, matcher.Properties(0));
    }

    // non-deterministic, unsorted
    {
        fst::Automaton<fst::BooleanWeight> a;
        a.AddState(); // 0
        a.AddState(); // 1
        a.EmplaceArc(1, fst::RangeLabel(19, 22), 0);
        a.EmplaceArc(1, fst::RangeLabel(1, 21), 0);
        ASSERT_EQ(0, a.Properties(fst::kIDeterministic | fst::kILabelSorted, true));
        fst::TableMatcher<decltype(a)> matcher(a, true);
        ASSERT_EQ(fst::kError, matcher.Properties(0));
    }

    // deterministic, unsorted
    {
        fst::Automaton<fst::BooleanWeight> a;
        a.AddState(); // 0
        a.AddState(); // 1
        a.EmplaceArc(1, fst::RangeLabel(19, 22), 0);
        a.EmplaceArc(1, fst::RangeLabel(1, 18), 0);
        ASSERT_EQ(fst::kIDeterministic, a.Properties(fst::kIDeterministic | fst::kILabelSorted, true));
        fst::TableMatcher<decltype(a)> matcher(a, true);
        ASSERT_EQ(fst::kError, matcher.Properties(0));
    }

    // acceptor, regardless of specified arc weights
    {
        fst::Automaton<fst::BooleanWeight> a;
        a.AddState(); // 0
        a.AddState(); // 1
        a.EmplaceArc(1, fst::RangeLabel(1, 1), 0);
        a.EmplaceArc(1, fst::RangeLabel(2, 4), 0);
        ASSERT_EQ(fst::kIDeterministic | fst::kILabelSorted, a.Properties(fst::kIDeterministic | fst::kILabelSorted, true));
        fst::ArcIteratorData<decltype(a)::Arc> data;
        a.InitArcIterator(1, &data);
        const_cast<decltype(a)::Arc &>(data.arcs[0]).olabel = fst::kNoLabel;

        fst::TableMatcher<decltype(a)> matcher(a, true);
        ASSERT_NE(fst::kError, matcher.Properties(0));
    }
}

TEST_F(FSTUtilsTest, test3) {
    // create matcher with an empty automaton
    {
        fst::Automaton<fst::BooleanWeight> a;
        fst::TableMatcher<decltype(a)> matcher(a, true);
        ASSERT_NE(fst::kError, matcher.Properties(0));
        ASSERT_TRUE(matcher.Done());
        matcher.Next();
        ASSERT_TRUE(matcher.Done());
        ASSERT_EQ(&a, &matcher.GetFst());
        ASSERT_EQ(fst::MATCH_INPUT, matcher.Type(false));
        ASSERT_EQ(fst::MATCH_INPUT, matcher.Type(true));

        std::unique_ptr<fst::TableMatcher<decltype(a)>> copy(matcher.Copy(false));
        ASSERT_NE(nullptr, copy);
        ASSERT_NE(fst::kError, copy->Properties(0));
        ASSERT_TRUE(copy->Done());
        copy->Next();
        ASSERT_TRUE(copy->Done());
        ASSERT_EQ(&a, &copy->GetFst());
        ASSERT_EQ(fst::MATCH_INPUT, copy->Type(false));
        ASSERT_EQ(fst::MATCH_INPUT, copy->Type(true));
    }

    // create matcher with non-empty automaton
    {
        fst::Automaton<fst::BooleanWeight> a;
        a.SetFinal(a.AddState());
        a.AddState();
        a.EmplaceArc(1, fst::RangeLabel(42, 42), 0);

        fst::TableMatcher<decltype(a)> matcher(a, true);
        ASSERT_NE(fst::kError, matcher.Properties(0));
        ASSERT_TRUE(matcher.Done());
        matcher.Next();
        ASSERT_TRUE(matcher.Done());
        ASSERT_EQ(fst::BooleanWeight(true), matcher.Final(0));
        ASSERT_EQ(fst::BooleanWeight(false), matcher.Final(1));
        ASSERT_EQ(0, matcher.Priority(0));
        ASSERT_EQ(1, matcher.Priority(1));
        ASSERT_EQ(&a, &matcher.GetFst());
        ASSERT_EQ(fst::MATCH_INPUT, matcher.Type(false));
        ASSERT_EQ(fst::MATCH_INPUT, matcher.Type(true));
        matcher.SetState(0);
        ASSERT_FALSE(matcher.Find(42));
        ASSERT_FALSE(matcher.Done());
        matcher.Next();
        ASSERT_TRUE(matcher.Done());
        matcher.SetState(1);
        ASSERT_TRUE(matcher.Find(42));
        ASSERT_EQ(0, matcher.Value().nextstate);
        matcher.Next();
        ASSERT_TRUE(matcher.Done());
        matcher.Next();
        ASSERT_TRUE(matcher.Done());

        std::unique_ptr<fst::TableMatcher<decltype(a)>> copy(matcher.Copy(false));
        ASSERT_NE(nullptr, copy);
        ASSERT_NE(fst::kError, copy->Properties(0));
        ASSERT_TRUE(copy->Done());
        copy->Next();
        ASSERT_TRUE(copy->Done());
        ASSERT_EQ(fst::BooleanWeight(true), copy->Final(0));
        ASSERT_EQ(fst::BooleanWeight(false), copy->Final(1));
        ASSERT_EQ(0, copy->Priority(0));
        ASSERT_EQ(1, copy->Priority(1));
        ASSERT_EQ(&a, &copy->GetFst());
        ASSERT_EQ(fst::MATCH_INPUT, copy->Type(false));
        ASSERT_EQ(fst::MATCH_INPUT, copy->Type(true));

        copy->SetState(0);
        ASSERT_FALSE(copy->Find(42));
        ASSERT_FALSE(copy->Done());
        copy->Next();
        ASSERT_TRUE(copy->Done());
        copy->Next();
        ASSERT_TRUE(copy->Done());
        copy->SetState(1);
        ASSERT_TRUE(copy->Find(42));
        ASSERT_EQ(0, copy->Value().nextstate);
        copy->Next();
        ASSERT_TRUE(copy->Done());
        copy->Next();
        ASSERT_TRUE(copy->Done());
    }

    // create matcher with non-empty automaton and range
    {
        fst::Automaton<fst::BooleanWeight> a;
        a.SetFinal(a.AddState()); // 0
        a.AddState();             // 1
        a.EmplaceArc(1, fst::RangeLabel(42), 0);
        a.EmplaceArc(1, fst::RangeLabel(43, 255), 0);

        fst::TableMatcher<decltype(a)> matcher(a, true);
        ASSERT_NE(fst::kError, matcher.Properties(0));
        ASSERT_TRUE(matcher.Done());
        matcher.Next();
        ASSERT_TRUE(matcher.Done());
        ASSERT_EQ(fst::BooleanWeight(true), matcher.Final(0));
        ASSERT_EQ(fst::BooleanWeight(false), matcher.Final(1));
        ASSERT_EQ(0, matcher.Priority(0));
        ASSERT_EQ(2, matcher.Priority(1));
        ASSERT_EQ(&a, &matcher.GetFst());
        ASSERT_EQ(fst::MATCH_INPUT, matcher.Type(false));
        ASSERT_EQ(fst::MATCH_INPUT, matcher.Type(true));

        matcher.SetState(0);
        ASSERT_FALSE(matcher.Find(42));
        ASSERT_FALSE(matcher.Done());
        matcher.Next();
        ASSERT_TRUE(matcher.Done());
        matcher.SetState(1);
        ASSERT_TRUE(matcher.Find(42));
        ASSERT_EQ(0, matcher.Value().nextstate);
        matcher.Next();
        ASSERT_EQ(0, matcher.Value().nextstate); // rho transition
        matcher.Next();
        ASSERT_TRUE(matcher.Done());
        matcher.Next();
        ASSERT_TRUE(matcher.Done());
        matcher.SetState(1);
        ASSERT_TRUE(matcher.Find(43));
        ASSERT_EQ(0, matcher.Value().nextstate); // rho transition
        matcher.Next();
        ASSERT_TRUE(matcher.Done());
        matcher.Next();
        ASSERT_TRUE(matcher.Done());

        std::unique_ptr<fst::TableMatcher<decltype(a)>> copy(matcher.Copy(false));
        ASSERT_NE(nullptr, copy);
        ASSERT_NE(fst::kError, copy->Properties(0));
        ASSERT_TRUE(copy->Done());
        copy->Next();
        ASSERT_TRUE(copy->Done());
        ASSERT_EQ(fst::BooleanWeight(true), copy->Final(0));
        ASSERT_EQ(fst::BooleanWeight(false), copy->Final(1));
        ASSERT_EQ(0, copy->Priority(0));
        ASSERT_EQ(2, copy->Priority(1));
        ASSERT_EQ(&a, &copy->GetFst());
        ASSERT_EQ(fst::MATCH_INPUT, copy->Type(false));
        ASSERT_EQ(fst::MATCH_INPUT, copy->Type(true));

        copy->SetState(0);
        ASSERT_FALSE(copy->Find(42));
        ASSERT_FALSE(copy->Done());
        copy->Next();
        ASSERT_TRUE(copy->Done());
        copy->SetState(1);
        ASSERT_TRUE(copy->Find(42));
        ASSERT_EQ(0, copy->Value().nextstate);
        copy->Next();
        ASSERT_EQ(0, copy->Value().nextstate); // rho transition
        copy->Next();
        ASSERT_TRUE(copy->Done());
        copy->Next();
        ASSERT_TRUE(copy->Done());
        copy->SetState(1);
        ASSERT_TRUE(copy->Find(43));
        ASSERT_EQ(0, copy->Value().nextstate); // rho transition
        copy->Next();
        ASSERT_TRUE(copy->Done());
        copy->Next();
        ASSERT_TRUE(copy->Done());
    }

    // complex automaton
    {
        fst::Automaton<fst::BooleanWeight> a;

        // build automaton
        {
            auto from = a.AddState();
            auto add_state = [&a, &from](uint32_t min, uint32_t max, int step) mutable {
                auto to = a.AddState();

                for (; min < max; min += step) {
                    a.EmplaceArc(from, fst::RangeLabel::fromRange(min), to);
                }

                from = to;
            };

            a.SetStart(from);        // state: 0
            add_state(1, 1024, 3);   // state: 1
            add_state(512, 2048, 7); // state: 2
            add_state(152, 512, 11); // state: 3
            {                        // state: 4
                auto to = a.AddState();
                a.EmplaceArc(from, fst::RangeLabel(1, 2), to);
                a.EmplaceArc(from, fst::RangeLabel(17, 167), to);
                a.EmplaceArc(from, fst::RangeLabel(178, 1023), to);
                from = to;
            }

            a.SetFinal(from); // state: 5
        }

        // check automaton

        {
            using matcher_t = fst::TableMatcher<decltype(a)>;
            using expected_matcher_t = fst::SortedRangeExplicitMatcher<decltype(a)>;

            expected_matcher_t expected_matcher(&a);
            matcher_t matcher(a, true);
            for (decltype(a)::StateId state = 0; state < a.NumStates(); ++state) {
                expected_matcher.SetState(state);
                matcher.SetState(state);

                for (matcher_t::Label min = 1, max = 2049; min < max; ++min) {
                    const auto found = expected_matcher.Find(min);
                    ASSERT_EQ(found, matcher.Find(min));
                    if (found) {
                        ASSERT_EQ(expected_matcher.Value().nextstate, matcher.Value().nextstate);
                    }
                }
            }
        }
    }
}
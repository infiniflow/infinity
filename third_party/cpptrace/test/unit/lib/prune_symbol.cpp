#include <gtest/gtest.h>
#include <gtest/gtest-matchers.h>
#include <gmock/gmock.h>
#include <gmock/gmock-matchers.h>

#ifdef TEST_MODULE
import cpptrace;
#else
#include <cpptrace/utils.hpp>
#endif

namespace {

#define DO_TEST(symbol, expected) EXPECT_EQ(cpptrace::prune_symbol(symbol), expected) << "Input: " << symbol

TEST(PruneSymbolTests, Basic) {
    // https://godbolt.org/z/Weas1ETPv
    DO_TEST("foo()", "foo");
    DO_TEST("foo(int, char)", "foo");
    DO_TEST("void foo(void)", "foo");
    DO_TEST("void foo(int, char)", "foo");
}

TEST(PruneSymbolTests, Namespaces) {
    // https://godbolt.org/z/WzfsrPhE4
    DO_TEST("foo()", "foo");
    DO_TEST("ns::foo()", "ns::foo");
    DO_TEST("ns::ns2::foo()", "ns::ns2::foo");
    DO_TEST("ns::v1::bar()", "ns::v1::bar");
    DO_TEST("(anonymous namespace)::bar()", "(anonymous namespace)::bar");

    DO_TEST("void foo(void)", "foo");
    DO_TEST("void ns::foo(void)", "ns::foo");
    DO_TEST("void ns::ns2::foo(void)", "ns::ns2::foo");
    DO_TEST("void ns::v1::bar(void)", "ns::v1::bar");
    DO_TEST("void `anonymous namespace'::bar(void)", "(anonymous namespace)::bar");
}

TEST(PruneSymbolTests, BasicTemplates) {
    // https://godbolt.org/z/czWo9bdrn
    DO_TEST("void foo<int>(int const&)", "foo");
    DO_TEST("void foo<int, double>(int const&, double const&)", "foo");
    DO_TEST("void foo<S<int>>(S<int> const&)", "foo"); // llvm
    DO_TEST("void foo<S<int> >(S<int> const&)", "foo"); // gnutils
    DO_TEST("void foo<S<int, float>>(S<int, float> const&)", "foo");
    DO_TEST("void foo<S<int, float> >(S<int, float> const&)", "foo");
    DO_TEST("void foo<S<S<float>, S<int>>>(S<S<float>, S<int>> const&)", "foo");
    DO_TEST("void foo<S<S<float>, S<int> > >(S<S<float>, S<int> > const&)", "foo");

    DO_TEST("void foo<int>(int const &)", "foo");
    DO_TEST("void foo<int,double>(int const &,double const &)", "foo");
    DO_TEST("void foo<S<int> >(S<int> const &)", "foo");
    DO_TEST("void foo<S<int,float> >(S<int,float> const &)", "foo");
    DO_TEST("void foo<S<S<float>,S<int> > >(S<S<float>,S<int> > const &)", "foo");
}

TEST(PruneSymbolTests, MemberFunctions) {
    // https://godbolt.org/z/re9zfzPq5
    DO_TEST("S::S()", "S::S");
    DO_TEST("S::~S()", "S::~S");
    DO_TEST("S::foo()", "S::foo");
    DO_TEST("S::bar() const", "S::bar");
    DO_TEST("void S::bar() const", "S::bar");

    DO_TEST("ns::SS<>::SS()", "ns::SS::SS");
    DO_TEST("ns::SS<>::~SS()", "ns::SS::~SS");
    DO_TEST("ns::SS<>::foo()", "ns::SS::foo");
    DO_TEST("ns::SS<int, float>::SS()", "ns::SS::SS");
    DO_TEST("ns::SS<int, float>::~SS()", "ns::SS::~SS");
    DO_TEST("ns::SS<int, float>::foo()", "ns::SS::foo");
    DO_TEST("ns::SS<ns::SS<int>, ns::SS<float>>::SS()", "ns::SS::SS");
    DO_TEST("ns::SS<ns::SS<int>, ns::SS<float>>::~SS()", "ns::SS::~SS");
    DO_TEST("ns::SS<ns::SS<int>, ns::SS<float>>::foo()", "ns::SS::foo");

    DO_TEST("ns::SS<>::SS<>(void)", "ns::SS::SS");
    DO_TEST("ns::SS<>::~SS<>(void)", "ns::SS::~SS");
    DO_TEST("void ns::SS<>::foo(void)", "ns::SS::foo");
    DO_TEST("ns::SS<int,float>::SS<int,float>(void)", "ns::SS::SS");
    DO_TEST("ns::SS<int,float>::~SS<int,float>(void)", "ns::SS::~SS");
    DO_TEST("void ns::SS<int,float>::foo(void)", "ns::SS::foo");
    DO_TEST("ns::SS<ns::SS<int>,ns::SS<float> >::SS<ns::SS<int>,ns::SS<float> >(void)", "ns::SS::SS");
    DO_TEST("ns::SS<ns::SS<int>,ns::SS<float> >::~SS<ns::SS<int>,ns::SS<float> >(void)", "ns::SS::~SS");
    DO_TEST("void ns::SS<ns::SS<int>,ns::SS<float> >::foo(void)", "ns::SS::foo");
}

TEST(PruneSymbolTests, TemplatedMemberFunctions) {
    // https://godbolt.org/z/dc3TEheK9
    DO_TEST("ns::SS<>::SS<>()", "ns::SS::SS");
    DO_TEST("void ns::SS<>::foo<>()", "ns::SS::foo");
    DO_TEST("ns::SS<int, float>::SS<int, float>(int, float)", "ns::SS::SS");
    DO_TEST("void ns::SS<int, float>::foo<int, float>()", "ns::SS::foo");
    DO_TEST("ns::SS<ns::SS<int>, ns::SS<float>>::SS<ns::SS<int>, ns::SS<float>>(ns::SS<int>, ns::SS<float>)", "ns::SS::SS");
    DO_TEST("void ns::SS<ns::SS<int>, ns::SS<float>>::foo<ns::SS<int>, ns::SS<float>>()", "ns::SS::foo");

    DO_TEST("ns::SS<ns::SS<int>, ns::SS<float> >::SS<ns::SS<int>, ns::SS<float> >(ns::SS<int>, ns::SS<float>)", "ns::SS::SS");
    DO_TEST("void ns::SS<ns::SS<int>, ns::SS<float> >::foo<ns::SS<int>, ns::SS<float> >()", "ns::SS::foo");

    DO_TEST("ns::SS<>::SS<><>(void)", "ns::SS::SS");
    DO_TEST("void ns::SS<>::foo<>(void)", "ns::SS::foo");
    DO_TEST("ns::SS<int,float>::SS<int,float><int,float>(int,float)", "ns::SS::SS");
    DO_TEST("void ns::SS<int,float>::foo<int,float>(void)", "ns::SS::foo");
    DO_TEST("ns::SS<ns::SS<int>,ns::SS<float> >::SS<ns::SS<int>,ns::SS<float> ><ns::SS<int>,ns::SS<float> >(ns::SS<int>,ns::SS<float>)", "ns::SS::SS");
    DO_TEST("void ns::SS<ns::SS<int>,ns::SS<float> >::foo<ns::SS<int>,ns::SS<float> >(void)", "ns::SS::foo");
}

TEST(PruneSymbolTests, Decltype) {
    // https://godbolt.org/z/dc3TEheK9
    DO_TEST("decltype(declval<int>() + declval<int>()) foo<int>(int)", "foo");
    DO_TEST("decltype (((declval<int>)())+((declval<int>)())) foo<int>(int)", "foo");
    DO_TEST("decltype(std::declval<int>() + std::declval<int>()) foo<int>(int)", "foo");
    DO_TEST("int foo<int>(int)", "foo");

    DO_TEST("decltype(declval<int>() + declval<int>()) bar<int>(decltype(declval<int>()))", "bar");
    DO_TEST("decltype (((declval<int>)())+((declval<int>)())) bar<int>(decltype ((declval<int>)()))", "bar");
    DO_TEST("decltype(std::declval<int>() + std::declval<int>()) bar<int>(decltype(std::declval<int>()))", "bar");
    DO_TEST("int bar<int>(int &&)", "bar");

    DO_TEST("decltype(declval<int>() < declval<int>()) baz<int>(int)", "baz");
    DO_TEST("decltype (((declval<int>)())<((declval<int>)())) baz<int>(int)", "baz");
    DO_TEST("decltype(std::declval<int>() < std::declval<int>()) baz<int>(int)", "baz");
    DO_TEST("bool baz<int>(int)", "baz");

    // https://godbolt.org/z/4M1xfW5rP
    DO_TEST("decltype(int{} + int{}) foo<int>(int)", "foo");
    DO_TEST("decltype (int{}+int{}) foo<int>(int)", "foo");
    DO_TEST("int foo<int>(int)", "foo");
}

TEST(PruneSymbolTests, Operators) {
    // https://godbolt.org/z/qMKEKW656
    DO_TEST("S<int>::operator*() const", "S::operator*");
    DO_TEST("S<int>::operator+(S<int> const&) const", "S::operator+");
    DO_TEST("S<int>::operator>(S<int> const&) const", "S::operator>");
    DO_TEST("S<int>::operator<<(S<int> const&) const", "S::operator<<");
    DO_TEST("S<int>::operator()() const", "S::operator()");
    DO_TEST("S<int>::operator[](int) const", "S::operator[]");

    DO_TEST("void S<int>::operator*(void)const", "S::operator*");
    DO_TEST("void S<int>::operator+(S<int> const &)const", "S::operator+");
    DO_TEST("void S<int>::operator>(S<int> const &)const", "S::operator>");
    DO_TEST("void S<int>::operator<<(S<int> const &)const", "S::operator<<");
    DO_TEST("void S<int>::operator()(void)const", "S::operator()");
    DO_TEST("void S<int>::operator[](int)const", "S::operator[]");

    // https://godbolt.org/z/E1bqKf8vv
    DO_TEST("operator*(S, S)", "operator*");
    DO_TEST("operator\"\" _w(unsigned long long)", "operator\"\"_w");
    DO_TEST("unsigned __int64 operator \"\" _w(unsigned __int64)", "operator\"\"_w");
}

TEST(PruneSymbolTests, TemplatedOperators) {
    // https://godbolt.org/z/nfcrTfj7M
    DO_TEST("void operator+<S>(S, S)", "operator+");
    // DO_TEST("void operator<<S>(S, S)", "operator<"); // TODO FAIL
    DO_TEST("void operator<<<S>(S, S)", "operator<<");
    DO_TEST("void operator< <S>(S, S)", "operator<");
    DO_TEST("void operator<< <S>(S, S)", "operator<<");
}

TEST(PruneSymbolTests, OperatorNewDeleteCoAwait) {
    // https://godbolt.org/z/rq16K9sK3
    DO_TEST("operator new(unsigned long)", "operator new");
    DO_TEST("operator new[](unsigned long)", "operator new[]");
    DO_TEST("operator delete(unsigned long)", "operator delete");
    DO_TEST("operator delete[](unsigned long)", "operator delete[]");
    DO_TEST("S::operator new(unsigned long)", "S::operator new");
    DO_TEST("S::operator new[](unsigned long)", "S::operator new[]");
    DO_TEST("S::operator delete(void*)", "S::operator delete");
    DO_TEST("S::operator delete[](void*)", "S::operator delete[]");

    DO_TEST("void * operator new(unsigned __int64)", "operator new");
    DO_TEST("void * operator new[](unsigned __int64)", "operator new[]");
    DO_TEST("void operator delete(void *)", "operator delete");
    DO_TEST("void operator delete[](void *)", "operator delete[]");
    DO_TEST("static void * S::operator new(unsigned __int64)", "S::operator new");
    DO_TEST("static void * S::operator new[](unsigned __int64)", "S::operator new[]");
    DO_TEST("static void S::operator delete(void *)", "S::operator delete");
    DO_TEST("static void S::operator delete[](void *)", "S::operator delete[]");

    // https://godbolt.org/z/a3GeKjh5a
    DO_TEST("operator co_await(A)", "operator co_await");
    DO_TEST("B::operator co_await()", "B::operator co_await");
    DO_TEST("void operator co_await(A)", "operator co_await");
    DO_TEST("void B::operator co_await(void)", "B::operator co_await");
}

TEST(PruneSymbolTests, NTTPs) {
    // https://godbolt.org/z/4aPavzsba
    DO_TEST("void foo<12, 20, 256, 1>()", "foo");
    DO_TEST("void foo<true, false>()", "foo");
    DO_TEST("void foo<(char)97, (char)98, (char)0, (char)39>()", "foo");
    DO_TEST("void foo<0x1p-1, 0x1.8p+3f, 0x1.00aabbccp+10>()", "foo");
    DO_TEST("void foo<&p>()", "foo");
    DO_TEST("void foo<&main>()", "foo");
    DO_TEST("void foo<&void foo<20, true>()>()", "foo");

    DO_TEST("void foo<12,20,256,1>(void)", "foo");
    DO_TEST("void foo<1,0>(void)", "foo");
    DO_TEST("void foo<0.500000,12.000000,1026.667712>(void)", "foo");
    DO_TEST("void foo<&int p>(void)", "foo");
    DO_TEST("void foo<&void foo<20,1>(void)>(void)", "foo");

    // https://godbolt.org/z/WEz836Yv7
    DO_TEST("void foo<fixed_string<12ul>{\"foobar`'\\\"bar\"}>()", "foo");
    DO_TEST("void foo<fixed_string<12>{char{102,111,111,98,97,114,96,39,34,98,97,114,0}}>(void)", "foo");

    DO_TEST("void foo<fixed_string<12ul>{\"foobar`'\\\"bar\"}>()::test", "foo::test");
    DO_TEST("void foo<fixed_string<13ul>{\"foobar`\\\"bar\\\"'\"}>()::test", "foo::test");
    DO_TEST("void foo<fixed_string<13ul>{\"foobar`\\\"bar'\"}>()::test", "foo::test");
    DO_TEST("void foo<fixed_string<13>{char{102,111,111,98,97,114,96,34,98,97,114,34,39,0}}>(void)::test", "foo::test");

    // test that unterminated strings cause errors
    DO_TEST("void foo<bar(\"test\")>::baz", "foo::baz");
    DO_TEST("void foo<bar(`test')>::baz", "foo::baz");
    DO_TEST("void foo<bar(\"test)>::baz", "void foo<bar(\"test)>::baz");
    DO_TEST("void foo<bar(`test)>::baz", "void foo<bar(`test)>::baz");
}

TEST(PruneSymbolTests, OperatorNTTPs) {
    // https://godbolt.org/z/foY7WfGv3
    DO_TEST("void foo<&S::operator<(S const&)>()", "foo");
    DO_TEST("void foo<&S::operator>(S const&)>()", "foo");
    DO_TEST("void foo<&S::operator>>(S const&)>()", "foo");
    DO_TEST("void foo<&S::operator>>=(S const&)>()", "foo");
    DO_TEST("void foo<&S::operator<=(S const&)>()", "foo");
    DO_TEST("void foo<&S::operator<<=(S const&)>()", "foo");
    // DO_TEST("void foo<&bool X<int, float>::operator<<int, float>(X<int, float> const&)>()", "foo"); // TODO: FAIL
    DO_TEST("void foo<&bool X<int, float>::operator><int, float>(X<int, float> const&)>()", "foo");
    DO_TEST("void foo<&bool X<int, float>::operator>><int, float>(X<int, float> const&)>()", "foo");
    DO_TEST("void foo<&bool X<int, float>::operator>>=<int, float>(X<int, float> const&)>()", "foo");
    DO_TEST("void foo<&bool X<int, float>::operator<=<int, float>(X<int, float> const&)>()", "foo");
    DO_TEST("void foo<&bool X<int, float>::operator<<=<int, float>(X<int, float> const&)>()", "foo");

    DO_TEST("void foo<&bool S::operator<(S const &)>(void)", "foo");
    DO_TEST("void foo<&bool S::operator>(S const &)>(void)", "foo");
    DO_TEST("void foo<&bool S::operator>>(S const &)>(void)", "foo");
    DO_TEST("void foo<&bool S::operator>>=(S const &)>(void)", "foo");
    DO_TEST("void foo<&bool S::operator<=(S const &)>(void)", "foo");
    DO_TEST("void foo<&bool S::operator<<=(S const &)>(void)", "foo");
    // DO_TEST("void foo<&bool X<int,float>::operator<<int,float>(X<int,float> const &)>(void)", "foo"); // TODO: FAIL
    DO_TEST("void foo<&bool X<int,float>::operator><int,float>(X<int,float> const &)>(void)", "foo");
    DO_TEST("void foo<&bool X<int,float>::operator>><int,float>(X<int,float> const &)>(void)", "foo");
    DO_TEST("void foo<&bool X<int,float>::operator>>=<int,float>(X<int,float> const &)>(void)", "foo");
    DO_TEST("void foo<&bool X<int,float>::operator<=<int,float>(X<int,float> const &)>(void)", "foo");
    DO_TEST("void foo<&bool X<int,float>::operator<<=<int,float>(X<int,float> const &)>(void)", "foo");

    DO_TEST("void foo<&S::operator>(S const&)>()::test", "foo::test");
    DO_TEST("void foo<&S::operator>>(S const&)>()::test", "foo::test");
    DO_TEST("void foo<&S::operator>>=(S const&)>()::test", "foo::test");
    DO_TEST("void foo<&bool S::operator>(S const &)>(void)::test", "foo::test");
    DO_TEST("void foo<&bool S::operator>>(S const &)>(void)::test", "foo::test");
    DO_TEST("void foo<&bool S::operator>>=(S const &)>(void)::test", "foo::test");
    DO_TEST("void foo<&bool X<int,float>::operator><int,float>(X<int,float> const &)>(void)::test", "foo::test");
    DO_TEST("void foo<&bool X<int,float>::operator>><int,float>(X<int,float> const &)>(void)::test", "foo::test");
    DO_TEST("void foo<&bool X<int,float>::operator>>=<int,float>(X<int,float> const &)>(void)::test", "foo::test");
    DO_TEST("void foo<&S::operator>(S const&), bar>()::test", "foo::test");
    DO_TEST("void foo<&S::operator>>(S const&), bar>()::test", "foo::test");
    DO_TEST("void foo<&S::operator>>=(S const&), bar>()::test", "foo::test");
    DO_TEST("void foo<&bool S::operator>(S const &)>(vo, barid)::test", "foo::test");
    DO_TEST("void foo<&bool S::operator>>(S const &)>(vo, barid)::test", "foo::test");
    DO_TEST("void foo<&bool S::operator>>=(S const &)>(vo, barid)::test", "foo::test");
    DO_TEST("void foo<&bool X<int,float>::operator><int,float>(X<int,float> const &)>(vo, barid)::test", "foo::test");
    DO_TEST("void foo<&bool X<int,float>::operator>><int,float>(X<int,float> const &)>(vo, barid)::test", "foo::test");
    DO_TEST("void foo<&bool X<int,float>::operator>>=<int,float>(X<int,float> const &)>(vo, barid)::test", "foo::test");

    // TODO: foo<&S::operator>>() isn't legal C++ but maybe it could appear in demangled output?
}

TEST(PruneSymbolTests, BasicLambdas) {
    // https://godbolt.org/z/5n83rGK8j
    DO_TEST("main::'lambda'()::operator()() const", "main::<lambda>::operator()");
    DO_TEST("main::'lambda0'()::operator()() const", "main::<lambda0>::operator()");
    DO_TEST("main::{lambda()#1}::operator()() const", "main::<lambda#1>::operator()");
    DO_TEST("main::{lambda()#2}::operator()() const", "main::<lambda#2>::operator()");
    DO_TEST("main::$_0::operator()() const", "main::$_0::operator()");
    DO_TEST("main::$_1::operator()() const", "main::$_1::operator()");
    DO_TEST("`int main(void)'::`2'::<lambda_1>::operator()(void)const", "`main'::`2'::<lambda_1>::operator()");
    DO_TEST("`int main(void)'::`2'::<lambda_2>::operator()(void)const", "`main'::`2'::<lambda_2>::operator()");
}

TEST(PruneSymbolTests, TemplatedLambdas) {
    // https://godbolt.org/z/GGEWfE144
    DO_TEST("auto main::'lambda'<typename $T>($T)::operator()<int>($T) const", "main::<lambda>::operator()");
    DO_TEST("auto main::{lambda<typename $T0>($T0)#1}::operator()<int>(int) const", "main::<lambda#1>::operator()");
    // DO_TEST("", ""); // TODO: llvm-symbolizer can't handle currently
    DO_TEST("auto `int main(void)'::`2'::<lambda_1>::operator()<int>(int)const", "`main'::`2'::<lambda_1>::operator()");
}

TEST(PruneSymbolTests, NestedLambdas) {
    // https://godbolt.org/z/s4GseqrGK
    DO_TEST("main::'lambda'()::operator()() const", "main::<lambda>::operator()");
    DO_TEST("main::'lambda'()::operator()() const::'lambda'()::operator()() const", "main::<lambda>::operator()::<lambda>::operator()");
    DO_TEST("main::{lambda()#1}::operator()() const", "main::<lambda#1>::operator()");
    DO_TEST("main::{lambda()#1}::operator()() const::{lambda()#1}::operator()() const", "main::<lambda#1>::operator()::<lambda#1>::operator()");
    DO_TEST("main::$_0::operator()() const", "main::$_0::operator()");
    DO_TEST("main::$_0::operator()() const::'lambda'()::operator()() const", "main::$_0::operator()::<lambda>::operator()");
    DO_TEST("`int main(void)'::`2'::<lambda_1>::operator()(void)const", "`main'::`2'::<lambda_1>::operator()");
    DO_TEST("``int main(void)'::`2'::<lambda_1>::operator()(void)const '::`2'::<lambda_1>::operator()(void)const", "``main'::`2'::<lambda_1>::operator()'::`2'::<lambda_1>::operator()");
    // https://godbolt.org/z/fnvW63819
    DO_TEST("auto main::'lambda'<typename $T>($T)::operator()<int>($T) const", "main::<lambda>::operator()");
    DO_TEST("auto auto main::'lambda'<typename $T>($T)::operator()<int>($T) const::'lambda'<typename $T0>($T)::operator()<int>($T) const", "main::<lambda>::operator()::<lambda>::operator()");
    DO_TEST("auto main::{lambda<typename $T0>($T0)#1}::operator()<int>(int) const", "main::<lambda#1>::operator()");
    DO_TEST("auto main::{lambda<typename $T0>($T0)#1}::operator()<int>(int) const::{lambda<typename $T0>($T0)#1}::operator()<int>(int) const", "main::<lambda#1>::operator()::<lambda#1>::operator()");
    // TODO: LLVM
    // TODO: LLVM
    DO_TEST("auto `int main(void)'::`2'::<lambda_1>::operator()<int>(int)const", "`main'::`2'::<lambda_1>::operator()");
    DO_TEST("auto `auto `int main(void)'::`2'::<lambda_1>::operator()<int>(int)const '::`2'::<lambda_1>::operator()<int>(int)const", "``main'::`2'::<lambda_1>::operator()'::`2'::<lambda_1>::operator()");
}

TEST(PruneSymbolTests, LambdaTemplateArgs) {
    // https://godbolt.org/z/9f53KezPE
    DO_TEST("S<main::'lambda'()>::foo()", "S::foo");
    DO_TEST("SS<main::'lambda0'(){}>::foo()", "SS::foo");
    DO_TEST("S<main::{lambda()#1}>::foo()", "S::foo");
    DO_TEST("SS<main::{lambda()#2}{}>::foo()", "SS::foo");
    DO_TEST("void S<`int main(void)'::`2'::<lambda_1_> >::foo(void)", "S::foo");
    DO_TEST("void SS<`int main(void)'::`2'::<lambda_2_>{}>::foo(void)", "SS::foo");
}

TEST(PruneSymbolTests, LambdasInTemplates) {
    // https://godbolt.org/z/qKv8xz7Mv
    DO_TEST("S<int>::foo()::'lambda'()::operator()() const", "S::foo::<lambda>::operator()");
    DO_TEST("S<int>::foo()::{lambda()#1}::operator()() const", "S::foo::<lambda#1>::operator()");
    DO_TEST("`void S<int>::foo(void)'::`2'::<lambda_1>::operator()(void)const", "`S::foo'::`2'::<lambda_1>::operator()");
}

TEST(PruneSymbolTests, LocalTypes) {
    // https://godbolt.org/z/51fbhMTMe
    DO_TEST("foo()::S::bar()", "foo::S::bar");
    DO_TEST("void `void foo(void)'::`2'::S::bar(void)", "`foo'::`2'::S::bar");

    DO_TEST("foo()::S::bar()::'lambda'()::operator()() const::V::boo()", "foo::S::bar::<lambda>::operator()::V::boo");
    DO_TEST("foo()::S::bar()::{lambda()#1}::operator()() const::V::boo()", "foo::S::bar::<lambda#1>::operator()::V::boo");
    DO_TEST("void ``void `void foo(void)'::`2'::S::bar(void)'::`2'::<lambda_1>::operator()(void)const '::`2'::V::boo(void)", "```foo'::`2'::S::bar'::`2'::<lambda_1>::operator()'::`2'::V::boo");

    // https://godbolt.org/z/hGaW8j9r3
    DO_TEST("auto auto A<SS>::foo<SS>()::'lambda'(auto)::operator()<int>(auto) const", "A::foo::<lambda>::operator()");
    DO_TEST("auto auto A<SS>::foo<SS>()::'lambda'(auto)::operator()<int>(auto) const::S::foo()", "A::foo::<lambda>::operator()::S::foo");
    DO_TEST("auto A<SS>::foo<SS>()::{lambda(auto:1)#1}::operator()<int>(int) const", "A::foo::<lambda#1>::operator()");
    DO_TEST("A<SS>::foo<SS>()::{lambda(auto:1)#1}::operator()<int>(int) const::S::foo()", "A::foo::<lambda#1>::operator()::S::foo");
    DO_TEST("auto `auto A<SS>::foo<SS>(void)'::`2'::<lambda_1>::operator()<int>(int)const", "`A::foo'::`2'::<lambda_1>::operator()");
    DO_TEST("void `auto `auto A<SS>::foo<SS>(void)'::`2'::<lambda_1>::operator()<int>(int)const '::`2'::S::foo(void)", "``A::foo'::`2'::<lambda_1>::operator()'::`2'::S::foo");
}

TEST(PruneSymbolTests, QualifiersAndAttributes) {
    // https://godbolt.org/z/rG5Ed5qed
    DO_TEST("S::foo() const volatile &&", "S::foo");
    DO_TEST("int const && S::foo(void)const volatile &&", "S::foo");

    DO_TEST("auto main::'lambda'(auto const volatile&&)::operator()<'lambda'(auto const volatile&&)>(this auto const volatile&&)", "main::<lambda>::operator()");
    DO_TEST("auto main::{lambda(auto:1 const volatile&&)#1}::operator()<{lambda(auto:1 const volatile&&)#1}>(this {lambda(auto:1 const volatile&&)#1} const volatile&&)", "main::<lambda#1>::operator()");
    DO_TEST("static auto `int main(void)'::`2'::<lambda_1>::operator()<`int main(void)'::`2'::<lambda_1> >(UNKNOWN,`int main(void)'::`2'::<lambda_1> const volatile &&)", "`main'::`2'::<lambda_1>::operator()");
}

TEST(PruneSymbolTests, ConversionOperator) {
    // https://godbolt.org/z/v8hc1vb9P
    DO_TEST("S::operator int()", "S::operator int");
    DO_TEST("S::operator void*()", "S::operator void*");
    DO_TEST("S::operator std::nullptr_t()", "S::operator std::nullptr_t");
    DO_TEST("S::operator X<int>()", "S::operator X");
    DO_TEST("S::operator ns::Z()", "S::operator ns::Z");
    DO_TEST("S::operator ns::Y<int><int>()", "S::operator ns::Y");
    DO_TEST("S::operator main::'lambda'()<main::'lambda'()>()", "S::operator main::<lambda>");
    DO_TEST("S::operator main::'lambda'()<main::'lambda'()>()::'lambda'()::operator()() const", "S::operator main::<lambda>::<lambda>::operator()");

    DO_TEST("S::operator decltype(nullptr)()", "S::operator decltype(nullptr)");
    DO_TEST("S::operator main::{lambda()#1}<main::{lambda()#1}>()", "S::operator main::<lambda#1>");
    DO_TEST("S::operator main::{lambda()#1}<main::{lambda()#1}>()::{lambda()#1}::operator()() const", "S::operator main::<lambda#1>::<lambda#1>::operator()");

    DO_TEST("S::operator main::$_0<main::$_0>()", "S::operator main::$_0");
    DO_TEST("S::operator main::$_0<main::$_0>()::'lambda'()::operator()() const", "S::operator main::$_0::<lambda>::operator()");

    // https://godbolt.org/z/4qqP9reqr
    DO_TEST("S::operator S::operator*()::X<S::operator*()::X>()", "S::operator S::operator*::X");
    // DO_TEST("S::operator<`S::operator*(void)'::`2'::X> `S::operator*(void)'::`2'::X(void)", "S::operator `S::operator*'::`2'::X"); // TODO FAIL

    DO_TEST("S::operator T&()", "S::operator T&");
    DO_TEST("S::operator T&&()", "S::operator T&&");

    // https://godbolt.org/z/PTe1xh6Go
    DO_TEST("S::operator int X::*()", "S::operator int X::*");
    DO_TEST("S::operator int Y<int>::*()", "S::operator int Y::*");

    DO_TEST("S::operator std::vector<int> X::*()", "S::operator std::vector X::*");
    DO_TEST("S::operator std::vector<int> Y<int>::*()", "S::operator std::vector Y::*");
    DO_TEST("S::operator std::vector<int> ns<X>::ns::X::*()", "S::operator std::vector ns::ns::X::*");
    DO_TEST("S::operator std::vector<int> ns<X>::ns::Y<int>::*()", "S::operator std::vector ns::ns::Y::*");
    DO_TEST("S::operator std::vector<int> ns<X>::ns::X::*()::test", "S::operator std::vector ns::ns::X::*::test");
    DO_TEST("S::operator std::vector<int> ns<X>::ns::Y<int>::*()::test", "S::operator std::vector ns::ns::Y::*::test");
}

TEST(PruneSymbolTests, DeducedConversionOperator) {
    // https://godbolt.org/z/9rzdKvGh7
    DO_TEST("S<float>::operator auto()", "S::operator auto");
    DO_TEST("S<float>::operator auto()", "S::operator auto");
    DO_TEST("S<float>::operator decltype(auto)()", "S::operator decltype(auto)");

    // DO_TEST("S<float>::operator (void)", "S::operator"); // Microsoft's lovely demangling
    // DO_TEST("S<float>::operator (void)::test", "S::operator::test");
    DO_TEST("S<float>::operator float(void)", "S::operator float");
}

TEST(PruneSymbolTests, FunctionPointers) {
    // https://godbolt.org/z/TWfa4f6Kc
    DO_TEST("void (*foo<int>())(int, double)", "foo");
    DO_TEST("void (**foo<int>())(int, double)", "foo");
    DO_TEST("void (&baz<int>())(int, double)", "baz");
    DO_TEST("void (** (**bar<int>())(int, double))(int, double)", "bar");
    DO_TEST("void (**(**bar<int>())(int, double))(int, double)", "bar");
    DO_TEST("void (__cdecl** foo<int>(void))(int,double)", "foo");
    DO_TEST("void (__cdecl** (__cdecl** bar<int>(void))(int,double))(int,double)", "bar");
    DO_TEST("void (__cdecl&baz<int>(void))(int,double)", "baz");
}

TEST(PruneSymbolTests, UnnamedTypes) {
    // https://godbolt.org/z/jx8GnrW4v
    DO_TEST("main::'unnamed'::foo()", "main::<unnamed>::foo");
    DO_TEST("main::{unnamed type#1}::foo()", "main::<unnamed type#1>::foo");
}

TEST(PruneSymbolTests, TemplateHeavySymbols) {
    // https://godbolt.org/z/z1nrMsYfs
    DO_TEST("__find_if<__gnu_cxx::__normal_iterator<int*, std::vector<int> >, __gnu_cxx::__ops::_Iter_pred<main()::<lambda(auto:19)> > >", "__find_if");
    // DO_TEST("operator()<__gnu_cxx::__normal_iterator<int*, std::vector<int> >, __gnu_cxx::__normal_iterator<int*, std::vector<int> >, std::identity, main()::<lambda(auto:18)> >", "ns::SS::SS");
    DO_TEST("std::__1::find_if[abi:ne200100]<std::__1::__wrap_iter<int*>, main::$_1>(std::__1::__wrap_iter<int*>, std::__1::__wrap_iter<int*>, main::$_1)", "std::__1::find_if");
    DO_TEST("std::__1::_IfImpl<borrowed_range<T>>::_Select<decltype(std::__1::ranges::__cpo::begin(std::declval<T&>())), std::__1::ranges::dangling> std::__1::ranges::__find_if::operator()[abi:ne200100]<std::__1::vector<int, std::__1::allocator<int>>&, std::__1::identity, main::$_0>(T&&, main::$_0, T0) const", "std::__1::ranges::__find_if::operator()");
    DO_TEST("std::__1::ranges::__find_if_impl[abi:ne200100]<std::__1::__wrap_iter<int*>, std::__1::__wrap_iter<int*>, main::$_0, std::__1::identity>(std::__1::__wrap_iter<int*>, std::__1::__wrap_iter<int*>, main::$_0&, std::__1::identity&)", "std::__1::ranges::__find_if_impl");
}

TEST(PruneSymbolTests, StorageClasses) {
    // https://godbolt.org/z/xPYKW8Pz5
    DO_TEST("static void S::foo(void)", "S::foo");
}

TEST(PruneSymbolTests, Noexcept) {
    // https://godbolt.org/z/xjsM67s17
    DO_TEST("void foo<X>(X (*)() noexcept(X::n))", "foo");
}

TEST(PruneSymbolTests, MiscNesting) {
    // https://godbolt.org/z/5Gj99ernr
    DO_TEST("void use1<5>(Wrapper<(5)<(5)>)", "use1");
    DO_TEST("void use2<5>(Wrapper<((5)>(5))>)", "use2");
    DO_TEST("void use1<5>(Wrapper<5 < 5>)", "use1");
    DO_TEST("void use2<5>(Wrapper<(5 > 5)>)", "use2");
    DO_TEST("void use1<5>(Wrapper<0>)", "use1");
    DO_TEST("void use2<5>(Wrapper<0>)", "use2");
}

TEST(PruneSymbolTests, Misc) {
    // https://godbolt.org/z/cqfW7MK57
    DO_TEST("foo(...)", "foo");
}

TEST(PruneSymbolTests, Extra) {
    DO_TEST("operator<<(std::ostream&, Foo const&)", "operator<<");
    DO_TEST("std::ostream & operator<<(std::ostream &, Foo const &)", "operator<<");
    DO_TEST("Foo::operator+=(int)", "Foo::operator+=");
    DO_TEST("Foo::operator+=(int) const", "Foo::operator+=");
    DO_TEST("Foo::operator bool() const noexcept", "Foo::operator bool");
    DO_TEST("Foo::operator int() const", "Foo::operator int");

    DO_TEST("Foo::bar() const", "Foo::bar");
    DO_TEST("Foo::bar() volatile", "Foo::bar");
    DO_TEST("Foo::bar() const &", "Foo::bar");
    DO_TEST("Foo::bar() &&", "Foo::bar");
    DO_TEST("Foo::bar() const noexcept", "Foo::bar");
    DO_TEST("Foo::bar() const & noexcept", "Foo::bar");

    DO_TEST("void foo<3>()", "foo");
    DO_TEST("void foo<'a'>()", "foo");
    DO_TEST("void foo<true>()", "foo");
    DO_TEST("void foo<&Foo::bar>()", "foo");
    DO_TEST("void foo<(int)5>()", "foo");

    DO_TEST("main::{lambda()#1}::operator()() const", "main::<lambda#1>::operator()");
    DO_TEST("main::$_0::operator()() const", "main::$_0::operator()");

    DO_TEST("virtual void __cdecl ns::Foo::bar(int) const", "ns::Foo::bar");
    DO_TEST("static int __cdecl ns::Foo::baz(char)", "ns::Foo::baz");
    DO_TEST("__thiscall Foo::Foo(void)", "Foo::Foo");
    DO_TEST("virtual __thiscall Foo::~Foo(void)", "Foo::~Foo");

    DO_TEST("virtual thunk to Foo::bar()", "Foo::bar");

    DO_TEST("void foo<std::vector<int,std::allocator<int>>>(std::vector<int,std::allocator<int>> const&)", "foo");
    DO_TEST("void foo<std::basic_string<char>>(std::basic_string<char> const&)", "foo");
}

TEST(PruneSymbolTests, RegressionTests) {
    // Symbols I've observed not pruning correctly
    DO_TEST("void (* const&std::_Any_data::_M_access<void (*)(cpptrace::v1::log_level, char const*)>() const)(cpptrace::v1::log_level, char const*)", "std::_Any_data::_M_access");
    DO_TEST("void (* const*std::__addressof<void (* const)(cpptrace::v1::log_level, char const*)>(void (* const&)(cpptrace::v1::log_level, char const*)))(cpptrace::v1::log_level, char const*)", "std::__addressof");
    DO_TEST("void (* const&std::forward<void (* const&)(cpptrace::v1::log_level, char const*)>(std::remove_reference<void (* const&)(cpptrace::v1::log_level, char const*)>::type&))(cpptrace::v1::log_level, char const*)", "std::forward");
    DO_TEST("fmt::v10::detail::parse_format_specs<char>(char const*, char const*, fmt::v10::detail::dynamic_format_specs<char>&, fmt::v10::basic_format_parse_context<char>&, fmt::v10::detail::type)::{unnamed type#1}::operator()(fmt::v10::detail::state, bool)", "fmt::v10::detail::parse_format_specs::<unnamed type#1>::operator()");
    DO_TEST("fmt::v10::detail::parse_format_specs<char>(char const*, char const*, fmt::v10::detail::dynamic_format_specs<char>&, fmt::v10::basic_format_parse_context<char>&, fmt::v10::detail::type)::{unnamed type#2}::operator()(fmt::v10::presentation_type, int)", "fmt::v10::detail::parse_format_specs::<unnamed type#2>::operator()");
}

}

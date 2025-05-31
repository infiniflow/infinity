#ifndef TRACING_COMMON_HPP
#define TRACING_COMMON_HPP

template<typename...> using void_t = void;

#ifndef CPPTRACE_BUILD_NO_SYMBOLS
#define EXPECT_FILE(A, B) EXPECT_THAT((A), testing::EndsWith(B))
#define EXPECT_LINE(A, B) EXPECT_EQ((A), (B))
#else
#define EXPECT_FILE(A, B) (void_t<decltype(A), decltype(B)>)0
#define EXPECT_LINE(A, B) (void_t<decltype(A), decltype(B)>)0
#endif

#endif

/*
 * Copyright (C) 2021 Tai Chi Minh Ralph Eastwood
 * All rights reserved
 *
 * This file is part of zsv/lib, distributed under the license defined at
 * https://opensource.org/licenses/MIT
 */

#ifndef ZSV_UTILS_COMPILER
#define ZSV_UTILS_COMPILER

#ifdef HAVE___BUILTIN_EXPECT

# ifndef LIKELY
# define LIKELY(x) __builtin_expect(x, 1)
# endif

# ifndef UNLIKELY
# define UNLIKELY(x) __builtin_expect(x, 0)
# endif

# ifndef VERY_LIKELY
#  ifdef NO___BUILTIN_EXPECT_WITH_PROBABILITY
#   define VERY_LIKELY(x) LIKELY(x)
#  else
#   define VERY_LIKELY(x) __builtin_expect_with_probability(x, 1, 0.999)
#  endif
# endif

# ifndef VERY_UNLIKELY
#  ifdef NO___BUILTIN_EXPECT_WITH_PROBABILITY
#   define VERY_UNLIKELY(x) UNLIKELY(x)
#  else
#   define VERY_UNLIKELY(x) __builtin_expect_with_probability(x, 0, 0.999)
#  endif
# endif

#else
/* no HAVE___BUILTIN_EXPECT */
# ifndef LIKELY
# define LIKELY(x) (x)
# endif

# ifndef UNLIKELY
# define UNLIKELY(x) (x)
# endif

# ifndef VERY_LIKELY
# define VERY_LIKELY(x) (x)
# endif

# ifndef VERY_UNLIKELY
# define VERY_UNLIKELY(x) (x)
# endif

#endif /* HAVE___BUILTIN_EXPECT */

#endif

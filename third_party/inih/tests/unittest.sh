#!/usr/bin/env bash

gcc -Wall ../ini.c unittest.c -o unittest_multi
./unittest_multi > baseline_multi.txt
rm -f unittest_multi

gcc -Wall ../ini.c -DINI_MAX_LINE=20 unittest.c -o unittest_multi_max_line
./unittest_multi_max_line > baseline_multi_max_line.txt
rm -f unittest_multi_max_line

gcc -Wall ../ini.c -DINI_ALLOW_MULTILINE=0 unittest.c -o unittest_single
./unittest_single > baseline_single.txt
rm -f unittest_single

gcc -Wall ../ini.c -DINI_ALLOW_INLINE_COMMENTS=0 unittest.c -o unittest_disallow_inline_comments
./unittest_disallow_inline_comments > baseline_disallow_inline_comments.txt
rm -f unittest_disallow_inline_comments

gcc -Wall ../ini.c -DINI_STOP_ON_FIRST_ERROR=1 unittest.c -o unittest_stop_on_first_error
./unittest_stop_on_first_error > baseline_stop_on_first_error.txt
rm -f unittest_stop_on_first_error

gcc -Wall ../ini.c -DINI_HANDLER_LINENO=1 unittest.c -o unittest_handler_lineno
./unittest_handler_lineno > baseline_handler_lineno.txt
rm -f unittest_handler_lineno

gcc -Wall ../ini.c -DINI_MAX_LINE=20 unittest_string.c -o unittest_string
./unittest_string > baseline_string.txt
rm -f unittest_string

gcc -Wall ../ini.c -DINI_USE_STACK=0 unittest.c -o unittest_heap
./unittest_heap > baseline_heap.txt
rm -f unittest_heap

gcc -Wall ../ini.c -DINI_USE_STACK=0 -DINI_MAX_LINE=20 -DINI_INITIAL_ALLOC=20 unittest.c -o unittest_heap_max_line
./unittest_heap_max_line > baseline_heap_max_line.txt
rm -f unittest_heap_max_line

gcc -Wall ../ini.c -DINI_USE_STACK=0 -DINI_ALLOW_REALLOC=1 -DINI_INITIAL_ALLOC=5 unittest.c -o unittest_heap_realloc
./unittest_heap_realloc > baseline_heap_realloc.txt
rm -f unittest_heap_realloc

gcc -Wall ../ini.c -DINI_USE_STACK=0 -DINI_MAX_LINE=20 -DINI_ALLOW_REALLOC=1 -DINI_INITIAL_ALLOC=5 unittest.c -o unittest_heap_realloc_max_line
./unittest_heap_realloc_max_line > baseline_heap_realloc_max_line.txt
rm -f unittest_heap_realloc_max_line

gcc -Wall ../ini.c -DINI_USE_STACK=0 -DINI_MAX_LINE=20 -DINI_INITIAL_ALLOC=20 unittest_string.c -o unittest_heap_string
./unittest_heap_string > baseline_heap_string.txt
rm -f unittest_heap_string

gcc -Wall ../ini.c -DINI_CALL_HANDLER_ON_NEW_SECTION=1 unittest.c -o unittest_call_handler_on_new_section
./unittest_call_handler_on_new_section > baseline_call_handler_on_new_section.txt
rm -f unittest_call_handler_on_new_section

gcc -Wall ../ini.c -DINI_ALLOW_NO_VALUE=1 unittest.c -o unittest_allow_no_value
./unittest_allow_no_value > baseline_allow_no_value.txt
rm -f unittest_allow_no_value

gcc -Wall -DINI_CUSTOM_ALLOCATOR=1 -DINI_USE_STACK=0 -DINI_ALLOW_REALLOC=1 -DINI_INITIAL_ALLOC=12 ../ini.c unittest_alloc.c -o unittest_alloc
./unittest_alloc > baseline_alloc.txt
rm -f unittest_alloc

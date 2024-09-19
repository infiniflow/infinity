@call tcc ..\ini.c -I..\ -run unittest.c > baseline_multi.txt
@call tcc ..\ini.c -I..\ -DINI_MAX_LINE=20 -run unittest.c > baseline_multi_max_line.txt
@call tcc ..\ini.c -I..\ -DINI_ALLOW_MULTILINE=0 -run unittest.c > baseline_single.txt
@call tcc ..\ini.c -I..\ -DINI_ALLOW_INLINE_COMMENTS=0 -run unittest.c > baseline_disallow_inline_comments.txt
@call tcc ..\ini.c -I..\ -DINI_STOP_ON_FIRST_ERROR=1 -run unittest.c > baseline_stop_on_first_error.txt
@call tcc ..\ini.c -I..\ -DINI_HANDLER_LINENO=1 -run unittest.c > baseline_handler_lineno.txt
@call tcc ..\ini.c -I..\ -DINI_USE_STACK=0 -run unittest.c > baseline_heap.txt
@call tcc ..\ini.c -I..\ -DINI_USE_STACK=0 -DINI_MAX_LINE=20 -DINI_INITIAL_ALLOC=20 -run unittest.c > baseline_heap_max_line.txt
@call tcc ..\ini.c -I..\ -DINI_USE_STACK=0 -DINI_ALLOW_REALLOC=1 -DINI_INITIAL_ALLOC=5 -run unittest.c > baseline_heap_realloc.txt
@call tcc ..\ini.c -I..\ -DINI_USE_STACK=0 -DINI_MAX_LINE=20 -DINI_ALLOW_REALLOC=1 -DINI_INITIAL_ALLOC=5 -run unittest.c > baseline_heap_realloc_max_line.txt
@call tcc ..\ini.c -I..\ -DINI_USE_STACK=0 -DINI_MAX_LINE=20 -DINI_INITIAL_ALLOC=20 -run unittest.c > baseline_heap_string.txt
@call tcc ..\ini.c -I..\ -DINI_CALL_HANDLER_ON_NEW_SECTION=1 -run unittest.c > baseline_call_handler_on_new_section.txt
@call tcc ..\ini.c -I..\ -DINI_ALLOW_NO_VALUE=1 -run unittest.c > baseline_allow_no_value.txt
@call tcc ..\ini.c -I..\ -DINI_CUSTOM_ALLOCATOR=1 -DINI_USE_STACK=0 -DINI_ALLOW_REALLOC=1 -DINI_INITIAL_ALLOC=12 -run unittest_alloc.c > baseline_alloc.txt

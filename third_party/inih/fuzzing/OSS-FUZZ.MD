### oss-fuzz local build with ASAN

```
export CC=clang
export CFLAGS="-O1 -fno-omit-frame-pointer -gline-tables-only -DFUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION -fsanitize=address -fsanitize-address-use-after-scope -fsanitize=fuzzer-no-link"
export LIB_FUZZING_ENGINE="-fsanitize=fuzzer"
```

### oss-fuzz local build with MSAN

```
export CC=clang
export CFLAGS="-O1 -fno-omit-frame-pointer -gline-tables-only -DFUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION -fsanitize=memory -fsanitize-memory-track-origins -fsanitize=fuzzer-no-link"
export LIB_FUZZING_ENGINE="-fsanitize=fuzzer"
```


### oss-fuzz local build with UBSAN
```
export CC=clang
export CFLAGS="-O1 -fno-omit-frame-pointer -gline-tables-only -DFUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION -fsanitize=array-bounds,bool,builtin,enum,float-divide-by-zero,function,integer-divide-by-zero,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,unsigned-integer-overflow,unreachable,vla-bound,vptr -fno-sanitize-recover=array-bounds,bool,builtin,enum,float-divide-by-zero,function,integer-divide-by-zero,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,unreachable,vla-bound,vptr -fsanitize=fuzzer-no-link"
export LIB_FUZZING_ENGINE="-fsanitize=fuzzer"
```

### run fuzzer
```
bash oss-fuzz.sh
mkdir seed/
cp ../tests/*.ini seed/
./inihfuzz seed/
```

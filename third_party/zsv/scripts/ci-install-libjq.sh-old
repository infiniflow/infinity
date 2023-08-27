#!/bin/sh

set -e

echo "[INF] Running $0"

echo "[INF] Building and installing jq (libjq)"

echo "[INF] Will apply a patch to jq so that when compiling with mingw64,"
echo "[INF] jq will not create its own functions that conflict with pthread functions"
echo "[INF] and which in turn prevent mingw64 from statically linking libwinpthread-1.dll"
echo "[INF] If you are building on your target system, this is not necessary, and the"
echo "[INF] standard jq distro can be used"

THIS_SCRIPT_PATH=$(dirname "$0")
THIS_SCRIPT_PATH=$(cd "$THIS_SCRIPT_PATH" && pwd)
JQ_PATCH="$THIS_SCRIPT_PATH/../app/external/jq.diff"
JQ_GIT_URL='https://github.com/stedolan/jq.git'
JQ_GIT_COMMIT='cff5336'
JQ_DIR="$PWD/jq"
JQ_PREFIX="$JQ_DIR/build"
JQ_INCLUDE_DIR="$JQ_PREFIX/include"
JQ_LIB_DIR="$JQ_PREFIX/lib"

echo "[INF] PWD:              $PWD"
echo "[INF] CC:               $CC"
echo "[INF] JQ_PATCH:    $JQ_PATCH"
echo "[INF] JQ_GIT_URL:       $JQ_GIT_URL"
echo "[INF] JQ_GIT_COMMIT:    $JQ_GIT_COMMIT"
echo "[INF] JQ_DIR:           $JQ_DIR"
echo "[INF] JQ_PREFIX:        $JQ_PREFIX"
echo "[INF] JQ_INCLUDE_DIR:   $JQ_INCLUDE_DIR"
echo "[INF] JQ_LIB_DIR:       $JQ_LIB_DIR"

rm -rf "$JQ_DIR"

echo "[INF] Adding git safe.directory to avoid 'dubious ownership' error"
git config --global --add safe.directory /Users/runner/work/zsv/zsv/jq
git clone "$JQ_GIT_URL"

cd jq
git checkout "$JQ_GIT_COMMIT"
echo "[INF] Applying patch"
git apply "$JQ_PATCH" || exit 1

echo "[INF] Configuring"
autoreconf -fi

if [ "$CC" = 'x86_64-w64-mingw32-gcc' ]; then
  CFLAGS='-O3' ./configure \
    --prefix="$JQ_PREFIX" \
    --disable-maintainer-mode \
    --without-oniguruma \
    --disable-docs \
    --disable-shared \
    --enable-static \
    --host=x86_64-w64-mingw32
else
  CFLAGS='-O3' ./configure \
    --prefix="$JQ_PREFIX" \
    --disable-maintainer-mode \
    --without-oniguruma \
    --disable-docs \
    --disable-shared \
    --enable-static
fi

echo "[INF] Building and installing"
make install

cd ..

echo "[INF] --- [DONE] ---"

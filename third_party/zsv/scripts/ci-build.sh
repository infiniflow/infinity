#!/bin/sh

set -e

echo "[INF] Running $0"

if [ "$PREFIX" = "" ] || [ "$CC" = "" ] || [ "$MAKE" = "" ] || [ "$ARTIFACT_DIR" = "" ]; then
  echo "[ERR] One or more environment variable(s) are not set!"
  echo "[ERR] Set PREFIX, CC, MAKE, and ARTIFACT_DIR before running $0 script."
  exit 1
fi

if [ ! -d "$ARTIFACT_DIR" ]; then
  echo "[WRN] Artifact directory not found! [$ARTIFACT_DIR]"
  echo "[WRN] Artifact directory will be created!"
fi

if [ "$RUN_TESTS" != true ]; then
  RUN_TESTS=false
fi

#JQ_DIR="$PWD/jq"
#JQ_PREFIX="$JQ_DIR/build"
#JQ_INCLUDE_DIR="$JQ_PREFIX/include"
#JQ_LIB_DIR="$JQ_PREFIX/lib"

echo "[INF] Building and generating artifacts"

echo "[INF] PWD:              $PWD"
echo "[INF] PREFIX:           $PREFIX"
echo "[INF] CC:               $CC"
echo "[INF] MAKE:             $MAKE"
echo "[INF] RUN_TESTS:        $RUN_TESTS"
echo "[INF] ARTIFACT_DIR:     $ARTIFACT_DIR"
#echo "[INF] JQ_DIR:           $JQ_DIR"
#echo "[INF] JQ_PREFIX:        $JQ_PREFIX"
#echo "[INF] JQ_INCLUDE_DIR:   $JQ_INCLUDE_DIR"
#echo "[INF] JQ_LIB_DIR:       $JQ_LIB_DIR"

echo "[INF] Listing compiler version [$CC]"
"$CC" --version

# ./scripts/ci-install-libjq.sh

echo "[INF] Configuring zsv"
# CFLAGS="-I$JQ_INCLUDE_DIR" LDFLAGS="-L$JQ_LIB_DIR" 
./configure \
  --prefix="$PREFIX" \
  --disable-termcap
#  --enable-jq

if [ "$RUN_TESTS" = true ]; then
  echo "[INF] Running tests"
  rm -rf build "$PREFIX"
  "$MAKE" test
  echo "[INF] Tests completed successfully!"

  echo "[INF] Configuring example extension and running example extension tests"
  (cd app/ext_example && "$MAKE" CONFIGFILE=../../config.mk test)
  echo "[INF] Tests completed successfully!"
fi

echo "[INF] Building"
rm -rf build "$PREFIX" /usr/local/etc/zsv.ini
"$MAKE" install
tree -h "$PREFIX"
echo "[INF] Built successfully!"

mkdir -p "$ARTIFACT_DIR"

ZIP="$PREFIX.zip"
echo "[INF] Compressing [$ZIP]"
cd "$PREFIX"
zip -r "$ZIP" .
ls -Gghl "$ZIP"
cd ..
mv "$PREFIX/$ZIP" "$ARTIFACT_DIR"
echo "[INF] Compressed! [$ZIP]"

TAR="$PREFIX.tar.gz"
echo "[INF] Compressing [$TAR]"
tar -czvf "$TAR" "$PREFIX"
ls -Gghl "$TAR"
mv "$TAR" "$ARTIFACT_DIR"
echo "[INF] Compressed! [$TAR]"

echo "[INF] --- [DONE] ---"

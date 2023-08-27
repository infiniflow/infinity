#!/bin/sh

set -e

echo "[INF] Running $0"

if [ "$TAG" = "" ] || [ "$PREFIX" = "" ] || [ "$ARTIFACT_DIR" = "" ]; then
  echo "[ERR] One or more environment variable(s) are not set!"
  echo "[ERR] Set TAG, PREFIX and ARTIFACT_DIR before running $0 script."
  exit 1
fi

if [ ! -d "$PREFIX" ]; then
  echo "[ERR] PREFIX directory not found! [$PREFIX]"
  echo "[ERR] First build with PREFIX [$PREFIX] and then run $0."
  exit 1
fi

if [ ! -d "$ARTIFACT_DIR" ]; then
  echo "[ERR] Artifact directory not found! [$ARTIFACT_DIR]"
  exit 1
fi

ARCH="$(echo "$PREFIX" | cut -d '-' -f1)"
VERSION="$TAG"

NUGET_PKG="zsv.$VERSION.nupkg"
NUGET_PKG_PATH="$ARTIFACT_DIR/$PREFIX.nupkg"
NUGET_PKG_FEED="$PWD/nuget-feed"
NUGET_SPEC='zsv.nuspec'
NUGET_SPEC_PATH="$PREFIX/$NUGET_SPEC"

echo "[INF] Creating nuget package [$NUGET_PKG]"

echo "[INF] PWD:              $PWD"
echo "[INF] TAG:              $TAG"
echo "[INF] PREFIX:           $PREFIX"
echo "[INF] ARTIFACT_DIR:     $ARTIFACT_DIR"
echo "[INF] ARCH:             $ARCH"
echo "[INF] VERSION:          $VERSION"

echo "[INF] Creating spec file [$NUGET_SPEC_PATH]"
cat << EOF > "$NUGET_SPEC_PATH"
<?xml version="1.0"?>
<package>
  <metadata>
    <id>zsv</id>
    <version>$VERSION</version>
    <description>zsv+lib: world's fastest CSV parser, with an extensible CLI</description>
    <authors>liquidaty</authors>
    <projectUrl>https://github.com/liquidaty/zsv</projectUrl>
    <license type="expression">MIT</license>
    <requireLicenseAcceptance>false</requireLicenseAcceptance>
    <tags>native</tags>
  </metadata>

  <files>
    <file src="bin/zsv.exe" target="native/bin" />
  </files>
</package>
EOF

echo "[INF] Dumping [$NUGET_SPEC_PATH]"
echo "[INF] --- [$NUGET_SPEC_PATH] ---"
cat -n "$NUGET_SPEC_PATH"
echo "[INF] --- [$NUGET_SPEC_PATH] ---"

tree "$PREFIX"

echo "[INF] Building [$NUGET_PKG]"
nuget pack "$NUGET_SPEC_PATH"
rm -f "$NUGET_SPEC"

echo "[INF] Renaming [$NUGET_PKG => $NUGET_PKG_PATH]"
mv "$NUGET_PKG" "$NUGET_PKG_PATH"

echo "[INF] Verifying nuget package [$NUGET_PKG_PATH]"

echo "[INF] Installing"
mkdir "$NUGET_PKG_FEED"
nuget add "$NUGET_PKG_PATH" -source "$NUGET_PKG_FEED"
nuget install zsv -version "$VERSION" -source "$NUGET_PKG_FEED"

echo "[INF] Verifying installed package"
NUGET_PKG_INSTALLED_DIR="zsv.$VERSION"
tree "$NUGET_PKG_INSTALLED_DIR"

echo "[INF] Uninstalling"
nuget delete zsv "$VERSION" -source "$NUGET_PKG_FEED" -NonInteractive

rm -rf "$NUGET_PKG_INSTALLED_DIR" "$NUGET_PKG_FEED"

echo "[INF] --- [DONE] ---"

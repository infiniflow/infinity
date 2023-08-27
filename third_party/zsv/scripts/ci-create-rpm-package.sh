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
VERSION="$(echo "$TAG" | cut -c1- | cut -d '-' -f1)"

RPM_DIR="$HOME/rpmbuild"
RPM_PKG="$PREFIX.rpm"
RPM_PKG_PATH="$RPM_DIR/RPMS/$RPM_PKG"
RPM_SPEC='zsv.spec'
RPM_SPEC_PATH="$RPM_DIR/SPECS/$RPM_SPEC"

echo "[INF] Creating rpm package [$RPM_PKG]"

echo "[INF] PWD:              $PWD"
echo "[INF] TAG:              $TAG"
echo "[INF] PREFIX:           $PREFIX"
echo "[INF] ARTIFACT_DIR:     $ARTIFACT_DIR"
echo "[INF] ARCH:             $ARCH"
echo "[INF] VERSION:          $VERSION"

echo "[INF] Listing linked libraries"
ldd "$PREFIX/bin/zsv"

echo "[INF] Setting up RPM buildtree [$RPM_DIR]"
rm -rf "$RPM_DIR"
mkdir -p "$RPM_DIR/BUILD/usr" "$RPM_DIR/SPECS"

echo "[INF] Copying build artifacts"
rm -rf "./$PREFIX/lib" "./$PREFIX/include"
cp -rfa "$PREFIX/bin" "$RPM_DIR/BUILD/usr/"

echo "[INF] Creating spec file [$RPM_SPEC_PATH]"
cat << EOF > "$RPM_SPEC_PATH"
%define _build_id_links none
%define _rpmfilename $RPM_PKG

Name: zsv
Version: $VERSION
Release: 1%{?dist}
Summary: zsv+lib: world's fastest CSV parser, with an extensible CLI
License: MIT
URL: https://github.com/liquidaty/zsv
Vendor: Liquidaty <liquidaty@users.noreply.github.com>

%description
zsv+lib: world's fastest CSV parser, with an extensible CLI

%install
rm -rf %{buildroot}
cp -rfa %{_builddir} %{buildroot}
tree %{buildroot}

%clean
rm -rf %{buildroot}

%files
/usr/bin/zsv
EOF

echo "[INF] Dumping [$RPM_SPEC_PATH]"
echo "[INF] --- [$RPM_SPEC_PATH] ---"
cat -n "$RPM_SPEC_PATH"
echo "[INF] --- [$RPM_SPEC_PATH] ---"

tree "$RPM_DIR"

echo "[INF] Building"
rpmbuild -v --clean -bb "$RPM_SPEC_PATH"
rpm -qlp "$RPM_PKG_PATH"

mv "$RPM_PKG_PATH" "$ARTIFACT_DIR/"
rm -rf "$RPM_DIR"

ls -Gghl "$ARTIFACT_DIR/$RPM_PKG"

echo "[INF] Verifying RPM package [$ARTIFACT_DIR/$RPM_PKG]"

echo "[INF] Installing"
sudo alien -i "./$ARTIFACT_DIR/$RPM_PKG"

echo "[INF] Verifying installed package"
whereis zsv
zsv version

echo "[INF] Uninstalling"
sudo apt remove -y zsv

echo "[INF] --- [DONE] ---"

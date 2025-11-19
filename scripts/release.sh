#!/usr/bin/bash

# Publish a release onto github, docker hub and pypi.

# You need prepare following before execute this script:
# - http/https proxy to speedup `docker push`
# - MY_GH_TOKEN (the github token shall be granted the admin permission)
# - DOCKERHUB_TOKEN (the hub.docker.com token shall be granted the read/write/delete permission)
# - PYPI_API_TOKEN (the pypi api token)

# Required CLI argument:
# - RELEASE_TAG

set -euo pipefail
set -x

RELEASE_TAG=$1
if [[ "$RELEASE_TAG" =~ ^v ]]; then
    PRERELEASE=false
else
    PRERELEASE=true
fi
GITHUB_SHA=$(git rev-parse HEAD)
REMOTE_NAME=$(git remote -v | awk '/github\.com[/:]infiniflow\/infinity/ && $3 == "(fetch)" {print $1; exit}')
RELEASE_DATETIME=$(date --rfc-3339=seconds)
BUILDER_CONTAINER=infinity_build_ubuntu20_clang20
BUILDER_IMAGE=infiniflow/infinity_builder:ubuntu20_clang20
WORKSPACE="$(git rev-parse --show-toplevel)"

function git_retag() {
    git push $REMOTE_NAME :refs/tags/$RELEASE_TAG
    git tag -f $RELEASE_TAG HEAD
    git push -f $REMOTE_NAME $RELEASE_TAG:refs/tags/$RELEASE_TAG
}

function build() {
    git submodule update --init --recursive
    if ! docker ps --format '{{.Names}}' | grep -qw "^$BUILDER_CONTAINER$"; then
        TZ=$(readlink -f /etc/localtime | awk -F '/zoneinfo/' '{print $2}')
        docker run -d --privileged --name $BUILDER_CONTAINER -e TZ=$TZ -v $HOME:$HOME -v /boot:/boot --network host --hostname $BUILDER_CONTAINER $BUILDER_IMAGE
    fi
    docker exec $BUILDER_CONTAINER bash -c "git config --global safe.directory \"*\" && cd $WORKSPACE && cmake -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCPACK_PACKAGE_VERSION=$RELEASE_TAG -DCPACK_DEBIAN_PACKAGE_ARCHITECTURE=amd64 -DCMAKE_VERBOSE_MAKEFILE=ON -S $WORKSPACE -B $WORKSPACE/cmake-build-reldeb"
    docker exec $BUILDER_CONTAINER bash -c "cmake --build $WORKSPACE/cmake-build-reldeb --target infinity"
    docker exec $BUILDER_CONTAINER bash -c "cd $WORKSPACE/cmake-build-reldeb && rm -f *.deb *.rpm *.gz && cpack"
    docker exec $BUILDER_CONTAINER bash -c "cd $WORKSPACE/python/infinity_sdk && uv build"
    docker build -t infiniflow/infinity:${RELEASE_TAG} -f scripts/Dockerfile_infinity .
}

function publish_sdk() {
    if [[ "$PRERELEASE" == "false" ]]; then
        cd $WORKSPACE/python/infinity_sdk && uv publish --token $PYPI_API_TOKEN
    else
        echo "Skipping twine upload because $RELEASE_TAG is a prerelease."
    fi
}

function publish_github() {
    gh auth login --with-token <<< $MY_GH_TOKEN
    # Check if the release already exists
    if gh release view $RELEASE_TAG > /dev/null 2>&1; then
        echo "Release $RELEASE_TAG already exists. Skipping creation."
    else
        gh release create $RELEASE_TAG --prerelease=$PRERELEASE
    fi
    gh release edit $RELEASE_TAG --notes "Release $RELEASE_TAG created from $GITHUB_SHA at $RELEASE_DATETIME"
    gh release upload $RELEASE_TAG cmake-build-reldeb/infinity-*.deb
    gh release upload $RELEASE_TAG cmake-build-reldeb/infinity-*.rpm
    gh release upload $RELEASE_TAG cmake-build-reldeb/infinity-*.tar.gz
}

function publish_image() {
    docker login --username infiniflow --password-stdin <<< $DOCKERHUB_TOKEN
    docker push infiniflow/infinity:${RELEASE_TAG}
}

git_retag
build
publish_sdk
publish_github
publish_image

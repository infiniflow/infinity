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

# Validate command line arguments
if [[ $# -ne 1 ]]; then
    echo "Error: Invalid number of arguments"
    echo "Usage: $0 <RELEASE_TAG>"
    echo ""
    echo "RELEASE_TAG must start with a letter and contain only letters, digits, dots, underscores, and hyphens."
    echo "Examples: v0.7.0, 0.7.0-dev1, test-0.7.0"
    exit 1
fi

RELEASE_TAG=$1

# Validate RELEASE_TAG format: must start with a letter and contain only alphanumeric, dot, underscore, hyphen
if [[ ! "$RELEASE_TAG" =~ ^[a-zA-Z][a-zA-Z0-9._-]*$ ]]; then
    echo "Error: Invalid RELEASE_TAG format: '$RELEASE_TAG'"
    echo ""
    echo "RELEASE_TAG must:"
    echo "  - Start with a letter (a-z or A-Z)"
    echo "  - Contain only letters, digits, dots (.), underscores (_), and hyphens (-)"
    echo ""
    echo "Examples: v0.7.0, 0.7.0-dev1, test-0.7.0"
    exit 1
fi
if [[ "$RELEASE_TAG" =~ ^v ]]; then
    PRERELEASE=false
else
    PRERELEASE=true
fi
GITHUB_SHA=$(git rev-parse HEAD)
REMOTE_NAME=$(git remote -v | awk '/github\.com[/:]infiniflow\/infinity/ && $3 == "(fetch)" {print $1; exit}')
RELEASE_DATETIME=$(date --rfc-3339=seconds)
BUILDER_CONTAINER=infinity_build_ubuntu22_clang20
BUILDER_IMAGE=infiniflow/infinity_builder:ubuntu22_clang20
WORKSPACE="$(git rev-parse --show-toplevel)"

function git_retag() {
    if [[ "$RELEASE_TAG" =~ ^v ]]; then
        # For official releases (tags starting with 'v'), check if current commit has the tag
        CURRENT_TAGS=$(git tag --points-at HEAD)
        if echo "$CURRENT_TAGS" | grep -qx "$RELEASE_TAG"; then
            echo "Current commit already has tag $RELEASE_TAG, skipping retag."
            return
        else
            echo "Error: Current commit does not have tag $RELEASE_TAG"
            if [[ -n "$CURRENT_TAGS" ]]; then
                echo "Current commit has these tags: $CURRENT_TAGS"
            else
                echo "Current commit has no tags"
            fi
            echo "Please checkout the correct commit or tag before running this script."
            exit 1
        fi
    else
        # For prereleases, allow retagging
        git push $REMOTE_NAME :refs/tags/$RELEASE_TAG
        git tag -f $RELEASE_TAG HEAD
        git push -f $REMOTE_NAME $RELEASE_TAG:refs/tags/$RELEASE_TAG
    fi
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
    cd $WORKSPACE && rm -rf dist/* && uv run python/infinity_sdk/prepare_huqie.py && uv build
    docker build -t infiniflow/infinity:${RELEASE_TAG} -f scripts/Dockerfile_infinity .
}

function publish_sdk() {
    if [[ "$PRERELEASE" == "false" ]]; then
        cd $WORKSPACE && uv publish --token $PYPI_API_TOKEN
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

    # Upload assets (will overwrite existing ones with --clobber)
    gh release upload $RELEASE_TAG cmake-build-reldeb/infinity-*.deb --clobber
    gh release upload $RELEASE_TAG cmake-build-reldeb/infinity-*.rpm --clobber
    gh release upload $RELEASE_TAG cmake-build-reldeb/infinity-*.tar.gz --clobber
}

function publish_image() {
    # If DOCKER_REGISTRY is set, push to custom registry; otherwise use Docker Hub
    if [ -n "${DOCKER_REGISTRY:-}" ]; then
        echo "Pushing to custom registry: $DOCKER_REGISTRY"
        # Tag for custom registry
        docker tag infiniflow/infinity:${RELEASE_TAG} ${DOCKER_REGISTRY}/infiniflow/infinity:${RELEASE_TAG}
        # Push to custom registry (assuming no auth needed for private registry)
        docker push ${DOCKER_REGISTRY}/infiniflow/infinity:${RELEASE_TAG}
    else
        echo "Pushing to Docker Hub"
        docker login --username infiniflow --password-stdin <<< $DOCKERHUB_TOKEN
        docker push infiniflow/infinity:${RELEASE_TAG}
    fi
}

git_retag
build
publish_sdk
publish_github
publish_image

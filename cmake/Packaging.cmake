set(CPACK_PACKAGE_NAME "infinity")

# dpkg requires version to start with a number
if (NOT DEFINED CPACK_PACKAGE_VERSION OR CPACK_PACKAGE_VERSION STREQUAL "")
    set(CPACK_PACKAGE_VERSION "${GIT_COMMIT_ID}")
endif ()

# Strip leading non-digit characters (e.g. leading 'v' from a tag like v0.7.0)
string(REGEX REPLACE "^[^0-9]+" "" CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}")
# Replace hyphens with dots so the version is a valid Debian/RPM version token
string(REPLACE "-" "." CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}")

if (CPACK_PACKAGE_VERSION STREQUAL "")
    set(CPACK_PACKAGE_VERSION "0.0.0")
endif ()

# Append the micro-architecture suffix (v2/v3/v4) for proper package differentiation
if (DEFINED PACKAGE_SUFFIX AND NOT PACKAGE_SUFFIX STREQUAL "")
    set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}.${PACKAGE_SUFFIX}")
endif ()

set(CPACK_PACKAGE_RELEASE 1)
set(CPACK_PACKAGE_CONTACT "Zhichang Yu <yuzhichang@gmail.com>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
        "The AI-native database built for LLM applications, offering incredibly fast vector and full-text search.")
set(CPACK_PACKAGE_VENDOR "infiniflow")

# ---------------------------------------------------------------------------
# Package file name: infinity-{arch}[-{suffix}]
# Use PACKAGE_ARCH for cross-compilation correctness (CMAKE_HOST_SYSTEM_PROCESSOR
# is the build host, not the target).
# ---------------------------------------------------------------------------
if (NOT DEFINED PACKAGE_ARCH)
    set(PACKAGE_ARCH "${CMAKE_HOST_SYSTEM_PROCESSOR}")
endif ()

if (DEFINED PACKAGE_SUFFIX AND NOT PACKAGE_SUFFIX STREQUAL "")
    set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${PACKAGE_ARCH}-${PACKAGE_SUFFIX}")
else ()
    set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${PACKAGE_ARCH}")
endif ()

# ---------------------------------------------------------------------------
# Install rules
# https://cmake.org/cmake/help/latest/command/install.html
# WARNING: If an absolute path is given, cpack will install the specific files
# on the host system (requires root permission) and then include them in the package.
# If a relative path (interpreted relative to CMAKE_INSTALL_PREFIX) is given,
# cpack includes specific files in the package without actually installing them.
# CMAKE_INSTALL_PREFIX defaults to "/usr/local".
# ---------------------------------------------------------------------------
set(CMAKE_INSTALL_PREFIX /usr)
install(TARGETS infinity DESTINATION bin)
install(FILES conf/infinity.service DESTINATION lib/systemd/system)
install(FILES conf/infinity_conf.toml DESTINATION etc)

# ---------------------------------------------------------------------------
# RPM generator
# https://cmake.org/cmake/help/latest/cpack_gen/rpm.html
# CPackRPM needs the absolute path of the file as CPack does not know that
# script is relative to source tree.
# ---------------------------------------------------------------------------
set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/conf/postinst")

find_program(LLVM_STRIP_EXECUTABLE
    NAMES llvm-strip-20 llvm-strip
    REQUIRED)
set(CPACK_RPM_SPEC_MORE_DEFINE "%global __strip ${LLVM_STRIP_EXECUTABLE}")

# ---------------------------------------------------------------------------
# DEB generator
# https://cmake.org/cmake/help/latest/cpack_gen/deb.html
# Note: DEB requires the file name be one of postinst, postrm, prerm and the
# "+x" permission, while rpm doesn't require that.
# ---------------------------------------------------------------------------
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/conf/postinst")

# ---------------------------------------------------------------------------
# Generators & misc
# ---------------------------------------------------------------------------
set(CPACK_GENERATOR "RPM;DEB;TGZ")

# Enable CPack debug output
set(CPACK_PACKAGE_DEBUG True)

# https://cmake.org/cmake/help/latest/variable/CPACK_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION.html
set(CPACK_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION "ON")

include(CPack)
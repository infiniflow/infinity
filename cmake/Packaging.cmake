set(CPACK_PACKAGE_NAME "infinity")

string(REGEX REPLACE "^[^0-9]+" "" CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}")

if (CPACK_PACKAGE_VERSION STREQUAL "")
    string(TIMESTAMP CURRENT_DATE "%Y%m%d")
    math(EXPR NEXT_PATCH "${PROJECT_VERSION_PATCH} + 1")
    set(NIGHTLY_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${NEXT_PATCH}")
    set(CPACK_PACKAGE_VERSION "${NIGHTLY_VERSION}~nightly.${CURRENT_DATE}")
endif ()

set(CPACK_PACKAGE_RELEASE 1)
set(CPACK_PACKAGE_CONTACT "Zhichang Yu <yuzhichang@gmail.com>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
        "The AI-native database built for LLM applications, offering incredibly fast vector and full-text search.")
set(CPACK_PACKAGE_VENDOR "infiniflow")

if (DEFINED PACKAGE_SUFFIX AND NOT PACKAGE_SUFFIX STREQUAL "")
    set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${PACKAGE_SUFFIX}")
else ()
    set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
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
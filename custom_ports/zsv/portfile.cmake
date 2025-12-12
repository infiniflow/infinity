vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO liquidaty/zsv
    REF v1.3.0
    SHA512 12e690bd685edaa93dd1113f25023209a04ff15cab67eaa063229c28e66dc1b0b3eeab89bbe13cff645fdecb3e3d3a5842c876b1d31edeb8188d3c75652f1ed6
    HEAD_REF main
)

set(ENV{CC} "gcc")
set(ENV{CXX} "g++")
vcpkg_execute_required_process(
    COMMAND "./configure" --prefix=${CURRENT_INSTALLED_DIR} --disable-test
    WORKING_DIRECTORY "${SOURCE_PATH}"
    LOGNAME configure
)

vcpkg_execute_required_process(
    COMMAND "./install.sh"
    WORKING_DIRECTORY "${SOURCE_PATH}"
    LOGNAME build
)

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
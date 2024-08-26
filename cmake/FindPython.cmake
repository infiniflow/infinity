if(NOT DEFINED Python3_EXECUTABLE)
    execute_process(
            COMMAND which python3
            RESULT_VARIABLE STATUS
            OUTPUT_VARIABLE OUTPUT
            ERROR_QUIET)
    if(STATUS EQUAL 0)
        string(STRIP ${OUTPUT} STRIPPED)
        message(STATUS "Using Python3 from 'which python3': ${STRIPPED}")
        set(Python3_EXECUTABLE ${STRIPPED})
    endif()
endif()

set (Python3_USE_STATIC_LIBS "ON")
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
message(STATUS "Python3 specified. Version found: " ${Python3_VERSION})
set(Python_EXECUTABLE ${Python3_EXECUTABLE})
message(STATUS "Using Python executable: " ${Python_EXECUTABLE})

find_package(Python3 COMPONENTS Development Module)
if(Python3_Development_FOUND AND Python3_INCLUDE_DIRS)
    set(Python_INCLUDE_DIRS ${Python3_INCLUDE_DIRS})
endif()

if(NOT Python_INCLUDE_DIRS)
    message(STATUS "Getting python include directory from sysconfig..")
    execute_process(
            COMMAND ${Python_EXECUTABLE} -c "import sysconfig; print(sysconfig.get_paths()['include'])"
            OUTPUT_VARIABLE Python_INCLUDE_DIRS RESULT_VARIABLE ret_code)
    string(STRIP "${Python_INCLUDE_DIRS}" Python_INCLUDE_DIRS)
    if((NOT (ret_code EQUAL "0")) OR (NOT IS_DIRECTORY ${Python_INCLUDE_DIRS})
            OR (NOT EXISTS ${Python_INCLUDE_DIRS}/Python.h))
        set(Python_INCLUDE_DIRS "")
    endif()
endif()

if(NOT Python_INCLUDE_DIRS)
    message(FATAL_ERROR "Cannot find python include directory")
endif()

message(STATUS "Found python include directory ${Python_INCLUDE_DIRS}")

if(NOT Python3_LIBRARIES)
    message(STATUS "Getting python library from sysconfig..")
    execute_process(
            COMMAND ${Python_EXECUTABLE} -c
            "
            import sysconfig
            import os
            import platform
            import sys
            vars = sysconfig.get_config_vars()
            if platform.system() == 'Windows':
                libpython_suffix = 'dll'
            else:
                libpython_suffix = 'a'
            python_version = f'python{sys.version_info.major}.{sys.version_info.minor}'
            libpython_name = f'lib{python_version}.{libpython_suffix}'
            base_dir = sysconfig.get_config_var('base')
            platform_dir = f'config-{sys.version_info.major}.{sys.version_info.minor}-{platform.machine()}-linux-gnu'
            libpython_path = os.path.join(base_dir, 'lib', python_version, platform_dir, libpython_name)
            print(libpython_path))
            "
            OUTPUT_VARIABLE Python3_LIBRARIES RESULT_VARIABLE ret_code)
    string(STRIP "${Python3_LIBRARIES}" Python3_LIBRARIES)
    if((NOT (ret_code EQUAL "0")) OR (NOT ${Python3_LIBRARIES}))
        set(Python_INCLUDE_DIRS "")
    endif()
endif()

if(NOT Python3_LIBRARIES)
    message(FATAL_ERROR "Cannot find python static library")
endif()

message(STATUS "Found python static library ${Python3_LIBRARIES}")
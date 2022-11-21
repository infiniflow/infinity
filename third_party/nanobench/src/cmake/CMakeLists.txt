# NOTE: This helper function assumes no generator expressions are used for the source files
# source: https://crascit.com/2016/01/31/enhanced-source-file-handling-with-target_sources/
function(target_sources_local target)
  if(POLICY CMP0076)
    # New behavior is available, so just forward to it by ensuring
    # that we have the policy set to request the new behavior, but
    # don't change the policy setting for the calling scope
    cmake_policy(PUSH)
    cmake_policy(SET CMP0076 NEW)
    target_sources(${target} ${ARGN})
    cmake_policy(POP)
    return()
  endif()

  # Must be using CMake 3.12 or earlier, so simulate the new behavior
  unset(_srcList)
  get_target_property(_targetSourceDir ${target} SOURCE_DIR)

  foreach(src ${ARGN})
    if(NOT src STREQUAL "PRIVATE" AND
       NOT src STREQUAL "PUBLIC" AND
       NOT src STREQUAL "INTERFACE" AND
       NOT IS_ABSOLUTE "${src}")
      # Relative path to source, prepend relative to where target was defined
      file(RELATIVE_PATH src "${_targetSourceDir}" "${CMAKE_CURRENT_LIST_DIR}/${src}")
    endif()
    list(APPEND _srcList ${src})
  endforeach()
  target_sources(${target} ${_srcList})
endfunction()

MESSAGE(STATUS "${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}, C++${CMAKE_CXX_STANDARD}, CMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}")

function(add_compile_flags_target target)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        if(NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.0)
            target_compile_options(${target} PRIVATE -fdiagnostics-color)
            target_compile_options(${target} PRIVATE -Wno-zero-as-null-pointer-constant) # doesn't work with spaceship operator
        endif()
        target_compile_options(${target} PRIVATE -Werror)
        target_compile_options(${target} PRIVATE -pedantic)
        target_compile_options(${target} PRIVATE -pedantic-errors)
        target_compile_options(${target} PRIVATE -fvisibility=hidden)
        target_compile_options(${target} PRIVATE -fstrict-aliasing)
    endif()

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        target_compile_options(${target} PRIVATE -Wall)
        target_compile_options(${target} PRIVATE -Wextra)
        target_compile_options(${target} PRIVATE -fdiagnostics-show-option)
        target_compile_options(${target} PRIVATE -Wconversion)
        target_compile_options(${target} PRIVATE -Wold-style-cast)
        target_compile_options(${target} PRIVATE -Wfloat-equal)
        target_compile_options(${target} PRIVATE -Wlogical-op)
        target_compile_options(${target} PRIVATE -Wundef)
        target_compile_options(${target} PRIVATE -Wredundant-decls)
        target_compile_options(${target} PRIVATE -Wshadow)
        target_compile_options(${target} PRIVATE -Wstrict-overflow=2)
        target_compile_options(${target} PRIVATE -Wwrite-strings)
        target_compile_options(${target} PRIVATE -Wpointer-arith)
        target_compile_options(${target} PRIVATE -Wcast-qual)
        target_compile_options(${target} PRIVATE -Wformat=2)
        # target_compile_options(${target} PRIVATE -Wswitch-default)
        target_compile_options(${target} PRIVATE -Wmissing-include-dirs)
        target_compile_options(${target} PRIVATE -Wcast-align)
        target_compile_options(${target} PRIVATE -Wswitch-enum)
        target_compile_options(${target} PRIVATE -Wnon-virtual-dtor)
        target_compile_options(${target} PRIVATE -Wctor-dtor-privacy)
        target_compile_options(${target} PRIVATE -Wsign-conversion)
        target_compile_options(${target} PRIVATE -Wdisabled-optimization)
        target_compile_options(${target} PRIVATE -Weffc++)
        # target_compile_options(${target} PRIVATE -Winline)
        target_compile_options(${target} PRIVATE -Winvalid-pch)
        target_compile_options(${target} PRIVATE -Wmissing-declarations)
        target_compile_options(${target} PRIVATE -Woverloaded-virtual)
        if(NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 6.0)
            target_compile_options(${target} PRIVATE -Wnoexcept)
        endif()

        # no way to silence it in the expression decomposition macros: _Pragma() in macros doesn't work for the c++ front-end of g++
        # https://gcc.gnu.org/bugzilla/show_bug.cgi?id=55578
        # https://gcc.gnu.org/bugzilla/show_bug.cgi?id=69543
        # Also the warning is completely worthless nowadays - http://stackoverflow.com/questions/14016993
        #target_compile_options(${target} PRIVATE -Waggregate-return)

        if(NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.0)
            target_compile_options(${target} PRIVATE -Wdouble-promotion)
            target_compile_options(${target} PRIVATE -Wtrampolines)
            #target_compile_options(${target} PRIVATE -Wzero-as-null-pointer-constant) # doesn't work with spaceship operator
            #target_compile_options(${target} PRIVATE -Wuseless-cast)
            target_compile_options(${target} PRIVATE -Wvector-operation-performance)
        endif()

        if(NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 6.0)
            target_compile_options(${target} PRIVATE -Wshift-overflow=2)
            target_compile_options(${target} PRIVATE -Wnull-dereference)
            target_compile_options(${target} PRIVATE -Wduplicated-cond)
        endif()

        if(NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.0)
            target_compile_options(${target} PRIVATE -Walloc-zero)
            target_compile_options(${target} PRIVATE -Walloca)
            target_compile_options(${target} PRIVATE -Wduplicated-branches)
        endif()

        if(NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.0)
            target_compile_options(${target} PRIVATE -Wcast-align=strict)
        endif()
    endif()

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(${target} PRIVATE -Weverything)
        target_compile_options(${target} PRIVATE -Wno-c++98-compat)
        target_compile_options(${target} PRIVATE -Wno-c++98-compat-pedantic)
        target_compile_options(${target} PRIVATE -Wno-c++98-compat-bind-to-temporary-copy)
        target_compile_options(${target} PRIVATE -Wno-c++98-compat-local-type-template-args)
        target_compile_options(${target} PRIVATE -Qunused-arguments -fcolor-diagnostics) # needed for ccache integration on travis
    endif()

    if(MSVC)
        target_compile_options(${target} PRIVATE /EHsc) # exception handling
        target_compile_options(${target} PRIVATE /std:c++latest) # for post c++14 updates in MSVC
        target_compile_options(${target} PRIVATE /permissive-)   # force standard conformance - this is the better flag than /Za
        target_compile_options(${target} PRIVATE /WX)
        target_compile_options(${target} PRIVATE /Wall) # turns on warnings from levels 1 through 4 which are off by default - https://msdn.microsoft.com/en-us/library/23k5d385.aspx
        
        add_definitions(/MP) # parallel builds

        target_compile_options(${target} PRIVATE 
            /wd4514 # unreferenced inline function has been removed
            /wd4571 # SEH related
            /wd4710 # function not inlined
            /wd4711 # function 'x' selected for automatic inline expansion
            
            /wd4616 # invalid compiler warnings - https://msdn.microsoft.com/en-us/library/t7ab6xtd.aspx
            /wd4619 # invalid compiler warnings - https://msdn.microsoft.com/en-us/library/tacee08d.aspx
            
            /wd4820 # padding in structs
            /wd4625 # copy constructor was implicitly defined as deleted
            /wd4626 # assignment operator was implicitly defined as deleted
            /wd5027 # move assignment operator was implicitly defined as deleted
            /wd5026 # move constructor was implicitly defined as deleted
            /wd4623 # default constructor was implicitly defined as deleted
            /wd4774 # 'swprintf_s' : format string expected in argument 3 is not a string literal

            /wd5045 # Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
            /wd5039 #: '_Thrd_start': pointer or reference to potentially throwing function passed to extern C function under -EHc. Undefined behavior
        )
    endif()

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU" )
        target_compile_options(${target} PRIVATE -ggdb -g)

        # enable lots of warning checking
        #target_compile_options(robinhood-test PRIVATE -Werror -Wall -Wextra -Weffc++ -Wconversion -Wunreachable-code -Wuninitialized -Wshadow -Wfloat-equal -Wmissing-braces)

        # make sure OpenMP in the tests work
        #target_compile_options(robinhood-test PRIVATE -fopenmp)
        #target_link_libraries(robinhood-test PRIVATE -fopenmp)

        #target_compile_options(robinhood-test PRIVATE -fsanitize=undefined,float-divide-by-zero,float-cast-overflow)
        #target_link_libraries(robinhood-test PRIVATE -fsanitize=undefined,float-divide-by-zero,float-cast-overflow)

        #target_compile_options(robinhood-test PRIVATE -fsanitize=address)
        #target_link_libraries(robinhood-test PRIVATE -fsanitize=address)

        # warns in doctest.h :(
        #target_compile_options(robinhood-test PRIVATE -fsanitize=memory)
        #target_link_libraries(robinhood-test PRIVATE -fsanitize=memory)
    endif()
endfunction()

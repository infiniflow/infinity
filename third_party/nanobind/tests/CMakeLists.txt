# If leaks are found, abort() during interpreter shutdown to catch this in the CI
add_definitions(-DNB_ABORT_ON_LEAK)

if (NB_TEST_STABLE_ABI)
  set(NB_EXTRA_ARGS ${NB_EXTRA_ARGS} STABLE_ABI)
endif()

if (NB_TEST_SHARED_BUILD)
  set(NB_EXTRA_ARGS ${NB_EXTRA_ARGS} NB_SHARED)
endif()

# Enable extra warning flags
if (MSVC)
  add_compile_options(/W4)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  add_compile_options(-Wall -Wextra -Wno-unused-local-typedefs)
endif()

if (UNIX AND (CMAKE_SIZEOF_VOID_P EQUAL 4) AND (CMAKE_SYSTEM_PROCESSOR STREQUAL i686))
  # Don't use the legacy 387 math coprocessor in 32 bit builds, this causes tests to fail
  add_compile_options(-mfpmath=sse -msse2)
endif()

if (NB_TEST_SANITIZE)
  add_compile_options(-fsanitize=address,undefined)
  add_link_options(-fsanitize=address,undefined)
endif()


nanobind_add_module(test_functions_ext test_functions.cpp ${NB_EXTRA_ARGS})
nanobind_add_module(test_classes_ext test_classes.cpp ${NB_EXTRA_ARGS})
nanobind_add_module(test_holders_ext test_holders.cpp ${NB_EXTRA_ARGS})
nanobind_add_module(test_stl_ext test_stl.cpp ${NB_EXTRA_ARGS})
nanobind_add_module(test_bind_map_ext test_stl_bind_map.cpp ${NB_EXTRA_ARGS})
nanobind_add_module(test_bind_vector_ext test_stl_bind_vector.cpp ${NB_EXTRA_ARGS})
nanobind_add_module(test_chrono_ext test_chrono.cpp ${NB_EXTRA_ARGS})
nanobind_add_module(test_enum_ext test_enum.cpp ${NB_EXTRA_ARGS})
nanobind_add_module(test_eval_ext test_eval.cpp ${NB_EXTRA_ARGS})
nanobind_add_module(test_ndarray_ext test_ndarray.cpp ${NB_EXTRA_ARGS})
nanobind_add_module(test_intrusive_ext test_intrusive.cpp test_intrusive_impl.cpp ${NB_EXTRA_ARGS})
nanobind_add_module(test_exception_ext test_exception.cpp ${NB_EXTRA_ARGS})
nanobind_add_module(test_make_iterator_ext test_make_iterator.cpp ${NB_EXTRA_ARGS})
nanobind_add_module(test_typing_ext test_typing.cpp ${NB_EXTRA_ARGS})
nanobind_add_module(test_issue_ext test_issue.cpp ${NB_EXTRA_ARGS})

foreach (NAME functions classes ndarray stl enum typing make_iterator)
  if (NAME STREQUAL typing)
    set(EXTRA
      MARKER_FILE py.typed
      PATTERN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/pattern_file.nb"
    )
  else()
    set(EXTRA "")
  endif()

  if (CMAKE_CONFIGURATION_TYPES)
    # On multi-config generators like Visual Studio, put stubs in the 'Debug' / 'Release' /.. folders
    set(PYI_PREFIX $<CONFIG>/)
  endif()

  nanobind_add_stub(
    ${NAME}_ext_stub
    MODULE test_${NAME}_ext
    OUTPUT ${PYI_PREFIX}test_${NAME}_ext.pyi
    PYTHON_PATH $<TARGET_FILE_DIR:test_${NAME}_ext>
    DEPENDS test_${NAME}_ext
    ${EXTRA})
endforeach()

nanobind_add_stub(
  py_stub
  MODULE py_stub_test
  OUTPUT ${PYI_PREFIX}py_stub_test.pyi
  PYTHON_PATH $<TARGET_FILE_DIR:test_stl_ext>
  DEPENDS py_stub_test.py
)

find_package (Eigen3 3.3.1 NO_MODULE)
if (TARGET Eigen3::Eigen)
  nanobind_add_module(test_eigen_ext test_eigen.cpp ${NB_EXTRA_ARGS})
  target_link_libraries(test_eigen_ext PRIVATE Eigen3::Eigen)
endif()

add_library(
  inter_module
  SHARED
  inter_module.h
  inter_module.cpp
)

target_compile_definitions(inter_module PRIVATE -DSHARED_BUILD)
target_compile_features(inter_module PRIVATE cxx_std_17)
target_include_directories(inter_module PRIVATE ${NB_DIR}/include)

nanobind_add_module(test_inter_module_1_ext NB_DOMAIN mydomain test_inter_module_1.cpp ${NB_EXTRA_ARGS})
nanobind_add_module(test_inter_module_2_ext NB_DOMAIN mydomain test_inter_module_2.cpp ${NB_EXTRA_ARGS})
target_link_libraries(test_inter_module_1_ext PRIVATE inter_module)
target_link_libraries(test_inter_module_2_ext PRIVATE inter_module)

set(TEST_FILES
  common.py
  test_classes.py
  test_eigen.py
  test_enum.py
  test_eval.py
  test_exception.py
  test_functions.py
  test_holders.py
  test_inter_module.py
  test_intrusive.py
  test_make_iterator.py
  test_stl.py
  test_stl_bind_map.py
  test_stl_bind_vector.py
  test_chrono.py
  test_ndarray.py
  test_stubs.py
  test_typing.py

  # Stub reference files
  test_classes_ext.pyi.ref
  test_functions_ext.pyi.ref
  test_make_iterator_ext.pyi.ref
  test_ndarray_ext.pyi.ref
  test_stl_ext.pyi.ref
  test_enum_ext.pyi.ref
  test_typing_ext.pyi.ref
  py_stub_test.py
  py_stub_test.pyi.ref
)

if (NOT (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR) OR MSVC)
  if (CMAKE_CONFIGURATION_TYPES)
    set(OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
  else()
    set(OUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
  endif()

  foreach(TEST_FILE IN LISTS TEST_FILES)
    set(IN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_FILE})
    set(OUT_FILE ${OUT_DIR}/${TEST_FILE})
    set(TEST_FILES_OUT ${TEST_FILES_OUT} ${OUT_FILE})
    add_custom_command(
      DEPENDS ${IN_FILE} OUTPUT ${OUT_FILE}
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${IN_FILE} ${OUT_DIR})
  endforeach()

  add_custom_target(copy-tests ALL DEPENDS ${TEST_FILES_OUT})
endif()

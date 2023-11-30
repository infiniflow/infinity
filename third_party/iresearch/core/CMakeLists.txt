# set version components
if (NOT IResearch_version_major)
  set(IResearch_version_major "1")
endif ()
if (NOT IResearch_version_minor)
  set(IResearch_version_minor "3")
endif ()
if (NOT IResearch_version_revision)
  set(IResearch_version_revision "0")
endif ()
if (NOT IResearch_version_patch)
  set(IResearch_version_patch "0")
endif ()

set(SIMD_LIBRARY_STATIC "simdcomp-static")

# set libatomic library 
# (not available on Windows and Mac)
if (NOT MSVC AND NOT APPLE)
  set(ATOMIC_LIBRARY atomic)
endif ()

# calculate version
math(EXPR IResearch_int_version "(${IResearch_version_major} * 1000000) + (${IResearch_version_minor} * 10000) + (${IResearch_version_revision} * 100) + (${IResearch_version_patch} * 1)")
set(IResearch_version "${IResearch_version_major}.${IResearch_version_minor}.${IResearch_version_revision}.${IResearch_version_patch}")

# generate version_defines.hpp
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/utils/version_defines.template.hpp"
  "${CMAKE_CURRENT_BINARY_DIR}/utils/version_defines.hpp"
)

set(IResearch_INCLUDE_DIR
  "${PROJECT_SOURCE_DIR}/core"
  CACHE INTERNAL
  ""
  )

if (MSVC)
  add_definitions(/W4)
  # disable "checked iterators" feature
  add_definitions(-D_SCL_SECURE_NO_WARNINGS)

  # set OS specific sources
  set(IResearch_core_os_specific_sources
    ./utils/mman_win32.cpp
    )

  # set OS specific headers
  set(IResearch_core_os_specific_headers
    ./utils/mman_win32.hpp
    )
else ()
  add_definitions(-Wall)

  if (NOT APPLE AND NOT (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" OR CMAKE_BUILD_TYPE STREQUAL "Debug"))
    add_link_options(-s)
  endif ()

  # set OS specific headers
  set(IResearch_core_os_specific_headers
    ./utils/mman_posix.hpp
    )
endif ()

source_group("analysis" ./analysis/*)
source_group("document" ./document/*)
source_group("error" ./error/*)
source_group("formats" ./formats/*)
source_group("index" ./index/*)
source_group("search" ./search/*)
source_group("store" ./store/*)
source_group("utils" ./utils/*)
source_group("service" ./service/*)

set(IResearch_core_sources
  ./utils/assert.cpp
  ./utils/fstext/fst-error.cpp
  ./utils/rst/strings/str_cat.cc
  ./analysis/analyzers.cpp
  ./analysis/token_attributes.cpp
  ./analysis/token_streams.cpp
  ./analysis/pipeline_token_stream.cpp
  ./analysis/delimited_token_stream.cpp 
  ./analysis/ngram_token_stream.cpp 
  ./analysis/token_stopwords_stream.cpp  
  ./analysis/segmentation_token_stream.cpp
  ./analysis/jieba_analyzer.cpp
  ./error/error.cpp
  ./formats/columnstore.cpp
  ./formats/columnstore2.cpp
  ./formats/formats.cpp
  ./formats/format_utils.cpp
  ./formats/skip_list.cpp
  ./formats/sparse_bitmap.cpp
  ./formats/formats_10.cpp
  ./formats/formats_burst_trie.cpp
  ./index/buffered_column.cpp
  ./index/directory_reader.cpp
  ./index/directory_reader_impl.cpp
  ./index/field_data.cpp
  ./index/field_meta.cpp
  ./index/file_names.cpp
  ./index/index_writer.cpp
  ./index/index_reader.cpp
  ./index/iterators.cpp
  ./index/merge_writer.cpp
  ./index/norm.cpp
  ./index/postings.cpp
  ./index/segment_reader.cpp
  ./index/segment_reader_impl.cpp
  ./index/segment_writer.cpp
  ./search/all_docs_provider.cpp
  ./search/all_filter.cpp
  ./search/all_iterator.cpp
  ./search/boost_scorer.cpp
  ./search/granular_range_filter.cpp
  ./search/scorers.cpp
  ./search/scorer.cpp
  ./search/score_function.cpp
  ./search/cost.cpp
  ./search/collectors.cpp
  ./search/score.cpp
  ./search/bitset_doc_iterator.cpp
  ./search/filter.cpp
  ./search/term_filter.cpp
  ./search/nested_filter.cpp
  ./search/terms_filter.cpp
  ./search/prefix_filter.cpp
  ./search/range_filter.cpp
  ./search/phrase_filter.cpp
  ./search/phrase_query.cpp
  ./search/column_existence_filter.cpp
  ./search/same_position_filter.cpp
  ./search/wildcard_filter.cpp
  ./search/levenshtein_filter.cpp
  ./search/multiterm_query.cpp
  ./search/term_query.cpp
  ./search/boolean_filter.cpp
  ./search/ngram_similarity_filter.cpp
  ./search/ngram_similarity_query.cpp
  ./search/proxy_filter.cpp
  ./search/tfidf.cpp
  ./search/bm25.cpp
  ./store/data_input.cpp
  ./store/data_output.cpp
  ./store/directory.cpp
  ./store/directory_attributes.cpp
  ./store/directory_cleaner.cpp
  ./store/fs_directory.cpp
  ./store/mmap_directory.cpp
  ./store/memory_directory.cpp
  ./store/store_utils.cpp
  ./utils/async_utils.cpp
  ./utils/thread_utils.cpp
  ./utils/attributes.cpp
  ./utils/automaton_utils.cpp
  ./utils/bit_packing.cpp
  ./utils/encryption.cpp
  ./utils/ctr_encryption.cpp
  ./utils/compression.cpp
  ./utils/delta_compression.cpp
  ./utils/lz4compression.cpp
  ./utils/directory_utils.cpp
  ./utils/file_utils.cpp
  ./utils/mmap_utils.cpp
  ./utils/index_utils.cpp
  ./utils/levenshtein_utils.cpp
  ./utils/wildcard_utils.cpp
  ./utils/levenshtein_default_pdp.cpp
  ./utils/memory.cpp
  ./utils/timer_utils.cpp
  ./utils/log.cpp
  ./utils/so_utils.cpp
  ./utils/process_utils.cpp
  ./utils/network_utils.cpp
  ./utils/numeric_utils.cpp
  ./utils/options.cpp
  ${IResearch_core_os_specific_sources}
  ${IResearch_core_optimized_sources}
  )

set(IResearch_core_headers
  ./utils/assert.hpp
  ./analysis/analyzer.hpp
  ./analysis/analyzer.hpp
  ./analysis/token_attributes.hpp
  ./analysis/token_stream.hpp
  ./analysis/token_streams.hpp
  ./error/error.hpp
  ./formats/formats.hpp
  ./formats/format_utils.hpp
  ./formats/skip_list.hpp
  ./formats/formats_10.hpp
  ./formats/formats_10_attributes.hpp
  ./formats/formats_burst_trie.hpp  
  ./index/directory_reader.hpp
  ./index/directory_reader_impl.hpp
  ./index/field_data.hpp
  ./index/field_meta.hpp
  ./index/file_names.hpp
  ./index/index_meta.hpp
  ./index/index_reader.hpp
  ./index/index_reader_options.hpp
  ./index/iterators.hpp
  ./index/segment_reader.hpp
  ./index/segment_reader_impl.hpp
  ./index/segment_writer.hpp
  ./index/index_writer.hpp
  ./search/all_filter.hpp
  ./search/all_iterator.hpp
  ./search/boost_scorer.hpp
  ./search/granular_range_filter.hpp
  ./search/scorers.hpp
  ./search/states_cache.hpp
  ./search/scorer.hpp
  ./search/cost.hpp
  ./search/filter.hpp
  ./search/term_filter.hpp
  ./search/nested_filter.hpp
  ./search/phrase_filter.hpp
  ./search/same_position_filter.hpp
  ./search/prefix_filter.hpp
  ./search/range_filter.hpp
  ./search/column_existence_filter.hpp
  ./search/multiterm_query.hpp
  ./search/term_query.hpp
  ./search/boolean_filter.hpp
  ./search/disjunction.hpp
  ./search/conjunction.hpp
  ./search/exclusion.hpp
  ./search/ngram_similarity_filter.hpp
  ./search/filter_visitor.hpp
  ./search/proxy_filter.hpp
  ./search/bm25.hpp
  ./search/tfidf.hpp
  ./store/data_input.hpp
  ./store/data_output.hpp
  ./store/directory.hpp
  ./store/fs_directory.hpp
  ./store/memory_directory.hpp
  ./store/store_utils.hpp
  ./utils/attributes.hpp
  ./utils/automaton.hpp
  ./utils/automaton_utils.hpp
  ./utils/wildcard_utils.hpp
  ./utils/bit_packing.hpp
  ./utils/bit_utils.hpp
  ./utils/block_pool.hpp
  ./utils/compression.hpp
  ./utils/directory_utils.hpp
  ./utils/lz4compression.hpp
  ./utils/file_utils.hpp
  ./utils/fstext/fst_builder.hpp
  ./utils/fstext/fst_decl.hpp
  ./utils/fstext/fst_table_matcher.hpp
  ./utils/fstext/fst_string_weight.hpp
  ./utils/mmap_utils.hpp
  ./utils/io_utils.hpp
  ./utils/iterator.hpp
  ./utils/math_utils.hpp
  ./utils/memory.hpp
  ./utils/misc.hpp
  ./utils/noncopyable.hpp
  ./utils/singleton.hpp
  ./utils/register.hpp
  ./utils/std.hpp
  ./utils/string.hpp
  ./utils/log.hpp
  ./utils/thread_utils.hpp
  ./utils/object_pool.hpp
  ./utils/so_utils.hpp
  ./utils/process_utils.hpp
  ./utils/network_utils.hpp
  ./utils/numeric_utils.hpp
  ./utils/version_utils.hpp
  ./utils/bitset.hpp
  ./utils/type_id.hpp
  ./utils/options.hpp
  ./shared.hpp
  ./types.hpp
  )

if (uring_FOUND)
  list(APPEND IResearch_core_sources ./store/async_directory.cpp)
endif ()

if (MSVC)
  set(DL_LIBRARY "Dbghelp.lib")  # TODO Try to remove it
else ()
  set(DL_LIBRARY dl)
endif ()

include_directories(
  ${IResearch_INCLUDE_DIR}
)


set(IRESEARCH_EXTERNAL_INCLUDES
  ${EXTERNAL_INCLUDE_DIRS}
  ${Lz4_INCLUDE_DIR}
  ${HIGHWAY_INCLUDE_DIR}
  ${OPENFST_INCLUDE_DIR}
  ${FROZEN_INCLUDE_DIR}
  ${JIEBA_INCLUDE_DIR}
  ${JIEBA_DEPS_INCLUDE_DIR}
  ${GPHM_INCLUDE_DIR})

if (SUPPRESS_EXTERNAL_WARNINGS)
  include_directories(SYSTEM ${IRESEARCH_EXTERNAL_INCLUDES})
else ()
  include_directories(${IRESEARCH_EXTERNAL_INCLUDES})
endif ()
 
file(GLOB_RECURSE
  service_sources
  CONFIGURE_DEPENDS
  service/*.cpp
)

add_library(iresearch
  STATIC
  ${IResearch_core_sources} 
  ${IResearch_core_headers}
  $<TARGET_OBJECTS:openfst>
  )

# trigger generation of BUILD_IDENTIFIER if needed
add_custom_command(
  OUTPUT ${CMAKE_BINARY_DIR}/BUILD_IDENTIFIER
  COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/BUILD_IDENTIFIER
)

add_dependencies(iresearch
  hwy
  )

target_include_directories(iresearch
  SYSTEM
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
  PRIVATE $<TARGET_PROPERTY:text,INTERFACE_INCLUDE_DIRECTORIES>
  PUBLIC ${VPACK_INCLUDE_PATH}
  PUBLIC $<TARGET_PROPERTY:hwy,INTERFACE_INCLUDE_DIRECTORIES>
  )

set_target_properties(iresearch
  PROPERTIES
  PREFIX lib
  IMPORT_PREFIX lib
  OUTPUT_NAME iresearch
  )

target_link_libraries(iresearch
  ${SIMD_LIBRARY_STATIC}
  text
  ${LIBURING}
  )

if (CLANG_TIDY_EXE)
  set_target_properties(
    iresearch-static
    PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_EXE};-header-filter=.;-checks=clang-analyzer-cplusplus*,google-*,performance-*,readability-*"
  )
endif ()


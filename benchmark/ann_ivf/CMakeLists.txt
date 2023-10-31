
add_executable(ann_ivf_flat_l2_benchmark "ann_ivf_flat_l2_benchmark.cpp")
target_include_directories(ann_ivf_flat_l2_benchmark PUBLIC "${CMAKE_SOURCE_DIR}/third_party/faiss")
target_include_directories(ann_ivf_flat_l2_benchmark PUBLIC "${CMAKE_SOURCE_DIR}")
target_link_libraries(faiss)
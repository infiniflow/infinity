# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

set(MLAS_ROOT ${CMAKE_SOURCE_DIR}/third_party/mlas)
set(MLAS_SRC_DIR ${MLAS_ROOT}/lib)
set(MLAS_INC_DIR ${MLAS_ROOT}/inc)

function(onnxruntime_add_include_to_target dst_target)
    foreach(src_target ${ARGN})
        if(TARGET ${src_target})
            target_include_directories(${dst_target} PRIVATE $<TARGET_PROPERTY:${src_target},INTERFACE_INCLUDE_DIRECTORIES>)
            target_compile_definitions(${dst_target} PRIVATE $<TARGET_PROPERTY:${src_target},INTERFACE_COMPILE_DEFINITIONS>)
            target_sources(${dst_target} PRIVATE $<TARGET_PROPERTY:${src_target},INTERFACE_SOURCES>)
        endif()
    endforeach()
endfunction()

function(onnxruntime_set_compile_flags target_name)
    if (CPUINFO_SUPPORTED)
        onnxruntime_add_include_to_target(${target_name} cpuinfo::cpuinfo)
    endif()
    if(onnxruntime_ENABLE_LAZY_TENSOR)
        target_compile_definitions(${target_name} PRIVATE ENABLE_LAZY_TENSOR)
    endif()
    if (onnxruntime_ENABLE_CPU_FP16_OPS)
        target_compile_definitions(${target_name} PRIVATE ENABLE_CPU_FP16_TRAINING_OPS)
    endif()
    if(onnxruntime_DISABLE_ABSEIL)
        target_compile_definitions(${target_name} PRIVATE DISABLE_ABSEIL)
    endif()
    if(UNIX)
        target_compile_definitions(${target_name} PRIVATE PLATFORM_POSIX)
    endif()
    target_compile_definitions(${target_name} PRIVATE EIGEN_USE_THREADS)
    if (onnxruntime_DISABLE_CONTRIB_OPS)
        target_compile_definitions(${target_name} PRIVATE DISABLE_CONTRIB_OPS)
    endif()

    if (onnxruntime_DISABLE_ML_OPS)
        target_compile_definitions(${target_name} PRIVATE DISABLE_ML_OPS)
    endif()

    if (onnxruntime_DISABLE_SPARSE_TENSORS)
        target_compile_definitions(${target_name} PRIVATE DISABLE_SPARSE_TENSORS)
    endif()

    if (onnxruntime_DISABLE_OPTIONAL_TYPE)
        target_compile_definitions(${target_name} PRIVATE DISABLE_OPTIONAL_TYPE)
    endif()

    if (onnxruntime_DISABLE_FLOAT8_TYPES)
        target_compile_definitions(${target_name} PRIVATE DISABLE_FLOAT8_TYPES)
    endif()

    if (onnxruntime_ENABLE_ATEN)
        target_compile_definitions(${target_name} PRIVATE ENABLE_ATEN)
    endif()

    if(USE_NEURAL_SPEED)
        target_compile_definitions(${target_name} PRIVATE ORT_NEURAL_SPEED)
    endif()

    set_target_properties(${target_name} PROPERTIES COMPILE_WARNING_AS_ERROR ON)
    if (onnxruntime_USE_CUDA)
        # Suppress a "conversion_function_not_usable" warning in gsl/span
        target_compile_options(${target_name} PRIVATE "$<$<COMPILE_LANGUAGE:CUDA>:SHELL:-Xcudafe \"--diag_suppress=conversion_function_not_usable\">")
        target_compile_definitions(${target_name} PRIVATE -DDISABLE_CUSPARSE_DEPRECATED)
    endif()
    if (MSVC)
        foreach(CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORY ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES})
            target_compile_options(${target_name} PRIVATE "$<$<COMPILE_LANGUAGE:CXX,C>:/external:I${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORY}>")
        endforeach()

        foreach(onnxruntime_external_lib IN LISTS onnxruntime_EXTERNAL_LIBRARIES)
            #TODO: the list contains cmake keywords like "debug". We should exclude them.
            if(TARGET ${onnxruntime_external_lib})
                get_target_property(onnxruntime_external_lib_include_dirs ${onnxruntime_external_lib} INTERFACE_INCLUDE_DIRECTORIES)
                foreach(onnxruntime_external_lib_include_dir IN LISTS onnxruntime_external_lib_include_dirs)
                    if(onnxruntime_external_lib_include_dir MATCHES "^\\$")
                        if(onnxruntime_external_lib_include_dir MATCHES "^\\$<BUILD_INTERFACE:([^>]+)>$")
                            string(REGEX REPLACE "^\\$<BUILD_INTERFACE:([^>]+)>$" "\\1" onnxruntime_external_lib_include_dir_cmake "${onnxruntime_external_lib_include_dir}")
                            cmake_path(NATIVE_PATH onnxruntime_external_lib_include_dir_cmake NORMALIZE onnxruntime_external_lib_include_dir_native)
                            target_compile_options(${target_name} PRIVATE "$<$<COMPILE_LANGUAGE:CXX,C>:/external:I${onnxruntime_external_lib_include_dir_native}>")
                        endif()
                    else()
                        cmake_path(NATIVE_PATH onnxruntime_external_lib_include_dir NORMALIZE onnxruntime_external_lib_include_dir_native)
                        target_compile_options(${target_name} PRIVATE "$<$<COMPILE_LANGUAGE:CXX,C>:/external:I${onnxruntime_external_lib_include_dir_native}>")
                    endif()
                endforeach()
            endif()
        endforeach()
        target_compile_definitions(${target_name} PRIVATE -DPLATFORM_WINDOWS -DNOGDI -DNOMINMAX -D_USE_MATH_DEFINES -D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
        if (onnxruntime_ENABLE_MEMLEAK_CHECKER)
            target_compile_definitions(${target_name} PRIVATE -DONNXRUNTIME_ENABLE_MEMLEAK_CHECK)
        endif()
        target_compile_options(${target_name} PRIVATE "$<$<COMPILE_LANGUAGE:CUDA>:SHELL:--compiler-options /utf-8>" "$<$<COMPILE_LANGUAGE:CXX,C>:/utf-8>")
        target_compile_options(${target_name} PRIVATE "$<$<COMPILE_LANGUAGE:CUDA>:SHELL:--compiler-options /sdl>" "$<$<COMPILE_LANGUAGE:CXX,C>:/sdl>")
        set_target_properties(${target_name}
                PROPERTIES VS_GLOBAL_CAExcludePath "${ORT_BINARY_DIR};${ORT_SOURCE_DIR}")
        # We do not treat warnings from 3rd-party libraries as errors. In order to do that, we need to add their header files locations to /external:I.
        target_compile_options(${target_name} PRIVATE "$<$<COMPILE_LANGUAGE:CXX,C>:/experimental:external>" "$<$<COMPILE_LANGUAGE:CUDA>:SHELL:--compiler-options /experimental:external>")
        target_compile_options(${target_name} PRIVATE "$<$<COMPILE_LANGUAGE:CXX,C>:/external:W0>" "$<$<COMPILE_LANGUAGE:CUDA>:SHELL:--compiler-options /external:W0>")
        target_compile_options(${target_name} PRIVATE "$<$<COMPILE_LANGUAGE:CXX,C>:/external:templates->" "$<$<COMPILE_LANGUAGE:CUDA>:SHELL:--compiler-options /external:templates->")
        target_compile_options(${target_name} PRIVATE "$<$<COMPILE_LANGUAGE:CXX,C>:/external:I${CMAKE_CURRENT_SOURCE_DIR}>" "$<$<COMPILE_LANGUAGE:CUDA>:SHELL:--compiler-options /external:I${CMAKE_CURRENT_SOURCE_DIR}>")
        target_compile_options(${target_name} PRIVATE "$<$<COMPILE_LANGUAGE:CXX,C>:/external:I${CMAKE_CURRENT_BINARY_DIR}>" "$<$<COMPILE_LANGUAGE:CUDA>:SHELL:--compiler-options /external:I${CMAKE_CURRENT_BINARY_DIR}>")
        if (onnxruntime_ENABLE_STATIC_ANALYSIS)
            target_compile_options(${target_name} PRIVATE "$<$<COMPILE_LANGUAGE:CUDA>:SHELL:--compiler-options /analyze>" "$<$<COMPILE_LANGUAGE:CXX,C>:/analyze>")
            if (onnxruntime_REDIRECT_STATIC_ANALYSIS_OUTPUTS_TO_FILE)
                target_compile_options(${target_name} PRIVATE "$<$<COMPILE_LANGUAGE:CUDA>:SHELL:--compiler-options /analyze:autolog:ext.sarif>" "$<$<COMPILE_LANGUAGE:CXX,C>:/analyze:autolog:ext.sarif>")
            endif()
            target_compile_options(${target_name} PRIVATE "$<$<COMPILE_LANGUAGE:CUDA>:SHELL:--compiler-options /analyze:external->" "$<$<COMPILE_LANGUAGE:CXX,C>:/analyze:external->")
            target_compile_options(${target_name} PRIVATE "$<$<COMPILE_LANGUAGE:CUDA>:SHELL:--compiler-options /wd6385>" )
            # There are many such warnings from STL:
            # include\list(148): warning C6011: Dereferencing NULL pointer '_Mycont'. : Lines: 146, 147, 148
            target_compile_options(${target_name} PRIVATE "$<$<COMPILE_LANGUAGE:CUDA>:SHELL:--compiler-options /wd6011>" )
        endif()
    else()
        # Enable warning
        target_compile_options(${target_name} PRIVATE "$<$<COMPILE_LANGUAGE:CUDA>:SHELL:--compiler-options -Wall>" "$<$<NOT:$<COMPILE_LANGUAGE:CUDA>>:-Wall>")
        target_compile_options(${target_name} PRIVATE "$<$<NOT:$<COMPILE_LANGUAGE:CUDA>>:-Wextra>")
        if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
            #external/protobuf/src/google/protobuf/arena.h:445:18: error: unused parameter 'p'
            target_compile_options(${target_name} PRIVATE "-Wno-unused-parameter")
        endif()
        target_compile_definitions(${target_name} PUBLIC -DNSYNC_ATOMIC_CPP11)
        onnxruntime_add_include_to_target(${target_name} nsync::nsync_cpp)
    endif()
    foreach(ORT_FLAG ${ORT_PROVIDER_FLAGS})
        target_compile_definitions(${target_name} PRIVATE ${ORT_FLAG})
    endforeach()
    if (HAS_DEPRECATED_COPY)
        #too many such errors in eigen
        target_compile_options(${target_name} PRIVATE "$<$<COMPILE_LANGUAGE:CUDA>:SHELL:--compiler-options -Wno-deprecated-copy>" "$<$<COMPILE_LANGUAGE:CXX>:-Wno-deprecated-copy>")
    endif()
    foreach(FLAG ${ORT_WARNING_FLAGS})
        target_compile_options(${target_name} PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${FLAG}>")
    endforeach()
    if (onnxruntime_USE_CUDA)
        foreach(FLAG ${ORT_WARNING_FLAGS})
            target_compile_options(${target_name} PRIVATE "$<$<COMPILE_LANGUAGE:CUDA>:SHELL:--compiler-options ${FLAG}>")
        endforeach()
        if (NVCC_HAS_STRICT_ALIASING AND "${target_name}" MATCHES "cuda")
            target_compile_options(${target_name} PRIVATE "$<$<COMPILE_LANGUAGE:CUDA>:-Wno-strict-aliasing>")
        endif()
        if (HAS_STRICT_ALIASING AND NOT "${target_name}" MATCHES "cuda")
            target_compile_options(${target_name} PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:-Wno-strict-aliasing>")
        endif()
    endif()
    if (onnxruntime_USE_ROCM)
        # flags are detected with CXX language mode, some flags are not supported with hipclang
        # because we may mix gcc and hipclang
        set(ORT_HIP_WARNING_FLAGS ${ORT_WARNING_FLAGS})
        list(REMOVE_ITEM ORT_HIP_WARNING_FLAGS -Wno-nonnull-compare)

        # float16.h:90:12: error: ‘tmp’ is used uninitialized
        list(APPEND ORT_HIP_WARNING_FLAGS -Wno-uninitialized)
        list(APPEND ORT_HIP_WARNING_FLAGS -Wno-deprecated-copy)

        # some #pragma unroll will fail, do not treat them as error
        # #warning must not be treated as error
        list(APPEND ORT_HIP_WARNING_FLAGS -Wno-error=pass-failed "-Wno-error=#warnings")

        # otherwise error: builtin __has_trivial_assign is deprecated; use __is_trivially_assignable instead
        if (ROCM_VERSION_DEV VERSION_GREATER_EQUAL "5.4")
            list(APPEND ORT_HIP_WARNING_FLAGS "-Wno-deprecated-builtins")
        endif()

        foreach(FLAG ${ORT_HIP_WARNING_FLAGS})
            target_compile_options(${target_name} PRIVATE "$<$<COMPILE_LANGUAGE:HIP>:SHELL:${FLAG}>")
        endforeach()
    endif()
endfunction()

function(onnxruntime_set_source_file_properties target_name)
    get_target_property(srcs ${target_name} SOURCES)

    # enable ARC for Objective-C/C++
    set(objective_c_cc_srcs ${srcs})
    list(FILTER objective_c_cc_srcs INCLUDE REGEX "\\.mm?$")
    set_property(SOURCE ${objective_c_cc_srcs} APPEND PROPERTY COMPILE_OPTIONS "-fobjc-arc")
endfunction()

function(onnxruntime_configure_target target_name)
    target_link_directories(${target_name} PRIVATE ${onnxruntime_LINK_DIRS})
    onnxruntime_set_compile_flags(${target_name})
    onnxruntime_set_source_file_properties(${target_name})
    if(WIN32 AND onnxruntime_ENABLE_STATIC_ANALYSIS AND onnxruntime_USE_CUSTOM_STATIC_ANALYSIS_RULES)
        set_target_properties(${target_name} PROPERTIES VS_USER_PROPS ${PROJECT_SOURCE_DIR}/EnableVisualStudioCodeAnalysis.props)
    endif()
    target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${ONNXRUNTIME_ROOT} ${abseil_cpp_SOURCE_DIR})
    if (onnxruntime_ENABLE_TRAINING_OPS)
        target_include_directories(${target_name} PRIVATE ${ORTTRAINING_ROOT})
    endif()
    if (onnxruntime_ENABLE_LTO)
        set_target_properties(${target_name} PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
        set_target_properties(${target_name} PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO TRUE)
        set_target_properties(${target_name} PROPERTIES INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL TRUE)
    endif()

    if (onnxruntime_BUILD_KERNEL_EXPLORER)
        get_target_property(target_type ${target_name} TYPE)
        if (target_type STREQUAL "MODULE_LIBRARY" OR target_type STREQUAL "SHARED_LIBRARY")
            set_property(TARGET ${target_name}
                    APPEND_STRING PROPERTY LINK_FLAGS " -Xlinker --version-script=${ONNXRUNTIME_ROOT}/python/tools/kernel_explorer/version_script.lds ")
        endif()
    endif()

    # Keep BinSkim happy
    if(MSVC AND NOT onnxruntime_target_platform MATCHES "ARM")
        target_link_options(${target_name} PRIVATE "/CETCOMPAT")
    endif()

endfunction()

function(onnxruntime_add_executable target_name)
    add_executable(${target_name} ${ARGN})
    onnxruntime_configure_target(${target_name})
    if (MSVC AND onnxruntime_target_platform STREQUAL "x86")
        target_link_options(${target_name} PRIVATE /SAFESEH)
    endif()
endfunction()


function(onnxruntime_add_static_library target_name)
    add_library(${target_name} STATIC ${ARGN})
    onnxruntime_configure_target(${target_name})
endfunction()

#
# All hardware agnostic source files here
# hardware specific files would cause trouble in
# multi-target build
#

onnxruntime_add_static_library(onnxruntime_mlas
  ${MLAS_SRC_DIR}/mlasi.h
  ${MLAS_SRC_DIR}/platform.cpp
  ${MLAS_SRC_DIR}/threading.cpp
  ${MLAS_SRC_DIR}/sgemm.cpp
  ${MLAS_SRC_DIR}/halfgemm.cpp
  ${MLAS_SRC_DIR}/qgemm.cpp
  ${MLAS_SRC_DIR}/qdwconv.cpp
  ${MLAS_SRC_DIR}/convolve.cpp
  ${MLAS_SRC_DIR}/convsym.cpp
  ${MLAS_SRC_DIR}/pooling.cpp
  ${MLAS_SRC_DIR}/transpose.cpp
  ${MLAS_SRC_DIR}/reorder.cpp
  ${MLAS_SRC_DIR}/snchwc.cpp
  ${MLAS_SRC_DIR}/activate.cpp
  ${MLAS_SRC_DIR}/logistic.cpp
  ${MLAS_SRC_DIR}/tanh.cpp
  ${MLAS_SRC_DIR}/erf.cpp
  ${MLAS_SRC_DIR}/compute.cpp
  ${MLAS_SRC_DIR}/quantize.cpp
  ${MLAS_SRC_DIR}/qgemm_kernel_default.cpp
  ${MLAS_SRC_DIR}/qladd.cpp
  ${MLAS_SRC_DIR}/qlmul.cpp
  ${MLAS_SRC_DIR}/qpostprocessor.cpp
  ${MLAS_SRC_DIR}/qlgavgpool.cpp
  ${MLAS_SRC_DIR}/qdwconv_kernelsize.cpp
  ${MLAS_SRC_DIR}/sqnbitgemm.h
  ${MLAS_SRC_DIR}/sqnbitgemm.cpp
  ${MLAS_SRC_DIR}/sqnbitgemm_q8_block.h
)

target_sources(onnxruntime_mlas PRIVATE
  ${MLAS_INC_DIR}/mlas_float16.h
  ${MLAS_INC_DIR}/mlas_gemm_postprocessor.h
  ${MLAS_INC_DIR}/mlas_q4.h
  ${MLAS_INC_DIR}/mlas_qnbit.h
  ${MLAS_INC_DIR}/mlas.h
)

if (NOT onnxruntime_ORT_MINIMAL_BUILD)
  target_sources(onnxruntime_mlas PRIVATE
    ${MLAS_SRC_DIR}/q4_dq.cpp
    ${MLAS_SRC_DIR}/q4gemm.cpp
  )
endif()

set(ONNXRUNTIME_MLAS_LIBS onnxruntime_mlas)

#TODO: set MASM flags properly
function(setup_mlas_source_for_windows)

  #
  # Sources common for all platforms.
  #
  target_sources(onnxruntime_mlas PRIVATE
    ${MLAS_SRC_DIR}/activate_fp16.cpp
    ${MLAS_SRC_DIR}/dwconv.cpp
    ${MLAS_SRC_DIR}/pooling_fp16.cpp
  )

  #The onnxruntime_target_platform variable was added by Windows AI team in onnxruntime_common.cmake
  #Don't use it for other platforms.
  if((onnxruntime_target_platform STREQUAL "ARM64") OR (onnxruntime_target_platform STREQUAL "ARM64EC"))
    set(PREPROCESS_ARMASM_FLAGS "")
    set(ARMASM_FLAGS "")

    if(onnxruntime_target_platform STREQUAL "ARM64")
      target_sources(onnxruntime_mlas PRIVATE
        ${MLAS_SRC_DIR}/halfgemm_kernel_neon.cpp
        ${MLAS_SRC_DIR}/qgemm_kernel_neon.cpp
        ${MLAS_SRC_DIR}/qgemm_kernel_udot.cpp
        ${MLAS_SRC_DIR}/qgemm_kernel_sdot.cpp
        ${MLAS_SRC_DIR}/sqnbitgemm_kernel_neon.cpp
      )

      set(mlas_platform_preprocess_srcs
        ${MLAS_SRC_DIR}/arm64/ConvSymS8KernelDot.asm
        ${MLAS_SRC_DIR}/arm64/ConvSymS8KernelDotLd64.asm
        ${MLAS_SRC_DIR}/arm64/ConvSymU8KernelDot.asm
        ${MLAS_SRC_DIR}/arm64/ConvSymS8KernelNeon.asm
        ${MLAS_SRC_DIR}/arm64/ConvSymU8KernelNeon.asm
        ${MLAS_SRC_DIR}/arm64/DepthwiseQConvSymS8KernelNeon.asm
        ${MLAS_SRC_DIR}/arm64/DepthwiseQConvSymU8KernelNeon.asm
        ${MLAS_SRC_DIR}/arm64/DepthwiseQConvKernelSize9Neon.asm
        ${MLAS_SRC_DIR}/arm64/HalfGemmKernelNeon.asm
        ${MLAS_SRC_DIR}/arm64/QgemmU8X8KernelNeon.asm
        ${MLAS_SRC_DIR}/arm64/QgemmS8S8KernelNeon.asm
        ${MLAS_SRC_DIR}/arm64/QgemmU8X8KernelUdot.asm
        ${MLAS_SRC_DIR}/arm64/QgemmS8S8KernelSdot.asm
        ${MLAS_SRC_DIR}/arm64/SgemmKernelNeon.asm
        ${MLAS_SRC_DIR}/arm64/SgemvKernelNeon.asm
        ${MLAS_SRC_DIR}/arm64/SymQgemmS8KernelNeon.asm
        ${MLAS_SRC_DIR}/arm64/SymQgemmS8KernelSDot.asm
        ${MLAS_SRC_DIR}/arm64/SymQgemmS8KernelSDotLd64.asm
      )
    else()
      target_sources(onnxruntime_mlas PRIVATE
        ${MLAS_SRC_DIR}/qgemm_kernel_neon.cpp
      )

      set(mlas_platform_preprocess_srcs
        ${MLAS_SRC_DIR}/arm64ec/QgemmU8X8KernelNeon.asm
        ${MLAS_SRC_DIR}/arm64ec/SgemmKernelNeon.asm
      )

      string(APPEND PREPROCESS_ARMASM_FLAGS " /arm64EC")
      string(APPEND ARMASM_FLAGS " -machine ARM64EC")
    endif()

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
      string(APPEND ARMASM_FLAGS " -g")
    endif()

    # Remove double quotes from flag strings.
    separate_arguments(PREPROCESS_ARMASM_FLAGS NATIVE_COMMAND "${PREPROCESS_ARMASM_FLAGS}")
    separate_arguments(ARMASM_FLAGS NATIVE_COMMAND "${ARMASM_FLAGS}")

    # Run the C precompiler on each input before the assembler.
    foreach(asm_filename ${mlas_platform_preprocess_srcs})
      get_filename_component(asm_filename_base ${asm_filename} NAME_WLE)
      set(preprocess_filename ${CMAKE_CURRENT_BINARY_DIR}/${asm_filename_base}.i)
      set(obj_filename ${CMAKE_CURRENT_BINARY_DIR}/${asm_filename_base}.obj)
      add_custom_command(
        OUTPUT ${obj_filename}
          COMMAND
              cl.exe ${PREPROCESS_ARMASM_FLAGS} /P ${asm_filename} /Fi${preprocess_filename}
          COMMAND
              armasm64.exe ${ARMASM_FLAGS} ${preprocess_filename} ${obj_filename}
        DEPENDS ${asm_filename}
        BYPRODUCTS ${preprocess_filename}
      )
      target_sources(onnxruntime_mlas PRIVATE ${obj_filename})
    endforeach()
  elseif(onnxruntime_target_platform STREQUAL "ARM")
    target_sources(onnxruntime_mlas PRIVATE
      ${MLAS_SRC_DIR}/arm/sgemmc.cpp
    )
  elseif(onnxruntime_target_platform STREQUAL "x64")

    file(GLOB_RECURSE mlas_platform_srcs_avx CONFIGURE_DEPENDS
      "${MLAS_SRC_DIR}/intrinsics/avx/*.cpp"
    )
    set_source_files_properties(${mlas_platform_srcs_avx} PROPERTIES COMPILE_FLAGS "/arch:AVX")

    file(GLOB_RECURSE mlas_platform_srcs_avx2 CONFIGURE_DEPENDS
      "${MLAS_SRC_DIR}/intrinsics/avx2/*.cpp"
    )
    set_source_files_properties(${mlas_platform_srcs_avx2} PROPERTIES COMPILE_FLAGS "/arch:AVX2")

    target_sources(onnxruntime_mlas PRIVATE
      ${MLAS_SRC_DIR}/dgemm.cpp
      ${mlas_platform_srcs_avx}
      ${mlas_platform_srcs_avx2}
      ${MLAS_SRC_DIR}/qgemm_kernel_amx.cpp
      ${MLAS_SRC_DIR}/qgemm_kernel_avx2.cpp
      ${MLAS_SRC_DIR}/qgemm_kernel_sse.cpp
      ${MLAS_SRC_DIR}/qgemm_kernel_sse41.cpp
      ${MLAS_SRC_DIR}/intrinsics/avx512/quantize_avx512f.cpp
      ${MLAS_SRC_DIR}/sqnbitgemm_kernel_avx2.cpp
      ${MLAS_SRC_DIR}/sqnbitgemm_kernel_avx512.cpp
      ${MLAS_SRC_DIR}/sqnbitgemm_kernel_avx512vnni.cpp
      ${MLAS_SRC_DIR}/amd64/QgemmU8S8KernelAmx.asm
      ${MLAS_SRC_DIR}/amd64/QgemmU8S8KernelAvx2.asm
      ${MLAS_SRC_DIR}/amd64/QgemmU8U8KernelAvx2.asm
      ${MLAS_SRC_DIR}/amd64/QgemmU8X8KernelAvx2.asm
      ${MLAS_SRC_DIR}/amd64/QgemmU8X8KernelAvx512Core.asm
      ${MLAS_SRC_DIR}/amd64/QgemvU8S8KernelAvx2.asm
      ${MLAS_SRC_DIR}/amd64/QgemvU8S8KernelAvx512Core.asm
      ${MLAS_SRC_DIR}/amd64/QgemvU8S8KernelAvx512Vnni.asm
      ${MLAS_SRC_DIR}/amd64/QgemvU8S8KernelAvxVnni.asm
      ${MLAS_SRC_DIR}/amd64/ConvSymKernelAvx2.asm
      ${MLAS_SRC_DIR}/amd64/ConvSymKernelAvx512Core.asm
      ${MLAS_SRC_DIR}/amd64/DgemmKernelSse2.asm
      ${MLAS_SRC_DIR}/amd64/DgemmKernelAvx.asm
      ${MLAS_SRC_DIR}/amd64/DgemmKernelFma3.asm
      ${MLAS_SRC_DIR}/amd64/DgemmKernelAvx512F.asm
      ${MLAS_SRC_DIR}/amd64/SgemmKernelSse2.asm
      ${MLAS_SRC_DIR}/amd64/SgemmKernelAvx.asm
      ${MLAS_SRC_DIR}/amd64/SgemmKernelM1Avx.asm
      ${MLAS_SRC_DIR}/amd64/SgemmKernelFma3.asm
      ${MLAS_SRC_DIR}/amd64/SgemmKernelAvx512F.asm
      ${MLAS_SRC_DIR}/amd64/SconvKernelSse2.asm
      ${MLAS_SRC_DIR}/amd64/SconvKernelAvx.asm
      ${MLAS_SRC_DIR}/amd64/SconvKernelFma3.asm
      ${MLAS_SRC_DIR}/amd64/SconvKernelAvx512F.asm
      ${MLAS_SRC_DIR}/amd64/SpoolKernelSse2.asm
      ${MLAS_SRC_DIR}/amd64/SpoolKernelAvx.asm
      ${MLAS_SRC_DIR}/amd64/SpoolKernelAvx512F.asm
      ${MLAS_SRC_DIR}/amd64/sgemma.asm
      ${MLAS_SRC_DIR}/amd64/cvtfp16a.asm
      ${MLAS_SRC_DIR}/amd64/SoftmaxKernelAvx.asm
      ${MLAS_SRC_DIR}/amd64/SoftmaxKernelAvx512F.asm
      ${MLAS_SRC_DIR}/amd64/TransKernelFma3.asm
      ${MLAS_SRC_DIR}/amd64/TransKernelAvx512F.asm
      ${MLAS_SRC_DIR}/amd64/LogisticKernelFma3.asm
      ${MLAS_SRC_DIR}/amd64/TanhKernelFma3.asm
      ${MLAS_SRC_DIR}/amd64/ErfKernelFma3.asm
    )
    if (NOT onnxruntime_ORT_MINIMAL_BUILD)
      target_sources(onnxruntime_mlas PRIVATE
        ${MLAS_SRC_DIR}/q4gemm_avx512.cpp
      )
    endif()
  else()
    target_sources(onnxruntime_mlas PRIVATE
      ${MLAS_SRC_DIR}/qgemm_kernel_sse.cpp
      ${MLAS_SRC_DIR}/qgemm_kernel_sse41.cpp
      ${MLAS_SRC_DIR}/i386/SgemmKernelSse2.asm
      ${MLAS_SRC_DIR}/i386/SgemmKernelAvx.asm
    )
  endif()
endfunction()

if (CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
  if (onnxruntime_ENABLE_WEBASSEMBLY_SIMD)
    file(GLOB_RECURSE mlas_platform_srcs
      "${MLAS_SRC_DIR}/wasm_simd/*.cpp"
    )
    set(mlas_platform_srcs
      ${mlas_platform_srcs}
      ${MLAS_SRC_DIR}/qgemm_kernel_wasmsimd.cpp
    )
  else()
    file(GLOB_RECURSE mlas_platform_srcs
      "${MLAS_SRC_DIR}/scalar/*.cpp"
    )
  endif()
  target_sources(onnxruntime_mlas PRIVATE ${mlas_platform_srcs})
elseif(MSVC)
  setup_mlas_source_for_windows()
else()

    if(APPLE)
        get_target_property(ONNXRUNTIME_MLAS_OSX_ARCH onnxruntime_mlas OSX_ARCHITECTURES)

        if(NOT ONNXRUNTIME_MLAS_OSX_ARCH)
         set(ONNXRUNTIME_MLAS_OSX_ARCH ${CMAKE_HOST_SYSTEM_PROCESSOR})
        endif()
        foreach(OSX_ARCH ${ONNXRUNTIME_MLAS_OSX_ARCH})
        if (OSX_ARCH STREQUAL "arm64")
            set(ARM64 TRUE)
        elseif (OSX_ARCH STREQUAL "arm64e")
            set(ARM64 TRUE)
        elseif (OSX_ARCH STREQUAL "arm")
            set(ARM TRUE)
        elseif (OSX_ARCH STREQUAL "x86_64")
            set(X86_64 TRUE)
        elseif (OSX_ARCH STREQUAL "i386")
            set(X86 TRUE)
        endif()
        endforeach()
    elseif(ANDROID)
        if (CMAKE_ANDROID_ARCH_ABI STREQUAL "armeabi-v7a")
          set(ARM TRUE)
        elseif (CMAKE_ANDROID_ARCH_ABI STREQUAL "arm64-v8a")
          set(ARM64 TRUE)
        elseif (CMAKE_ANDROID_ARCH_ABI STREQUAL "x86_64")
          set(X86_64 TRUE)
        elseif (CMAKE_ANDROID_ARCH_ABI STREQUAL "x86")
          set(X86 TRUE)
        endif()
    else()
        #Linux/FreeBSD/PowerPC/...
        #The value of CMAKE_SYSTEM_PROCESSOR should be from `uname -m`
        #Example values:
        #arm64v8/ubuntu -> aarch64
        #arm32v6/alpine -> armv7l
        #arm32v7/centos -> armv7l
        #ppc64le/debian -> ppc64le
        #s390x/ubuntu -> s390x
        #ppc64le/busybox -> ppc64le
        #arm64v8/ubuntu -> aarch64
        #Android: armv7-a aarch64 i686 x86_64
        #chasun: I don't think anyone uses 'arm64'
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "^arm64.*")
          set(ARM64 TRUE)
        elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^arm.*")
          set(ARM TRUE)
        elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^aarch64.*")
          set(ARM64 TRUE)
        elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(powerpc.*|ppc.*)")
          set(POWER TRUE)
        elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(i.86|x86?)$")
          set(X86 TRUE)
        elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64)$")
          set(X86_64 TRUE)
        elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^loongarch64.*")
          set(LOONGARCH64 TRUE)
        endif()
    endif()

    if(APPLE)
      get_target_property(ONNXRUNTIME_MLAS_MACOSX_ARCH onnxruntime_mlas OSX_ARCHITECTURES)
    endif()
    list(LENGTH ONNXRUNTIME_MLAS_MACOSX_ARCH ONNXRUNTIME_MLAS_MACOSX_ARCH_LENGTH)
    if(ONNXRUNTIME_MLAS_MACOSX_ARCH_LENGTH GREATER 1)
        set(ONNXRUNTIME_MLAS_MULTI_ARCH TRUE)
    endif()
    #If ONNXRUNTIME_MLAS_MULTI_ARCH is true, we need to go through every if branch below
    #and split MLAS to multiple static libraries.
    #Otherwise, it works like if(...) elseif(...) elseif(...) endif()
    set(MLAS_SOURCE_IS_NOT_SET 1)
    if(ARM)
        enable_language(ASM)

        set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -mfpu=neon")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpu=neon")

        set(mlas_platform_srcs
          ${MLAS_SRC_DIR}/aarch32/QgemmU8X8KernelNeon.S
          ${MLAS_SRC_DIR}/arm/sgemmc.cpp
          ${MLAS_SRC_DIR}/qgemm_kernel_neon.cpp
        )
        if(NOT ONNXRUNTIME_MLAS_MULTI_ARCH)
          set(MLAS_SOURCE_IS_NOT_SET 0)
        endif()
    endif()
    if(ARM64 AND MLAS_SOURCE_IS_NOT_SET )
        enable_language(ASM)
        set(mlas_platform_srcs
          ${MLAS_SRC_DIR}/aarch64/ConvSymS8KernelDot.S
          ${MLAS_SRC_DIR}/aarch64/ConvSymS8KernelDotLd64.S
          ${MLAS_SRC_DIR}/aarch64/ConvSymU8KernelDot.S
          ${MLAS_SRC_DIR}/aarch64/ConvSymS8KernelNeon.S
          ${MLAS_SRC_DIR}/aarch64/ConvSymU8KernelNeon.S
          ${MLAS_SRC_DIR}/aarch64/DepthwiseQConvSymS8KernelNeon.S
          ${MLAS_SRC_DIR}/aarch64/DepthwiseQConvSymU8KernelNeon.S
          ${MLAS_SRC_DIR}/aarch64/DepthwiseQConvKernelSize9Neon.S
          ${MLAS_SRC_DIR}/aarch64/QgemmU8X8KernelNeon.S
          ${MLAS_SRC_DIR}/aarch64/QgemmS8S8KernelNeon.S
          ${MLAS_SRC_DIR}/aarch64/QgemmU8X8KernelUdot.S
          ${MLAS_SRC_DIR}/aarch64/QgemmS8S8KernelSdot.S
          ${MLAS_SRC_DIR}/aarch64/SgemmKernelNeon.S
          ${MLAS_SRC_DIR}/aarch64/SgemvKernelNeon.S
          ${MLAS_SRC_DIR}/aarch64/SymQgemmS8KernelNeon.S
          ${MLAS_SRC_DIR}/aarch64/SymQgemmS8KernelSdot.S
          ${MLAS_SRC_DIR}/aarch64/SymQgemmS8KernelSdotLd64.S
          ${MLAS_SRC_DIR}/qgemm_kernel_neon.cpp
          ${MLAS_SRC_DIR}/qgemm_kernel_udot.cpp
          ${MLAS_SRC_DIR}/qgemm_kernel_sdot.cpp
          ${MLAS_SRC_DIR}/sqnbitgemm_kernel_neon.cpp
        )
        set_source_files_properties(${MLAS_SRC_DIR}/sqnbitgemm_kernel_neon.cpp
                                    PROPERTIES COMPILE_FLAGS " -march=armv8.2-a+dotprod")
        if (NOT APPLE)
          set(mlas_platform_srcs
            ${mlas_platform_srcs}
            ${MLAS_SRC_DIR}/aarch64/HalfGemmKernelNeon.S
            ${MLAS_SRC_DIR}/aarch64/QgemmS8S8KernelSmmla.S
            ${MLAS_SRC_DIR}/aarch64/QgemmU8X8KernelUmmla.S
            ${MLAS_SRC_DIR}/aarch64/SbgemmKernelNeon.S
            ${MLAS_SRC_DIR}/activate_fp16.cpp
            ${MLAS_SRC_DIR}/dwconv.cpp
            ${MLAS_SRC_DIR}/halfgemm_kernel_neon.cpp
            ${MLAS_SRC_DIR}/pooling_fp16.cpp
            ${MLAS_SRC_DIR}/qgemm_kernel_smmla.cpp
            ${MLAS_SRC_DIR}/qgemm_kernel_ummla.cpp
            ${MLAS_SRC_DIR}/sbgemm_kernel_neon.cpp
          )
          set_source_files_properties(${MLAS_SRC_DIR}/aarch64/HalfGemmKernelNeon.S PROPERTIES COMPILE_FLAGS " -march=armv8.2-a+fp16 ")
          set_source_files_properties(${MLAS_SRC_DIR}/aarch64/QgemmS8S8KernelSmmla.S PROPERTIES COMPILE_FLAGS " -march=armv8.2-a+i8mm ")
          set_source_files_properties(${MLAS_SRC_DIR}/aarch64/QgemmU8X8KernelUmmla.S PROPERTIES COMPILE_FLAGS " -march=armv8.2-a+i8mm ")
          set_source_files_properties(${MLAS_SRC_DIR}/aarch64/SbgemmKernelNeon.S PROPERTIES COMPILE_FLAGS " -march=armv8.2-a+bf16 ")
          set_source_files_properties(${MLAS_SRC_DIR}/activate_fp16.cpp PROPERTIES COMPILE_FLAGS " -march=armv8.2-a+fp16 ")
          set_source_files_properties(${MLAS_SRC_DIR}/dwconv.cpp PROPERTIES COMPILE_FLAGS " -march=armv8.2-a+fp16 ")
          set_source_files_properties(${MLAS_SRC_DIR}/pooling_fp16.cpp PROPERTIES COMPILE_FLAGS " -march=armv8.2-a+fp16 ")
          set_source_files_properties(${MLAS_SRC_DIR}/sbgemm_kernel_neon.cpp PROPERTIES COMPILE_FLAGS " -march=armv8.2-a+bf16 ")
        endif()

        if(ONNXRUNTIME_MLAS_MULTI_ARCH)
            onnxruntime_add_static_library(onnxruntime_mlas_arm64 ${mlas_platform_srcs})
            set_target_properties(onnxruntime_mlas_arm64 PROPERTIES OSX_ARCHITECTURES "arm64")
            list(APPEND ONNXRUNTIME_MLAS_LIBS onnxruntime_mlas_arm64)
            set(mlas_platform_srcs )
        else()
            set(MLAS_SOURCE_IS_NOT_SET 0)
        endif()
    endif()
    if(POWER AND MLAS_SOURCE_IS_NOT_SET)
        set(mlas_platform_srcs
          ${MLAS_SRC_DIR}/power/SgemmKernelPower.cpp
          ${MLAS_SRC_DIR}/dgemm.cpp
          ${MLAS_SRC_DIR}/power/DgemmKernelPower.cpp
          ${MLAS_SRC_DIR}/power/QuantizePower.cpp
        )
        set_source_files_properties(${MLAS_SRC_DIR}/power/SgemmKernelPower.cpp PROPERTIES COMPILE_FLAGS "-DSINGLE")

        check_cxx_compiler_flag("-mcpu=power9" HAS_POWER9)
        if (HAS_POWER9)
          set(mlas_platform_srcs
            ${mlas_platform_srcs}
            ${MLAS_SRC_DIR}/power/QuantizePowerVSX.cpp
          )
          set_source_files_properties(${MLAS_SRC_DIR}/power/QuantizePowerVSX.cpp PROPERTIES COMPILE_FLAGS "-mcpu=power9")
        endif()

        check_cxx_compiler_flag("-mcpu=power10" HAS_POWER10)
        if(HAS_POWER10)
          set(CMAKE_REQUIRED_FLAGS "-mcpu=power10")
          check_cxx_source_compiles("
            #include <altivec.h>
            int main() {
              __vector_quad acc0;
              __builtin_mma_xxsetaccz (&acc0);
              return 0;
            }"
            COMPILES_P10
          )
          if(COMPILES_P10)
            check_cxx_source_compiles("
              #include <sys/auxv.h>
              int main() {
                unsigned long hwcap2 = getauxval(AT_HWCAP2);
                bool HasP10 = ((hwcap2 & PPC_FEATURE2_MMA) && (hwcap2 & PPC_FEATURE2_ARCH_3_1));
                return 0;
              }"
              HAS_P10_RUNTIME
            )
            if (HAS_P10_RUNTIME)
              set_source_files_properties(${MLAS_SRC_DIR}/platform.cpp PROPERTIES COMPILE_FLAGS "-DPOWER10")
              set_source_files_properties(${MLAS_SRC_DIR}/qgemm.cpp PROPERTIES COMPILE_FLAGS "-DPOWER10")
            endif()
            set(mlas_platform_srcs_power10
              ${MLAS_SRC_DIR}/power/SgemmKernelPOWER10.cpp
              ${MLAS_SRC_DIR}/power/DgemmKernelPOWER10.cpp
              ${MLAS_SRC_DIR}/power/qgemm_kernel_power10.cpp
            )
            set_source_files_properties(${MLAS_SRC_DIR}/power/SgemmKernelPOWER10.cpp PROPERTIES COMPILE_FLAGS "-O2 -mcpu=power10 -DSINGLE")
            set_source_files_properties(${MLAS_SRC_DIR}/power/DgemmKernelPOWER10.cpp PROPERTIES COMPILE_FLAGS "-O2 -mcpu=power10")
            set_source_files_properties(${MLAS_SRC_DIR}/power/qgemm_kernel_power10.cpp PROPERTIES COMPILE_FLAGS "-O3 -mcpu=power10")
            set(mlas_platform_srcs
              ${mlas_platform_srcs}
              ${mlas_platform_srcs_power10}
            )
          endif()
        endif()
        if(NOT ONNXRUNTIME_MLAS_MULTI_ARCH)
          set(MLAS_SOURCE_IS_NOT_SET 0)
        endif()
    endif()
    if(X86 AND MLAS_SOURCE_IS_NOT_SET)
        enable_language(ASM)

        set(mlas_platform_srcs_sse2
          ${MLAS_SRC_DIR}/qgemm_kernel_sse.cpp
          ${MLAS_SRC_DIR}/x86/SgemmKernelSse2.S
        )
        set_source_files_properties(${mlas_platform_srcs_sse2} PROPERTIES COMPILE_FLAGS "-msse2")

        set(mlas_platform_srcs_avx
          ${MLAS_SRC_DIR}/x86/SgemmKernelAvx.S
        )
        set_source_files_properties(${mlas_platform_srcs_avx} PROPERTIES COMPILE_FLAGS "-mavx")

        set(mlas_platform_srcs
          ${mlas_platform_srcs_sse2}
          ${mlas_platform_srcs_avx}
        )

        # In r23, NDK remove __x86.get_pc_thunk.* from libatomic. Add our own
        # implementation to avoid external dependency.
        if(ANDROID)
          set(mlas_platform_srcs
            ${mlas_platform_srcs}
            ${MLAS_SRC_DIR}/x86/x86.get_pc_thunk.S
          )
        endif()

        if(NOT ONNXRUNTIME_MLAS_MULTI_ARCH)
          set(MLAS_SOURCE_IS_NOT_SET 0)
        endif()
    endif()
    if(X86_64 AND MLAS_SOURCE_IS_NOT_SET)
        enable_language(ASM)

        # Forward the flags for the minimum target platform version from the C
        # compiler to the assembler. This works around CMakeASMCompiler.cmake.in
        # not including the logic to set this flag for the assembler.
        set(CMAKE_ASM${ASM_DIALECT}_OSX_DEPLOYMENT_TARGET_FLAG "${CMAKE_C_OSX_DEPLOYMENT_TARGET_FLAG}")

        # The LLVM assembler does not support the .arch directive to enable instruction
        # set extensions and also doesn't support AVX-512F instructions without
        # turning on support via command-line option. Group the sources by the
        # instruction set extension and explicitly set the compiler flag as appropriate.

        set(mlas_platform_srcs_sse2
          ${MLAS_SRC_DIR}/qgemm_kernel_sse.cpp
          ${MLAS_SRC_DIR}/x86_64/DgemmKernelSse2.S
          ${MLAS_SRC_DIR}/x86_64/SgemmKernelSse2.S
          ${MLAS_SRC_DIR}/x86_64/SgemmTransposePackB16x4Sse2.S
          ${MLAS_SRC_DIR}/x86_64/SconvKernelSse2.S
          ${MLAS_SRC_DIR}/x86_64/SpoolKernelSse2.S
        )
        set_source_files_properties(${mlas_platform_srcs_sse2} PROPERTIES COMPILE_FLAGS "-msse2")

        set(mlas_platform_srcs_avx
          ${MLAS_SRC_DIR}/x86_64/DgemmKernelAvx.S
          ${MLAS_SRC_DIR}/x86_64/SgemmKernelAvx.S
          ${MLAS_SRC_DIR}/x86_64/SgemmKernelM1Avx.S
          ${MLAS_SRC_DIR}/x86_64/SgemmKernelM1TransposeBAvx.S
          ${MLAS_SRC_DIR}/x86_64/SgemmTransposePackB16x4Avx.S
          ${MLAS_SRC_DIR}/x86_64/SconvKernelAvx.S
          ${MLAS_SRC_DIR}/x86_64/SpoolKernelAvx.S
          ${MLAS_SRC_DIR}/x86_64/SoftmaxKernelAvx.S
          ${MLAS_SRC_DIR}/intrinsics/avx/min_max_elements.cpp
        )
        set_source_files_properties(${mlas_platform_srcs_avx} PROPERTIES COMPILE_FLAGS "-mavx")

        set(mlas_platform_srcs_avx2
          ${MLAS_SRC_DIR}/x86_64/QgemmU8S8KernelAvx2.S
          ${MLAS_SRC_DIR}/x86_64/QgemvU8S8KernelAvx2.S
          ${MLAS_SRC_DIR}/x86_64/QgemmU8U8KernelAvx2.S
          ${MLAS_SRC_DIR}/x86_64/QgemvU8S8KernelAvxVnni.S
          ${MLAS_SRC_DIR}/x86_64/QgemmU8X8KernelAvx2.S
          ${MLAS_SRC_DIR}/x86_64/ConvSymKernelAvx2.S
          ${MLAS_SRC_DIR}/x86_64/DgemmKernelFma3.S
          ${MLAS_SRC_DIR}/x86_64/SgemmKernelFma3.S
          ${MLAS_SRC_DIR}/x86_64/SconvKernelFma3.S
          ${MLAS_SRC_DIR}/x86_64/TransKernelFma3.S
          ${MLAS_SRC_DIR}/x86_64/LogisticKernelFma3.S
          ${MLAS_SRC_DIR}/x86_64/TanhKernelFma3.S
          ${MLAS_SRC_DIR}/x86_64/ErfKernelFma3.S
          ${MLAS_SRC_DIR}/intrinsics/avx2/qladd_avx2.cpp
          ${MLAS_SRC_DIR}/intrinsics/avx2/qdwconv_avx2.cpp
          ${MLAS_SRC_DIR}/sqnbitgemm_kernel_avx2.cpp
        )
        set_source_files_properties(${mlas_platform_srcs_avx2} PROPERTIES COMPILE_FLAGS "-mavx2 -mfma")

        set(mlas_platform_srcs_avx512f
          ${MLAS_SRC_DIR}/x86_64/DgemmKernelAvx512F.S
          ${MLAS_SRC_DIR}/x86_64/SgemmKernelAvx512F.S
          ${MLAS_SRC_DIR}/x86_64/SconvKernelAvx512F.S
          ${MLAS_SRC_DIR}/x86_64/SoftmaxKernelAvx512F.S
          ${MLAS_SRC_DIR}/x86_64/SpoolKernelAvx512F.S
          ${MLAS_SRC_DIR}/x86_64/TransKernelAvx512F.S
          ${MLAS_SRC_DIR}/intrinsics/avx512/quantize_avx512f.cpp
        )
        set_source_files_properties(${mlas_platform_srcs_avx512f} PROPERTIES COMPILE_FLAGS "-mavx512f")

        set(mlas_platform_srcs_avx512core
          ${MLAS_SRC_DIR}/x86_64/QgemvU8S8KernelAvx512Core.S
          ${MLAS_SRC_DIR}/x86_64/QgemvU8S8KernelAvx512Vnni.S
          ${MLAS_SRC_DIR}/x86_64/QgemmU8X8KernelAvx512Core.S
          ${MLAS_SRC_DIR}/x86_64/ConvSymKernelAvx512Core.S
          ${MLAS_SRC_DIR}/sqnbitgemm_kernel_avx512.cpp
        )
        set_source_files_properties(${mlas_platform_srcs_avx512core} PROPERTIES COMPILE_FLAGS "-mavx512bw -mavx512dq -mavx512vl")

        set(mlas_platform_srcs_avx512vnni
          ${MLAS_SRC_DIR}/sqnbitgemm_kernel_avx512vnni.cpp
        )
        set_source_files_properties(${mlas_platform_srcs_avx512vnni} PROPERTIES COMPILE_FLAGS "-mfma -mavx512vnni -mavx512bw -mavx512dq -mavx512vl -mavx512f")

        set(mlas_platform_srcs
          ${MLAS_SRC_DIR}/activate_fp16.cpp
          ${MLAS_SRC_DIR}/dwconv.cpp
          ${MLAS_SRC_DIR}/dgemm.cpp
          ${MLAS_SRC_DIR}/pooling_fp16.cpp
          ${MLAS_SRC_DIR}/qgemm_kernel_avx2.cpp
          ${mlas_platform_srcs_sse2}
          ${mlas_platform_srcs_avx}
          ${mlas_platform_srcs_avx2}
          ${mlas_platform_srcs_avx512f}
          ${mlas_platform_srcs_avx512core}
          ${mlas_platform_srcs_avx512vnni}
        )

        if (NOT onnxruntime_ORT_MINIMAL_BUILD)
          set(mlas_platform_srcs
            ${mlas_platform_srcs}
            ${MLAS_SRC_DIR}/q4gemm_avx512.cpp
          )
          set_source_files_properties(${MLAS_SRC_DIR}/q4gemm_avx512.cpp PROPERTIES COMPILE_FLAGS "-mfma -mavx512vnni -mavx512bw -mavx512dq -mavx512vl -mavx512f")
        endif()
        if(NOT APPLE)
          set(mlas_platform_srcs
            ${mlas_platform_srcs}
	        ${MLAS_SRC_DIR}/x86_64/QgemmU8S8KernelAmxCommon.S
            ${MLAS_SRC_DIR}/qgemm_kernel_amx.cpp
            ${MLAS_SRC_DIR}/x86_64/QgemmU8S8KernelAmx.S
            )
          set_source_files_properties(${MLAS_SRC_DIR}/qgemm_kernel_amx.cpp PROPERTIES COMPILE_FLAGS "-mavx2 -mavx512bw -mavx512dq -mavx512vl -mavx512f")
          set_source_files_properties(${MLAS_SRC_DIR}/x86_64/QgemmU8S8KernelAmx.S PROPERTIES COMPILE_FLAGS "-mavx2 -mavx512bw -mavx512dq -mavx512vl -mavx512f")
        endif()

        if(ONNXRUNTIME_MLAS_MULTI_ARCH)
          onnxruntime_add_static_library(onnxruntime_mlas_x86_64 ${mlas_platform_srcs})
          set_target_properties(onnxruntime_mlas_x86_64 PROPERTIES OSX_ARCHITECTURES "x86_64")
          list(APPEND ONNXRUNTIME_MLAS_LIBS onnxruntime_mlas_x86_64)
          set(mlas_platform_srcs )
        else()
          set(MLAS_SOURCE_IS_NOT_SET 0)
        endif()
    endif()
    if(LOONGARCH64 AND MLAS_SOURCE_IS_NOT_SET)
        set(mlas_platform_srcs
          ${MLAS_SRC_DIR}/qgemm_kernel_lsx.cpp
          ${MLAS_SRC_DIR}/loongarch64/SgemmKernelLasx.S
          ${MLAS_SRC_DIR}/loongarch64/DgemmKernelLsx.S
          ${MLAS_SRC_DIR}/loongarch64/DgemmKernelLasx.S
          ${MLAS_SRC_DIR}/loongarch64/SgemmKernelLsx.S
          ${MLAS_SRC_DIR}/loongarch64/SconvKernelLsx.S
          ${MLAS_SRC_DIR}/loongarch64/SconvKernelLasx.S
          ${MLAS_SRC_DIR}/loongarch64/SpoolKernelLSX.S
          ${MLAS_SRC_DIR}/loongarch64/SpoolKernelLasx.S
          ${MLAS_SRC_DIR}/loongarch64/SgemmTransposePackB16x4LSX.S
          ${MLAS_SRC_DIR}/loongarch64/SgemmTransposePackB16x4Lasx.S
          ${MLAS_SRC_DIR}/loongarch64/SoftmaxKernelLasx.S
            )
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mlsx -mlasx")
        if(NOT ONNXRUNTIME_MLAS_MULTI_ARCH)
          set(MLAS_SOURCE_IS_NOT_SET 0)
        endif()
    endif()
    if(NOT ONNXRUNTIME_MLAS_MULTI_ARCH AND MLAS_SOURCE_IS_NOT_SET)
        file(GLOB_RECURSE mlas_platform_srcs
          "${MLAS_SRC_DIR}/scalar/*.cpp")
    endif()
    target_sources(onnxruntime_mlas PRIVATE ${mlas_platform_srcs})
endif()


foreach(mlas_target ${ONNXRUNTIME_MLAS_LIBS})
    target_include_directories(${mlas_target} PRIVATE ${MLAS_INC_DIR} ${MLAS_SRC_DIR})
    onnxruntime_add_include_to_target(${mlas_target} ${GSL_TARGET})

    set_target_properties(${mlas_target} PROPERTIES FOLDER "ONNXRuntime")
endforeach()

if (WIN32)
  target_compile_options(onnxruntime_mlas PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:/wd6385>" "$<$<COMPILE_LANGUAGE:CXX>:/wd4127>")
  if (onnxruntime_ENABLE_STATIC_ANALYSIS)
    target_compile_options(onnxruntime_mlas PRIVATE  "$<$<COMPILE_LANGUAGE:CXX>:/analyze:stacksize 131072>")
  endif()
endif()

if (PLATFORM_NAME STREQUAL "macabi")
  # Needed for maccatalyst C compilation
  # i.e. the flags below add "--target=x86_64-apple-ios14.0-macabi -ffunction-sections -fdata-sections"
  target_compile_options(onnxruntime_mlas PRIVATE ${CMAKE_C_FLAGS})
endif()

if (NOT onnxruntime_BUILD_SHARED_LIB)
    install(TARGETS onnxruntime_mlas
            ARCHIVE   DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY   DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME   DESTINATION ${CMAKE_INSTALL_BINDIR}
            FRAMEWORK DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

# set up source group for MLAS source files
block()
  set(source_group_srcs)
  foreach(mlas_target ${ONNXRUNTIME_MLAS_LIBS})
    get_target_property(mlas_target_srcs ${mlas_target} SOURCES)
    foreach(mlas_target_src ${mlas_target_srcs})
      cmake_path(IS_PREFIX MLAS_ROOT ${mlas_target_src} in_mlas_root)
      if(in_mlas_root)
        list(APPEND source_group_srcs ${mlas_target_src})
      endif()
    endforeach()
  endforeach()
  source_group(TREE ${MLAS_ROOT} FILES ${source_group_srcs})
endblock()

#
#if (NOT onnxruntime_ORT_MINIMAL_BUILD)
#
#  #
#  # Command line tool for quantization and de-quantization of 2-D fp32 tensors
#  # based on block-wise quantization of int4
#  #
#
#  onnxruntime_add_executable(onnxruntime_mlas_q4dq
#    ${MLAS_SRC_DIR}/q4_dq_cli.cpp
#  )
#  target_include_directories(onnxruntime_mlas_q4dq PRIVATE ${MLAS_INC_DIR} ${MLAS_SRC_DIR})
#  set_target_properties(onnxruntime_mlas_q4dq PROPERTIES FOLDER "ONNXRuntimeTest")
#
#  target_link_libraries(onnxruntime_mlas_q4dq PRIVATE ${ONNXRUNTIME_MLAS_LIBS} onnxruntime_common)
#  if (CPUINFO_SUPPORTED AND NOT CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
#    target_link_libraries(onnxruntime_mlas_q4dq PRIVATE cpuinfo)
#  endif()
#  if(NOT WIN32)
#    target_link_libraries(onnxruntime_mlas_q4dq PRIVATE nsync::nsync_cpp ${CMAKE_DL_LIBS})
#  endif()
#  if (CMAKE_SYSTEM_NAME STREQUAL "Android")
#    target_link_libraries(onnxruntime_mlas_q4dq PRIVATE ${android_shared_libs})
#  endif()
#
#  if(WIN32)
#    target_link_libraries(onnxruntime_mlas_q4dq PRIVATE debug Dbghelp Advapi32)
#  endif()
#  if (onnxruntime_LINK_LIBATOMIC)
#    target_link_libraries(onnxruntime_mlas_q4dq PRIVATE atomic)
#  endif()
#  target_link_libraries(onnxruntime_mlas_q4dq PRIVATE Threads::Threads)
#
#  if (CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
#    if (onnxruntime_ENABLE_WEBASSEMBLY_THREADS)
#      set_target_properties(onnxruntime_mlas_q4dq PROPERTIES LINK_FLAGS "-s ALLOW_MEMORY_GROWTH=1 -s PROXY_TO_PTHREAD=1 -s EXIT_RUNTIME=1")
#    else()
#      set_target_properties(onnxruntime_mlas_q4dq PROPERTIES LINK_FLAGS "-s ALLOW_MEMORY_GROWTH=1")
#    endif()
#  endif()
#
#endif()

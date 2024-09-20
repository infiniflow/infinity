// MinIO C++ Library for Amazon S3 Compatible Cloud Storage
// Copyright 2022-2024 MinIO, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0

#ifndef MINIO_CPP_CONFIG_H_INCLUDED
#define MINIO_CPP_CONFIG_H_INCLUDED

#define MINIO_CPP_STRINGIFY(x) #x
#define MINIO_CPP_TO_STRING(x) MINIO_CPP_STRINGIFY(x)

#define MINIO_CPP_MAJOR_VERSION 0
#define MINIO_CPP_MINOR_VERSION 3
#define MINIO_CPP_PATCH_VERSION 0

#define MINIO_CPP_VERSION                                                  \
  "" MINIO_CPP_TO_STRING(MINIO_CPP_MAJOR_VERSION) "." MINIO_CPP_TO_STRING( \
      MINIO_CPP_MINOR_VERSION) "." MINIO_CPP_TO_STRING(MINIO_CPP_PATCH_VERSION)

#if defined(_M_X64) || defined(__x86_64__)
#define MINIO_CPP_ARCH_STRING "x86_64"
#elif defined(_M_IX86) || defined(__X86__) || defined(__i386__)
#define MINIO_CPP_ARCH_STRING "x86"
#elif defined(_M_ARM64) || defined(__arm64__) || defined(__aarch64__)
#define MINIO_CPP_ARCH_STRING "arm64"
#elif defined(_M_ARM) || defined(_M_ARMT) || defined(__arm__) || \
    defined(__thumb__) || defined(__thumb2__)
#define MINIO_CPP_ARCH_STRING "arm32"
#elif defined(_MIPS_ARCH_MIPS64) || defined(__mips64)
#define MINIO_CPP_ARCH_STRING "mips64"
#elif defined(_MIPS_ARCH_MIPS32) || defined(_M_MRX000) || defined(__mips__)
#define MINIO_CPP_ARCH_STRING "mips32"
#elif (defined(__riscv) || defined(__riscv__)) && defined(__riscv_xlen)
#define MINIO_CPP_ARCH_STRING "riscv" MINIO_CPP_STRINGIFY(__riscv_xlen)
#elif defined(__loongarch__)
#define MINIO_CPP_ARCH_STRING "loongarch"
#elif defined(__s390__) || defined(__s390x__)
#define MINIO_CPP_ARCH_STRING "s390"
#else
#define MINIO_CPP_ARCH_STRING "unknown-arch"
#endif

#if defined(_WIN32) || defined(__CYGWIN__)
#define MINIO_CPP_PLATFORM_STRING "windows"
#elif defined(__ANDROID__)
#define MINIO_CPP_PLATFORM_STRING "android"
#elif defined(__linux__) || defined(__linux)
#define MINIO_CPP_PLATFORM_STRING "linux"
#elif defined(__APPLE__) || defined(__MACH__)
#define MINIO_CPP_PLATFORM_STRING "darwin"
#elif defined(__FreeBSD__)
#define MINIO_CPP_PLATFORM_STRING "freebsd"
#elif defined(__NetBSD__)
#define MINIO_CPP_PLATFORM_STRING "netbsd"
#elif defined(__OpenBSD__)
#define MINIO_CPP_PLATFORM_STRING "openbsd"
#else
#define MINIO_CPP_PLATFORM_STRING "unknown-os"
#endif

#define DEFAULT_USER_AGENT                                       \
  "MinIO (" MINIO_CPP_PLATFORM_STRING "; " MINIO_CPP_ARCH_STRING \
  ") minio-cpp/" MINIO_CPP_VERSION ""

#endif  // MINIO_CPP_CONFIG_H_INCLUDED

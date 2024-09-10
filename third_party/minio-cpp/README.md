# MinIO C++ Client SDK for Amazon S3 Compatible Cloud Storage [![Slack](https://slack.min.io/slack?type=svg)](https://slack.min.io) [![Sourcegraph](https://sourcegraph.com/github.com/minio/minio-cpp/-/badge.svg)](https://sourcegraph.com/github.com/minio/minio-cpp?badge) [![Apache V2 License](https://img.shields.io/badge/license-Apache%20V2-blue.svg)](https://github.com/minio/minio-cpp/blob/master/LICENSE)

MinIO C++ SDK is Simple Storage Service (aka S3) client to perform bucket and object operations to any Amazon S3 compatible object storage service.

For a complete list of APIs and examples, please take a look at the [MinIO C++ Client API Reference](https://minio-cpp.min.io/)

## Build Requirements

* [cmake](https://cmake.org/) 3.10 or higher.
* [vcpkg](https://vcpkg.io/en/index.html) package manager.
* A working C++ compiler that supports at least C++17.

## Installation via `vcpkg`

MinIO C++ client SDK can be installed via `vcpkg` package manager:

```bash
$ vcpkg install minio-cpp
```

Typically `minio-cpp` will be part of dependencies specified in `vcpkg.json` file:

```json
{
  "$schema": "https://raw.githubusercontent.com/microsoft/vcpkg-tool/main/docs/vcpkg.schema.json",
  "name": "your-project",
  "version": "0.0.1",
  "dependencies": [
    { "name": "minio-cpp" }
  ]
}
```

## Using `minio-cpp` with cmake

MinIO C++ cliend SDK can be consumed as a dependency in CMakeLists.txt, the following can be used as an example:

```cmake
cmake_minimum_required(VERSION 3.10)

project(miniocpp_example LANGUAGES C CXX)

# This will try to find miniocpp package and all its dependencies.
find_package(miniocpp REQUIRED)

# Create an executable called miniocpp-example:
add_executable(miniocpp-example example.cpp)

# Link the executable to miniocpp and all its dependencies:
target_link_libraries(miniocpp-example PRIVATE miniocpp::miniocpp)

# Make sure you are using at least C++17:
target_compile_features(miniocpp-example PUBLIC cxx_std_17)
```

Note that `miniocpp::miniocpp` is a cmake imported target, which contains all the instructions necessary to use `minio-cpp` library from your cmake projet file.

## Hacking minio-cpp

In order to run minio-cpp tests and examples, you can do the following assuming `VCPKG_ROOT` points to a valid `vcpkg` installation:

```bash
$ git clone https://github.com/minio/minio-cpp
$ cd minio-cpp
$ ${VCPKG_ROOT}/vcpkg install
$ cmake . -B build/Debug -DCMAKE_BUILD_TYPE=Debug -DMINIO_CPP_TEST=ON -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake
$ cmake --build ./build/Debug
```

Note that cmake also supports multi-configuration generators. Multi-configuration generators don't use `CMAKE_BUILD_TYPE` during configure time. For example a Visual Studio project can be setup the following way:

```bash
$ git clone https://github.com/minio/minio-cpp
$ cd minio-cpp
$ ${VCPKG_ROOT}/vcpkg install
$ cmake . -B build -DMINIO_CPP_TEST=ON -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake
$ cmake --build ./build --config Debug
```

The examples above assumed that you have `vcpkg` already installed and you have a `VCPKG_ROOT` environment variable set. This is common if you use `vcpkg` to handle dependencies of multiple projects as only a single installation of `vcpkg` is required in that case. If you don't have `vcpkg` installed and you only want to use it to test `minio-cpp`, it's possible to install it locally like this:

```bash
$ git clone https://github.com/minio/minio-cpp
$ cd minio-cpp
$ git clone https://github.com/microsoft/vcpkg.git
$ ./vcpkg/bootstrap-vcpkg.sh
$ ./vcpkg/vcpkg install
$ cmake . -B ./build/Debug -DCMAKE_BUILD_TYPE=Debug -DMINIO_CPP_TEST=ON -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake
$ cmake --build ./build/Debug
```

We recommend the setup with `VCPKG_ROOT` for development. In that case there is a `configure.sh` script, that can be used to create both Debug and Release projects:

```bash
$ git clone https://github.com/minio/minio-cpp
$ cd minio-cpp
$ ./configure.sh -DMINIO_CPP_TEST=ON
```

## Example:: file-uploader.cc

```c++
#include <miniocpp/client.h>

int main(int argc, char* argv[]) {
  // Create S3 base URL.
  minio::s3::BaseUrl base_url("play.min.io");

  // Create credential provider.
  minio::creds::StaticProvider provider(
      "Q3AM3UQ867SPQQA43P2F", "zuf+tfteSlswRu7BJ86wekitnifILbZam1KYY3TG");

  // Create S3 client.
  minio::s3::Client client(base_url, &provider);
  std::string bucket_name = "asiatrip";

  // Check 'asiatrip' bucket exist or not.
  bool exist;
  {
    minio::s3::BucketExistsArgs args;
    args.bucket = bucket_name;

    minio::s3::BucketExistsResponse resp = client.BucketExists(args);
    if (!resp) {
      std::cout << "unable to do bucket existence check; " << resp.Error()
                << std::endl;
      return EXIT_FAILURE;
    }

    exist = resp.exist;
  }

  // Make 'asiatrip' bucket if not exist.
  if (!exist) {
    minio::s3::MakeBucketArgs args;
    args.bucket = bucket_name;

    minio::s3::MakeBucketResponse resp = client.MakeBucket(args);
    if (!resp) {
      std::cout << "unable to create bucket; " << resp.Error() << std::endl;
      return EXIT_FAILURE;
    }
  }

  // Upload '/home/user/Photos/asiaphotos.zip' as object name
  // 'asiaphotos-2015.zip' to bucket 'asiatrip'.
  minio::s3::UploadObjectArgs args;
  args.bucket = bucket_name;
  args.object = "asiaphotos-2015.zip";
  args.filename = "/home/user/Photos/asiaphotos.zip";

  minio::s3::UploadObjectResponse resp = client.UploadObject(args);
  if (!resp) {
    std::cout << "unable to upload object; " << resp.Error() << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "'/home/user/Photos/asiaphotos.zip' is successfully uploaded as "
            << "object 'asiaphotos-2015.zip' to bucket 'asiatrip'."
            << std::endl;

  return EXIT_SUCCESS;
}
```

## License

This SDK is distributed under the [Apache License, Version 2.0](https://www.apache.org/licenses/LICENSE-2.0), see [LICENSE](https://github.com/minio/minio-cpp/blob/master/LICENSE) for more information.

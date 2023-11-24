from ubuntu:23.10

RUN sed -i 's@//.*archive.ubuntu.com@//mirrors.ustc.edu.cn@g' /etc/apt/sources.list
RUN apt update

# Clang 17 or 18 is required for C++20 modules. GCC is not supported.
RUN apt install -y clang-*-17
RUN ln -s /usr/lib/llvm-17/bin/clang-scan-deps /usr/bin/clang-scan-deps

# CMake 3.28+ is requrired for C++20 modules.
# download https://github.com/Kitware/CMake/releases/download/v3.28.0-rc5/cmake-3.28.0-rc5-linux-x86_64.tar.gz
COPY cmake-3.28.0-rc5-linux-x86_64.tar.gz .
RUN tar xzf cmake-3.28.0-rc5-linux-x86_64.tar.gz && cp -rf cmake-3.28.0-rc5-linux-x86_64/bin/* /usr/local/bin && cp -rf cmake-3.28.0-rc5-linux-x86_64/share/* /usr/local/share

RUN apt install -y wget emacs-nox git make ninja-build bison flex libomp-17-dev libblas-dev liblapack-dev libboost1.81-dev liburing-dev libgflags-dev libleveldb-dev

ENV CC=/usr/bin/clang-17
ENV CXX=/usr/bin/clang++-17

# iresearch requires lz4
# git clone https://github.com/lz4/lz4.git && tar czvf lz4.tgz lz4
COPY lz4.tgz .
RUN tar xzf lz4.tgz && cd lz4 && make && make install

# dependencies for brpc
RUN apt install -y libgrpc++-dev libgrpc-dev protobuf-compiler-grpc libprotobuf-dev python3-grpcio python3-grpc-tools libprotoc-dev libprotoc-dev

ENV LZ4_ROOT=/usr/local
ENTRYPOINT [ "bash", "-c", "while true; do sleep 60; done"]

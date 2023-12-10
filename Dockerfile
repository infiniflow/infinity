FROM ubuntu:23.10

RUN sed -i 's@//.*archive.ubuntu.com@//mirrors.ustc.edu.cn@g' /etc/apt/sources.list
RUN apt update

# Clang 17+ is required for C++20 modules. GCC is not supported.
RUN apt install -y clang-*-17
RUN ln -s /usr/bin/clang-scan-deps-17 /usr/bin/clang-scan-deps && ln -s /usr/bin/clang-format-17 /usr/bin/clang-format && ln -s /usr/bin/clang-tidy-17 /usr/bin/clang-tidy

ENV CC=/usr/bin/clang-17
ENV CXX=/usr/bin/clang++-17

# tools
RUN apt install -y wget curl emacs-nox git build-essential ninja-build bison flex python3-numpy postgresql-client

# rust, refers to https://rsproxy.cn/
ENV RUSTUP_DIST_SERVER="https://rsproxy.cn"
ENV RUSTUP_UPDATE_ROOT="https://rsproxy.cn/rustup"
ENV PATH=/root/.cargo/bin:$PATH
RUN wget -O rustup-init.sh https://rsproxy.cn/rustup-init.sh && sh rustup-init.sh -y
RUN mkdir -vp $HOME/.cargo && echo "[source.crates-io]" >> $HOME/.cargo/config \
    && echo "replace-with = 'rsproxy-sparse'" >> $HOME/.cargo/config \
    && echo "[source.rsproxy]" >> $HOME/.cargo/config \
    && echo "registry = 'https://rsproxy.cn/index/'" >> $HOME/.cargo/config \
    && echo "[source.rsproxy-sparse]" >> $HOME/.cargo/config \
    && echo "registry = 'sparse+https://rsproxy.cn/index/'" >> $HOME/.cargo/config \
    && echo "[registries.rsproxy]" >> $HOME/.cargo/config \
    && echo "index = 'https://rsproxy.cn/crates.io-index'" >> $HOME/.cargo/config \
	&& echo "[net]" >> $HOME/.cargo/config \
	&& echo "git-fetch-with-cli = true" >> $HOME/.cargo/config

# sqllogictest
RUN cargo install sqllogictest-bin


# CMake 3.28+ is requrired for C++20 modules.
# download https://github.com/Kitware/CMake/releases/download/v3.28.0/cmake-3.28.0-linux-x86_64.tar.gz
COPY cmake-3.28.0-linux-x86_64.tar.gz .
RUN tar xzf cmake-3.28.0-linux-x86_64.tar.gz && cp -rf cmake-3.28.0-linux-x86_64/bin/* /usr/local/bin && cp -rf cmake-3.28.0-linux-x86_64/share/* /usr/local/share && rm -fr cmake-3.28.0-linux-x86_64

# iresearch requires lz4
# git clone https://github.com/lz4/lz4.git && tar czvf lz4.tgz lz4
COPY lz4.tgz .
RUN tar xzf lz4.tgz && cd lz4 && make && make install && rm -fr lz4
ENV LZ4_ROOT=/usr/local

# build dependencies
RUN apt install -y libomp-17-dev libblas-dev liblapack-dev libboost1.81-dev liburing-dev libgflags-dev libleveldb-dev libevent-dev libthrift-dev

RUN apt install -y thrift-compiler python3-pytest python3-venv

ENTRYPOINT [ "bash", "-c", "while true; do sleep 60; done"]

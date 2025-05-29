FROM ubuntu:24.10

RUN apt update
RUN apt install -y curl git git-lfs zip build-essential entr libssl-dev libdouble-conversion-dev pkg-config ninja-build cmake zlib1g-dev libffi-dev clang libclang-dev llvm llvm-dev libzip-dev libbz2-dev doctest-dev gcc g++ libgc-dev
RUN apt install -y vim gdb lldb file valgrind

# Setup a user
RUN groupadd cpptracegroup
RUN useradd -m -g cpptracegroup -s /bin/bash cpptrace
RUN mkdir /opt/work/ && chown cpptrace:cpptracegroup /opt/work/
USER cpptrace

WORKDIR /opt/
WORKDIR /opt/work

# RUN git clone --recurse-submodules https://github.com/jank-lang/jank.git
# WORKDIR /opt/work/jank/compiler+runtime
# RUN git checkout 5668b16
# RUN CC=clang CXX=clang++ ./bin/configure -GNinja -DCMAKE_BUILD_TYPE=Debug
# RUN ./bin/compile

# WORKDIR /opt/work/jank
# WORKDIR /opt/work/jank/compiler+runtime

COPY ./entry.sh .
ENTRYPOINT ["./entry.sh"]
# ENTRYPOINT ["/bin/bash"]

# podman build -t cpptrace-container .
# podman run --user=cpptrace --cap-drop=all --network none -it cpptrace-container

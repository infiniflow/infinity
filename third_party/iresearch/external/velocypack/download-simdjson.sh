rm -rf simdjson
git clone https://github.com/simdjson/simdjson
cd simdjson
mkdir build && cd build
cmake ../ -DCMAKE_BUILD_TYPE=Release 
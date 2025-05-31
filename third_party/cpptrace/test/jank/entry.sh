#!/bin/sh

echo "------------------------------------ BUILDING ------------------------------------"

cd jank/compiler+runtime

CC=clang CXX=clang++ ./bin/configure -GNinja -DCMAKE_BUILD_TYPE=Debug && ./bin/compile

cat > test.jank << EOF
(ns test)

(defn foo []
  (throw "meow"))

(defn -main [& args]
  (foo))
(-main)
EOF

./build/jank run test.jank
# gdb --args ./build/jank run test.jank

exec /bin/bash

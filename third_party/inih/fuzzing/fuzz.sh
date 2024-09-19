#!/usr/bin/env bash
../../afl-2.52b/afl-fuzz -i testcases -o findings -- ./inihfuzz @@

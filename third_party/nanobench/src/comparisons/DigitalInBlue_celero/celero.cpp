#include <celero/Celero.h>

#include <random>

// g++ -O2 main.cpp -I. -L /home/martinus/git/Celero/build -lcelero -o ce
// LD_LIBRARY_PATH=/home/martinus/git/Celero/build ./ce
CELERO_MAIN

// This gives segmentation fault :-(

uint64_t x = 1;
BENCHMARK(FrameworkComparison, ComparisonFast, 50, 50000) {
    celero::DoNotOptimizeAway(x += x);
}

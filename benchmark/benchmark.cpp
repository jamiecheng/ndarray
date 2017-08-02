//
// Created by jamie on 8/3/17.
//

#include "benchmark/benchmark.h"
#include "array.hpp"

using Array = nd::array<double>;

static void BM_transpose(benchmark::State& state) {
    Array a({100, 100}, 0.0);

    while (state.KeepRunning()) {
        a.transpose();
    }
}
BENCHMARK(BM_transpose);

static void BM_at(benchmark::State& state) {
    Array a({100, 100}, 0.0);

    while (state.KeepRunning()) {
        a.at(10);
    }
}
BENCHMARK(BM_at);

static void BM_item(benchmark::State& state) {
    Array a({100, 100}, 0.0);

    while (state.KeepRunning()) {
        a.item({10, 10});
    }
}
BENCHMARK(BM_item);

BENCHMARK_MAIN();
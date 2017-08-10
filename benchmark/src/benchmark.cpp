//
// Created by jamie on 8/3/17.
//

#include "benchmark/benchmark.h"
#include "array.hpp"
#include "array.h"

using array = nd::array<double>;

/////////////////////////////
//    ndarray benchmark    //
/////////////////////////////

static void BM_constructor(benchmark::State &state) {
    array a({1000, 1000}, 0.0);
    array b;

    while (state.KeepRunning()) {
        b = a;
    }
}

BENCHMARK(BM_constructor);

static void BM_transpose(benchmark::State &state) {
    array a({1000, 1000}, 0.0);

    while (state.KeepRunning()) {
        a.transpose();
    }
}

BENCHMARK(BM_transpose);

static void BM_at(benchmark::State &state) {
    array a({100, 100}, 0.0);

    while (state.KeepRunning()) {
        a.at(10);
    }
}

BENCHMARK(BM_at);

static void BM_item(benchmark::State &state) {
    array a({100, 100}, 0.0);

    while (state.KeepRunning()) {
        a.item({10, 10});
    }
}

BENCHMARK(BM_item);

static void BM_sum(benchmark::State &state) {
    array a({100, 100}, 0.0);

    while (state.KeepRunning()) {
        a + 1.0;
    }
}

BENCHMARK(BM_sum);

static void BM_dot(benchmark::State &state) {
    array a({10, 10}, 0.0);

    array b({10, 10}, 0.0);

    while (state.KeepRunning()) {
        a.dot(b);
    }
}

BENCHMARK(BM_dot);

static void BM_hash(benchmark::State &state) {
    array a({100, 100}, 0.);

    while (state.KeepRunning()) {
        a.hash();
    }
}

BENCHMARK(BM_hash);

static void BM_compare(benchmark::State &state) {
    array a({100, 100}, 0.);
    array b({100, 100}, 0.);

    while (state.KeepRunning()) {
        a == b;
    }
}

BENCHMARK(BM_compare);

/////////////////////////////
//    old lib benchmark    //
/////////////////////////////

static void BM_nnet_constructor(benchmark::State &state) {

    while (state.KeepRunning()) {
        nnet::Array a(1000, 1000);
    }
}

BENCHMARK(BM_nnet_constructor);

static void BM_nnet_sum(benchmark::State &state) {
    nnet::Array a(100, 100);

    while (state.KeepRunning()) {
        a + 1;
    }
}

BENCHMARK(BM_nnet_sum);

static void BM_nnet_transpose(benchmark::State &state) {
    nnet::Array a(1000, 1000);

    while (state.KeepRunning()) {
        a.transponse();
    }
}

BENCHMARK(BM_nnet_transpose);

static void BM_nnet_item(benchmark::State &state) {
    nnet::Array a(100, 100);

    while (state.KeepRunning()) {
        a[10][10];
    }
}

BENCHMARK(BM_nnet_item);

static void BM_nnet_dot(benchmark::State &state) {
    nnet::Array a(10, 10);

    nnet::Array b(10, 10);

    while (state.KeepRunning()) {
        a.dot(b);
    }
}

BENCHMARK(BM_nnet_dot);//->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
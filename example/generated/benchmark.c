
/* starch generated code. Do not edit. */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>

#include "starch.h"

const unsigned starch_benchmark_warmup_loops = 10;

typedef struct timespec starch_benchmark_time;
inline static void starch_benchmark_get_time(starch_benchmark_time *t)
{
#ifdef CLOCK_THREAD_CPUTIME_ID
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, t);
#else
    clock_gettime(CLOCK_MONOTONIC, t);
#endif
}

static uint64_t starch_benchmark_elapsed(starch_benchmark_time *start, starch_benchmark_time *end)
{
    return ((uint64_t)end->tv_sec - (uint64_t)start->tv_sec) * 1000000000U + (uint64_t)end->tv_nsec - (uint64_t)start->tv_nsec;
}


static void starch_benchmark_one_subtract_n( starch_subtract_n_regentry * _entry, const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 )
{
    fprintf(stderr, "  %-40.40s  ", _entry->name);

    /* test for support */
    if (_entry->flavor_supported && !(_entry->flavor_supported())) {
        fprintf(stderr, "unsupported\n");
        return;
    }

    /* initial warmup */
    for (unsigned _loop = 0; _loop < starch_benchmark_warmup_loops; ++_loop)
        _entry->callable ( arg0, arg1, arg2, arg3 );

    /* pre-benchmark, find a loop count that takes at least 100ms */
    starch_benchmark_time _start, _end;
    uint64_t _elapsed = 0;
    uint64_t _loops = 127;
    while (_elapsed < 100000000) {
        _loops *= 2;
        starch_benchmark_get_time(&_start);
        for (uint64_t _loop = 0; _loop < _loops; ++_loop)
            _entry->callable ( arg0, arg1, arg2, arg3 );
        starch_benchmark_get_time(&_end);
        _elapsed = starch_benchmark_elapsed(&_start, &_end);
    }

    /* real benchmark, run for approx 1 second */
    _loops = _loops * 1000000000 / _elapsed;
    starch_benchmark_get_time(&_start);
    for (uint64_t _loop = 0; _loop < _loops; ++_loop)
        _entry->callable ( arg0, arg1, arg2, arg3 );
    starch_benchmark_get_time(&_end);
    _elapsed = starch_benchmark_elapsed(&_start, &_end);

    uint64_t _per_loop = _elapsed / _loops;
    fprintf(stderr, "%" PRIu64 " ns/call\n", _per_loop);
}

static void starch_benchmark_run_subtract_n( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 )
{
    for (starch_subtract_n_regentry *_entry = starch_subtract_n_registry; _entry->name; ++_entry) {
        starch_benchmark_one_subtract_n( _entry, arg0, arg1, arg2, arg3 );
    }
}

/* prototype the benchmarking function so that we can build with -Wmissing-declarations */
void starch_subtract_n_benchmark(void);


#define STARCH_SYMBOL(_name) starch_ ## _name ## _benchmark_sym
#define STARCH_IMPL(_function,_impl) starch_ ## _function ## _ ## _impl ## _dummy_benchmark
#define STARCH_IMPL_REQUIRES(_function,_impl,_feature) STARCH_IMPL(_function,_impl)
#define STARCH_BENCHMARK(_function) starch_ ## _function ## _benchmark
#define STARCH_BENCHMARK_RUN(_function, ...) starch_benchmark_run_ ## _function ( __VA_ARGS__ )

#include "../impl/subtract_n.c"

static void starch_benchmark_all_subtract_n(void)
{
    fprintf(stderr, "==== subtract_n ===\n");
    STARCH_BENCHMARK(subtract_n) ();
}

int main(int argc, char **argv)
{
    const char *selected = (argc > 1 ? argv[1] : NULL);
    if (!selected || !strcmp(selected, "subtract_n")) {
        starch_benchmark_all_subtract_n();
    }
}


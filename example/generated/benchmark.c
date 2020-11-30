
/* starch generated code. Do not edit. */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdalign.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>
#include <errno.h>

#include "starch.h"

const unsigned starch_benchmark_warmup_loops = 10;

typedef struct timespec starch_benchmark_time;
inline void starch_benchmark_get_time(starch_benchmark_time *t)
{
#ifdef CLOCK_THREAD_CPUTIME_ID
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, t);
#else
    clock_gettime(CLOCK_MONOTONIC, t);
#endif
}

uint64_t starch_benchmark_elapsed(starch_benchmark_time *start, starch_benchmark_time *end)
{
    return ((uint64_t)end->tv_sec - (uint64_t)start->tv_sec) * 1000000000U + (uint64_t)end->tv_nsec - (uint64_t)start->tv_nsec;
}

void *starch_benchmark_aligned_alloc(size_t alignment, size_t type_alignment, size_t size)
{
    size_t use_alignment = (type_alignment > alignment ? type_alignment : alignment);
    if (use_alignment < sizeof(void*))
        use_alignment = sizeof(void*);
    if ( (use_alignment % type_alignment) || (use_alignment % alignment) ) {
        fprintf(stderr, "STARCH_BENCHMARK_ALLOC of %zu bytes failed: conflicting alignment requirements (%zu versus %zu)\n", size, alignment, type_alignment);
        return NULL;
    }

    /* Over-allocate so we can stash our own pointer before the start, and so that we can adjust
     * the returned alignment so it is only aligned to the requested boundary, and not also
     * aligned to a larger power of two (we don't want to accidentally benchmark the performance
     * of a more restrictive larger alignment)
     */
    void *block_ptr = aligned_alloc(use_alignment, size + use_alignment * 3);
    if (!block_ptr) {
        fprintf(stderr, "STARCH_BENCHMARK_ALLOC of %zu bytes failed: %s\n", size, strerror(errno));
        return NULL;
    }

    void *user_ptr = (char*)block_ptr + use_alignment;
    if ( (uintptr_t)user_ptr % (use_alignment * 2) == 0 ) {
        // user_ptr is aligned to the next power of two, but we don't want that, move it on
        user_ptr = (char*)block_ptr + use_alignment * 2;
    }

    void **stash = (void**)user_ptr - 1;
    *stash = block_ptr;

    return user_ptr;
}

void starch_benchmark_aligned_free(void *user_ptr)
{
    if (!user_ptr)
        return;
    void **stash = (void**)user_ptr - 1;
    free(*stash);
}


static void starch_benchmark_one_subtract_n( starch_subtract_n_regentry * _entry, const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 )
{
    fprintf(stderr, "  %-40s  ", _entry->name);

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

static void starch_benchmark_one_subtract_n_aligned( starch_subtract_n_aligned_regentry * _entry, const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 )
{
    fprintf(stderr, "  %-40s  ", _entry->name);

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

static void starch_benchmark_run_subtract_n_aligned( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 )
{
    for (starch_subtract_n_aligned_regentry *_entry = starch_subtract_n_aligned_registry; _entry->name; ++_entry) {
        starch_benchmark_one_subtract_n_aligned( _entry, arg0, arg1, arg2, arg3 );
    }
}

/* prototype the benchmarking function so that we can build with -Wmissing-declarations */
void starch_subtract_n_aligned_benchmark(void);


#undef STARCH_ALIGNMENT

#define STARCH_ALIGNMENT 1
#define STARCH_ALIGNED(_ptr) (_ptr)
#define STARCH_SYMBOL(_name) starch_ ## _name ## _benchmark_sym
#define STARCH_IMPL(_function,_impl) starch_ ## _function ## _ ## _impl ## _dummy_benchmark
#define STARCH_IMPL_REQUIRES(_function,_impl,_feature) STARCH_IMPL(_function,_impl)
#define STARCH_BENCHMARK(_function) starch_ ## _function ## _benchmark
#define STARCH_BENCHMARK_RUN(_function, ...) starch_benchmark_run_ ## _function ( __VA_ARGS__ )
#define STARCH_BENCHMARK_ALLOC(_count, _type) starch_benchmark_aligned_alloc(1, alignof(_type), (_count) * sizeof(_type))
#define STARCH_BENCHMARK_FREE(_ptr) starch_benchmark_aligned_free(_ptr)

#include "../benchmark/subtract_n_benchmark.c"

#undef STARCH_ALIGNMENT
#undef STARCH_ALIGNED
#undef STARCH_SYMBOL
#undef STARCH_IMPL
#undef STARCH_IMPL_REQUIRES
#undef STARCH_BENCHMARK
#undef STARCH_BENCHMARK_RUN
#undef STARCH_BENCHMARK_ALLOC
#undef STARCH_BENCHMARK_FREE

#define STARCH_ALIGNMENT STARCH_MIX_ALIGNMENT
#define STARCH_ALIGNED(_ptr) (__builtin_assume_aligned((_ptr), STARCH_MIX_ALIGNMENT))
#define STARCH_SYMBOL(_name) starch_ ## _name ## _aligned_benchmark_sym
#define STARCH_IMPL(_function,_impl) starch_ ## _function ## _aligned_ ## _impl ## _dummy_benchmark
#define STARCH_IMPL_REQUIRES(_function,_impl,_feature) STARCH_IMPL(_function,_impl)
#define STARCH_BENCHMARK(_function) starch_ ## _function ## _aligned_benchmark
#define STARCH_BENCHMARK_RUN(_function, ...) starch_benchmark_run_ ## _function ## _aligned ( __VA_ARGS__ )
#define STARCH_BENCHMARK_ALLOC(_count, _type) starch_benchmark_aligned_alloc(STARCH_MIX_ALIGNMENT, alignof(_type), (_count) * sizeof(_type))
#define STARCH_BENCHMARK_FREE(_ptr) starch_benchmark_aligned_free(_ptr)

#include "../benchmark/subtract_n_benchmark.c"

static void starch_benchmark_all_subtract_n(void)
{
    fprintf(stderr, "==== subtract_n ===\n");
    starch_subtract_n_benchmark ();
}
static void starch_benchmark_all_subtract_n_aligned(void)
{
    fprintf(stderr, "==== subtract_n_aligned ===\n");
    starch_subtract_n_aligned_benchmark ();
}

int main(int argc, char **argv)
{
    const char *selected = (argc > 1 ? argv[1] : NULL);
    if (!selected || !strcmp(selected, "subtract_n")) {
        starch_benchmark_all_subtract_n();
    }
    if (!selected || !strcmp(selected, "subtract_n_aligned")) {
        starch_benchmark_all_subtract_n_aligned();
    }
}


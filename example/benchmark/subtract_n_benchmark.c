void STARCH_BENCHMARK(subtract_n) (void)
{
    uint16_t *in = NULL, *out = NULL;
    const unsigned len = 65536;
    const unsigned n = 42;

    if (!(in = STARCH_BENCHMARK_ALLOC(len, uint16_t)) || !(out = STARCH_BENCHMARK_ALLOC(len, uint16_t))) {
        goto done;
    }

    STARCH_BENCHMARK_RUN( subtract_n, in, len, n, out );

 done:
    STARCH_BENCHMARK_FREE(in);
    STARCH_BENCHMARK_FREE(out);
}

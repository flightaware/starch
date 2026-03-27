#include <stdio.h>

void STARCH_BENCHMARK(subtract_n) (void)
{
    uint16_t *in = NULL, *out = NULL;
    const unsigned len = 65536;
    const unsigned n = 42;

    if (!(in = calloc(len, sizeof(uint16_t))) || !(out = calloc(len, sizeof(uint16_t)))) {
        goto done;
    }

    for (unsigned i = 0; i < len; ++i)
        in[i] = rand() % 65535;

    STARCH_BENCHMARK_RUN( subtract_n, in, len, n, out );

 done:
    free(in);
    free(out);
}

bool STARCH_BENCHMARK_VERIFY(subtract_n)(const uint16_t *in, unsigned len, uint16_t n, uint16_t *out)
{
    bool okay = true;

    for (unsigned i = 0; i < len; ++i) {
        uint16_t expected = in[i] - n;
        if (out[i] != expected) {
            fprintf(stderr, "verification failed: in[%u]=%u n=%u out[%u]=%u expected=%u\n", i, in[i], n, i, out[i], expected);
            okay = false;
        }
    }

    return okay;
}

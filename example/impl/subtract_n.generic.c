void STARCH_IMPL(subtract_n, generic) (const uint16_t * restrict in, unsigned len, uint16_t n, uint16_t * restrict out)
{
    while (len--) {
        out[0] = in[0] - n;
        in++;
        out++;
    }
}

__attribute__((optimize(0)))
void STARCH_IMPL(subtract_n, no_opt) (const uint16_t * restrict in, unsigned len, uint16_t n, uint16_t * restrict out)
{
    while (len--) {
        out[0] = in[0] - n;
        in++;
        out++;
    }
}

__attribute__((optimize("no-tree-vectorize")))
void STARCH_IMPL(subtract_n, no_tree_vec) (const uint16_t * restrict in, unsigned len, uint16_t n, uint16_t * restrict out)
{
    while (len--) {
        out[0] = in[0] - n;
        in++;
        out++;
    }
}

void STARCH_IMPL(subtract_n, unroll_4) (const uint16_t * restrict in, unsigned len, uint16_t n, uint16_t * restrict out)
{
    unsigned len4 = len >> 2;
    unsigned len1 = len & 3;

    while (len4--) {
        out[0] = in[0] - n;
        out[1] = in[1] - n;
        out[2] = in[2] - n;
        out[3] = in[3] - n;
        in += 4;
        out += 4;
    }

    while (len1--) {
        out[0] = in[0] - n;
        in++;
        out++;
    }
}

void STARCH_IMPL(subtract_n, bad_implementation) (const uint16_t * restrict in, unsigned len, uint16_t n, uint16_t * restrict out)
{
    // This is a deliberately bad implementation that produces
    // incorrect results. The error should be caught during
    // benchmarking via STARCH_BENCHMARK_VERIFY.

    while (len--) {
        out[0] = in[0] - n;
        if (len % 10000 == 0)
            out[0] += 1;
        in++;
        out++;
    }
}

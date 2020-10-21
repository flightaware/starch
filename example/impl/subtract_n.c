void STARCH_IMPL(subtract_n, generic) (const uint16_t *in, unsigned len, uint16_t n, uint16_t *out)
{
    const uint16_t * restrict in_align = __builtin_assume_aligned(in, 16);
    uint16_t * restrict out_align = __builtin_assume_aligned(out, 16);

    while (len--) {
        out_align[0] = in_align[0] - n;
        in_align++;
        out_align++;
    }
}

void STARCH_IMPL(subtract_n, unroll_4) (const uint16_t *in, unsigned len, uint16_t n, uint16_t *out)
{
    const uint16_t * restrict in_align = __builtin_assume_aligned(in, 16);
    uint16_t * restrict out_align = __builtin_assume_aligned(out, 16);

    unsigned len4 = len >> 2;
    unsigned len1 = len & 3;

    while (len4--) {
        out_align[0] = in_align[0] - n;
        out_align[1] = in_align[1] - n;
        out_align[2] = in_align[2] - n;
        out_align[3] = in_align[3] - n;
        in_align += 4;
        out_align += 4;
    }

    while (len1--) {
        out_align[0] = in_align[0] - n;
        in_align++;
        out_align++;
    }
}

#ifdef STARCH_FEATURE_NEON

#include <arm_neon.h>

void STARCH_IMPL_REQUIRES(subtract_n, neon_intrinsics, STARCH_FEATURE_NEON) (const uint16_t *in, unsigned len, uint16_t n, uint16_t *out)
{
    const uint16_t * restrict in_align = __builtin_assume_aligned(in, 16);
    uint16_t * restrict out_align = __builtin_assume_aligned(out, 16);

    uint16x8_t subtractor = vdupq_n_u16(n);

    unsigned len8 = len >> 3;
    unsigned len4 = len & 4;
    unsigned len1 = len & 3;

    while (len8--) {
        uint16x8_t value = vld1q_u16(in_align);
        uint16x8_t result = vsubq_u16(value, subtractor);
        vst1q_u16(out_align, result);
        in_align += 8;
        out_align += 8;
    }

    if (len4) {
        uint16x4_t value = vld1_u16(in_align);
        uint16x4_t result = vsub_u16(value, vget_low_u16(subtractor));
        vst1_u16(out_align, result);
        in_align += 4;
        out_align += 4;
    }
    
    while (len1--) {
        uint16x4_t value = vld1_dup_u16(in_align);
        uint16x4_t result = vsub_u16(value, vget_low_u16(subtractor));
        vst1_lane_u16(out_align, result, 0);
        in_align++;
        out_align++;
    }
}

#endif


#ifdef STARCH_BENCHMARK

#include <stdlib.h>

uint64_t STARCH_BENCHMARK(subtract_n) (void)
{
    uint16_t *in = NULL, *out = NULL;
    const unsigned len = 65536;
    const unsigned n = 42;
    uint64_t timing = 0;

    if (!(in = aligned_alloc(16, len * sizeof(uint16_t))) || !(out = aligned_alloc(16, len * sizeof(uint16_t)))) {
        perror("aligned_alloc");
        goto done;
    }

    STARCH_BENCHMARK_LOOP( subtract_n, (in, len, n, out), timing );

 done:
    free(in);
    free(out);
    return timing;
}

#endif

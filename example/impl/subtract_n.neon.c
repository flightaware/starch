#ifndef __ARM_NEON
#  error Neon support not available
#endif

#include <arm_neon.h>

void STARCH_IMPL(subtract_n, neon_intrinsics) (const uint16_t * restrict in, unsigned len, uint16_t n, uint16_t * restrict out)
{
    uint16x8_t subtractor = vdupq_n_u16(n);

    unsigned len8 = len >> 3;
    unsigned len4 = len & 4;
    unsigned len1 = len & 3;

    while (len8--) {
        uint16x8_t value = vld1q_u16(in);
        uint16x8_t result = vsubq_u16(value, subtractor);
        vst1q_u16(out, result);
        in += 8;
        out += 8;
    }

    if (len4) {
        uint16x4_t value = vld1_u16(in);
        uint16x4_t result = vsub_u16(value, vget_low_u16(subtractor));
        vst1_u16(out, result);
        in += 4;
        out += 4;
    }

    while (len1--) {
        uint16x4_t value = vld1_dup_u16(in);
        uint16x4_t result = vsub_u16(value, vget_low_u16(subtractor));
        vst1_lane_u16(out, result, 0);
        in++;
        out++;
    }
}

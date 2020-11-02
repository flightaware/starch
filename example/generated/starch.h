
/* starch generated code. Do not edit. */

#include <stdint.h>

/* mixes */

/* Generic build, compiler defaults only */
#ifdef STARCH_MIX_GENERIC
#define STARCH_FLAVOR_GENERIC
#endif /* STARCH_MIX_GENERIC */

/* ARM */
#ifdef STARCH_MIX_ARM
#define STARCH_FLAVOR_ARMV7A_VFPV4
#define STARCH_FLAVOR_ARMV7A_VFPV3
#define STARCH_FLAVOR_GENERIC
#endif /* STARCH_MIX_ARM */

/* x64-64 */
#ifdef STARCH_MIX_X86_64
#define STARCH_FLAVOR_X86_64_AVX2
#define STARCH_FLAVOR_X86_64_AVX
#define STARCH_FLAVOR_GENERIC
#endif /* STARCH_MIX_X86_64 */


/* entry points and registries */

typedef void (* starch_subtract_n_ptr) ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
extern starch_subtract_n_ptr starch_subtract_n;

typedef struct {
    int rank;
    const char *name;
    starch_subtract_n_ptr callable;
    int (*flavor_supported)();
} starch_subtract_n_regentry;

extern starch_subtract_n_regentry starch_subtract_n_registry[];
starch_subtract_n_regentry * starch_subtract_n_select();
void starch_subtract_n_set_wisdom( const char * const * received_wisdom );

/* flavors and prototypes */

#ifdef STARCH_FLAVOR_GENERIC
void starch_subtract_n_generic_generic ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
void starch_subtract_n_unroll_4_generic ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
#endif /* STARCH_FLAVOR_GENERIC */

#ifdef STARCH_FLAVOR_ARMV7A_VFPV3
int supports_neon_vfpv3 (void);
#define STARCH_FEATURE_NEON
void starch_subtract_n_generic_armv7a_vfpv3 ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
void starch_subtract_n_unroll_4_armv7a_vfpv3 ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
void starch_subtract_n_neon_intrinsics_armv7a_vfpv3 ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
#endif /* STARCH_FLAVOR_ARMV7A_VFPV3 */

#ifdef STARCH_FLAVOR_ARMV7A_VFPV4
int supports_neon_vfpv4 (void);
#define STARCH_FEATURE_NEON
void starch_subtract_n_generic_armv7a_vfpv4 ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
void starch_subtract_n_unroll_4_armv7a_vfpv4 ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
void starch_subtract_n_neon_intrinsics_armv7a_vfpv4 ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
#endif /* STARCH_FLAVOR_ARMV7A_VFPV4 */

#ifdef STARCH_FLAVOR_X86_64_AVX
int supports_x86_avx (void);
void starch_subtract_n_generic_x86_64_avx ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
void starch_subtract_n_unroll_4_x86_64_avx ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
#endif /* STARCH_FLAVOR_X86_64_AVX */

#ifdef STARCH_FLAVOR_X86_64_AVX2
int supports_x86_avx2 (void);
void starch_subtract_n_generic_x86_64_avx2 ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
void starch_subtract_n_unroll_4_x86_64_avx2 ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
#endif /* STARCH_FLAVOR_X86_64_AVX2 */


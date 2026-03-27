
/* starch generated code. Do not edit. */

#include <stdint.h>

/* mixes */

/* ARM, 64-bit */
#ifdef STARCH_MIX_AARCH64
#ifdef STARCH_MIX
#  error Multiple starch mixes enabled, only one should be enabled
#endif
#define STARCH_MIX_NAME "aarch64"
#define STARCH_FLAVOR_ARMV8A_ASIMD
#define STARCH_FLAVOR_GENERIC
#endif /* STARCH_MIX_AARCH64 */

/* ARM, 32-bit */
#ifdef STARCH_MIX_ARM
#ifdef STARCH_MIX
#  error Multiple starch mixes enabled, only one should be enabled
#endif
#define STARCH_MIX_NAME "arm"
#define STARCH_FLAVOR_ARMV7A_ASIMD
#define STARCH_FLAVOR_GENERIC
#endif /* STARCH_MIX_ARM */

/* Generic build, compiler defaults only */
#ifdef STARCH_MIX_GENERIC
#ifdef STARCH_MIX
#  error Multiple starch mixes enabled, only one should be enabled
#endif
#define STARCH_MIX_NAME "generic"
#define STARCH_FLAVOR_GENERIC
#endif /* STARCH_MIX_GENERIC */

/* x64-64 */
#ifdef STARCH_MIX_X86_64
#ifdef STARCH_MIX
#  error Multiple starch mixes enabled, only one should be enabled
#endif
#define STARCH_MIX_NAME "x86_64"
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
    const char *flavor;
    starch_subtract_n_ptr callable;
    int (*flavor_supported)();
} starch_subtract_n_regentry;

extern starch_subtract_n_regentry starch_subtract_n_registry[];
starch_subtract_n_regentry * starch_subtract_n_select();
void starch_subtract_n_set_wisdom( const char * const * received_wisdom );


/* flavors and prototypes */

#ifdef STARCH_FLAVOR_ARMV7A_ASIMD
int supports_asimd (void);
void starch_subtract_n_generic_armv7a_asimd ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
void starch_subtract_n_no_opt_armv7a_asimd ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
void starch_subtract_n_no_tree_vec_armv7a_asimd ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
void starch_subtract_n_unroll_4_armv7a_asimd ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
void starch_subtract_n_bad_implementation_armv7a_asimd ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
void starch_subtract_n_neon_intrinsics_armv7a_asimd ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
#endif /* STARCH_FLAVOR_ARMV7A_ASIMD */

int starch_read_wisdom (const char * path);

#ifdef STARCH_FLAVOR_ARMV8A_ASIMD
int supports_asimd (void);
void starch_subtract_n_generic_armv8a_asimd ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
void starch_subtract_n_no_opt_armv8a_asimd ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
void starch_subtract_n_no_tree_vec_armv8a_asimd ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
void starch_subtract_n_unroll_4_armv8a_asimd ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
void starch_subtract_n_bad_implementation_armv8a_asimd ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
void starch_subtract_n_neon_intrinsics_armv8a_asimd ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
#endif /* STARCH_FLAVOR_ARMV8A_ASIMD */

int starch_read_wisdom (const char * path);

#ifdef STARCH_FLAVOR_GENERIC
void starch_subtract_n_generic_generic ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
void starch_subtract_n_no_opt_generic ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
void starch_subtract_n_no_tree_vec_generic ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
void starch_subtract_n_unroll_4_generic ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
void starch_subtract_n_bad_implementation_generic ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
#endif /* STARCH_FLAVOR_GENERIC */

int starch_read_wisdom (const char * path);

#ifdef STARCH_FLAVOR_X86_64_AVX
int supports_x86_avx (void);
void starch_subtract_n_generic_x86_64_avx ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
void starch_subtract_n_no_opt_x86_64_avx ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
void starch_subtract_n_no_tree_vec_x86_64_avx ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
void starch_subtract_n_unroll_4_x86_64_avx ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
void starch_subtract_n_bad_implementation_x86_64_avx ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
#endif /* STARCH_FLAVOR_X86_64_AVX */

int starch_read_wisdom (const char * path);

#ifdef STARCH_FLAVOR_X86_64_AVX2
int supports_x86_avx2 (void);
void starch_subtract_n_generic_x86_64_avx2 ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
void starch_subtract_n_no_opt_x86_64_avx2 ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
void starch_subtract_n_no_tree_vec_x86_64_avx2 ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
void starch_subtract_n_unroll_4_x86_64_avx2 ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
void starch_subtract_n_bad_implementation_x86_64_avx2 ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 );
#endif /* STARCH_FLAVOR_X86_64_AVX2 */

int starch_read_wisdom (const char * path);


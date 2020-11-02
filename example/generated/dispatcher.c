
/* starch generated code. Do not edit. */

#include <stdlib.h>
#include <string.h>

#include "starch.h"

/* helper for re-sorting registries */
struct starch_regentry_prefix {
    int rank;
};

static int starch_regentry_rank_compare (const void *l, const void *r)
{
    const struct starch_regentry_prefix *left = l, *right = r;
    return left->rank - right->rank;
}

/* dispatcher / registry for subtract_n */

starch_subtract_n_regentry * starch_subtract_n_select() {
    for (starch_subtract_n_regentry *entry = starch_subtract_n_registry;
         entry->name;
         ++entry)
    {
        if (entry->flavor_supported && !(entry->flavor_supported()))
            continue;
        return entry;
    }
    return NULL;
}

static void starch_subtract_n_dispatch ( const uint16_t * arg0, unsigned arg1, uint16_t arg2, uint16_t * arg3 ) {
    starch_subtract_n_regentry *entry = starch_subtract_n_select();
    if (!entry)
        abort();

    starch_subtract_n = entry->callable;
    starch_subtract_n ( arg0, arg1, arg2, arg3 );
}

starch_subtract_n_ptr starch_subtract_n = starch_subtract_n_dispatch;

void starch_subtract_n_set_wisdom (const char * const * received_wisdom)
{
    /* re-rank the registry based on received wisdom */
    starch_subtract_n_regentry *entry;
    for (entry = starch_subtract_n_registry; entry->name; ++entry) {
        const char * const *search;
        for (search = received_wisdom; *search; ++search) {
            if (!strcmp(*search, entry->name)) {
                break;
            }
        }
        if (*search) {
            /* matches an entry in the wisdom list, order by position in the list */
            entry->rank = search - received_wisdom;
        } else {
            /* no match, rank after all possible matches, retaining existing order */
            entry->rank = (search - received_wisdom) + (entry - starch_subtract_n_registry);
        }
    }

    /* re-sort based on the new ranking */
    qsort(starch_subtract_n_registry, entry - starch_subtract_n_registry, sizeof(starch_subtract_n_regentry), starch_regentry_rank_compare);

    /* reset the implementation pointer so the next call will re-select */
    starch_subtract_n = starch_subtract_n_dispatch;
}

starch_subtract_n_regentry starch_subtract_n_registry[] = {
  
#ifdef STARCH_MIX_GENERIC
    { 0, "generic_generic", starch_subtract_n_generic_generic, NULL },
    { 1, "unroll_4_generic", starch_subtract_n_unroll_4_generic, NULL },
#endif /* STARCH_MIX_GENERIC */
  
#ifdef STARCH_MIX_ARM
    { 0, "neon_intrinsics_armv7a_vfpv4", starch_subtract_n_neon_intrinsics_armv7a_vfpv4, supports_neon_vfpv4 },
    { 1, "neon_intrinsics_armv7a_vfpv3", starch_subtract_n_neon_intrinsics_armv7a_vfpv3, supports_neon_vfpv3 },
    { 2, "generic_generic", starch_subtract_n_generic_generic, NULL },
    { 3, "generic_armv7a_vfpv4", starch_subtract_n_generic_armv7a_vfpv4, supports_neon_vfpv4 },
    { 4, "unroll_4_armv7a_vfpv4", starch_subtract_n_unroll_4_armv7a_vfpv4, supports_neon_vfpv4 },
    { 5, "generic_armv7a_vfpv3", starch_subtract_n_generic_armv7a_vfpv3, supports_neon_vfpv3 },
    { 6, "unroll_4_armv7a_vfpv3", starch_subtract_n_unroll_4_armv7a_vfpv3, supports_neon_vfpv3 },
    { 7, "unroll_4_generic", starch_subtract_n_unroll_4_generic, NULL },
#endif /* STARCH_MIX_ARM */
  
#ifdef STARCH_MIX_X86_64
    { 0, "generic_x86_64_avx2", starch_subtract_n_generic_x86_64_avx2, supports_x86_avx2 },
    { 1, "generic_x86_64_avx", starch_subtract_n_generic_x86_64_avx, supports_x86_avx },
    { 2, "unroll_4_generic", starch_subtract_n_unroll_4_generic, NULL },
    { 3, "unroll_4_x86_64_avx2", starch_subtract_n_unroll_4_x86_64_avx2, supports_x86_avx2 },
    { 4, "unroll_4_x86_64_avx", starch_subtract_n_unroll_4_x86_64_avx, supports_x86_avx },
    { 5, "generic_generic", starch_subtract_n_generic_generic, NULL },
#endif /* STARCH_MIX_X86_64 */
    { 0, NULL, NULL, NULL }
};


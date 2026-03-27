
/* starch generated code. Do not edit. */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
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
  
#ifdef STARCH_MIX_AARCH64
    { 0, "generic_armv8a_asimd", "armv8a_asimd", starch_subtract_n_generic_armv8a_asimd, supports_asimd },
    { 1, "no_opt_armv8a_asimd", "armv8a_asimd", starch_subtract_n_no_opt_armv8a_asimd, supports_asimd },
    { 2, "no_tree_vec_armv8a_asimd", "armv8a_asimd", starch_subtract_n_no_tree_vec_armv8a_asimd, supports_asimd },
    { 3, "unroll_4_armv8a_asimd", "armv8a_asimd", starch_subtract_n_unroll_4_armv8a_asimd, supports_asimd },
    { 4, "bad_implementation_armv8a_asimd", "armv8a_asimd", starch_subtract_n_bad_implementation_armv8a_asimd, supports_asimd },
    { 5, "neon_intrinsics_armv8a_asimd", "armv8a_asimd", starch_subtract_n_neon_intrinsics_armv8a_asimd, supports_asimd },
    { 6, "generic_generic", "generic", starch_subtract_n_generic_generic, NULL },
    { 7, "no_opt_generic", "generic", starch_subtract_n_no_opt_generic, NULL },
    { 8, "no_tree_vec_generic", "generic", starch_subtract_n_no_tree_vec_generic, NULL },
    { 9, "unroll_4_generic", "generic", starch_subtract_n_unroll_4_generic, NULL },
    { 10, "bad_implementation_generic", "generic", starch_subtract_n_bad_implementation_generic, NULL },
#endif /* STARCH_MIX_AARCH64 */
  
#ifdef STARCH_MIX_ARM
    { 0, "generic_armv7a_asimd", "armv7a_asimd", starch_subtract_n_generic_armv7a_asimd, supports_asimd },
    { 1, "no_opt_armv7a_asimd", "armv7a_asimd", starch_subtract_n_no_opt_armv7a_asimd, supports_asimd },
    { 2, "no_tree_vec_armv7a_asimd", "armv7a_asimd", starch_subtract_n_no_tree_vec_armv7a_asimd, supports_asimd },
    { 3, "unroll_4_armv7a_asimd", "armv7a_asimd", starch_subtract_n_unroll_4_armv7a_asimd, supports_asimd },
    { 4, "bad_implementation_armv7a_asimd", "armv7a_asimd", starch_subtract_n_bad_implementation_armv7a_asimd, supports_asimd },
    { 5, "neon_intrinsics_armv7a_asimd", "armv7a_asimd", starch_subtract_n_neon_intrinsics_armv7a_asimd, supports_asimd },
    { 6, "generic_generic", "generic", starch_subtract_n_generic_generic, NULL },
    { 7, "no_opt_generic", "generic", starch_subtract_n_no_opt_generic, NULL },
    { 8, "no_tree_vec_generic", "generic", starch_subtract_n_no_tree_vec_generic, NULL },
    { 9, "unroll_4_generic", "generic", starch_subtract_n_unroll_4_generic, NULL },
    { 10, "bad_implementation_generic", "generic", starch_subtract_n_bad_implementation_generic, NULL },
#endif /* STARCH_MIX_ARM */
  
#ifdef STARCH_MIX_GENERIC
    { 0, "generic_generic", "generic", starch_subtract_n_generic_generic, NULL },
    { 1, "no_opt_generic", "generic", starch_subtract_n_no_opt_generic, NULL },
    { 2, "no_tree_vec_generic", "generic", starch_subtract_n_no_tree_vec_generic, NULL },
    { 3, "unroll_4_generic", "generic", starch_subtract_n_unroll_4_generic, NULL },
    { 4, "bad_implementation_generic", "generic", starch_subtract_n_bad_implementation_generic, NULL },
#endif /* STARCH_MIX_GENERIC */
  
#ifdef STARCH_MIX_X86_64
    { 0, "generic_x86_64_avx2", "x86_64_avx2", starch_subtract_n_generic_x86_64_avx2, supports_x86_avx2 },
    { 1, "no_opt_x86_64_avx2", "x86_64_avx2", starch_subtract_n_no_opt_x86_64_avx2, supports_x86_avx2 },
    { 2, "no_tree_vec_x86_64_avx2", "x86_64_avx2", starch_subtract_n_no_tree_vec_x86_64_avx2, supports_x86_avx2 },
    { 3, "unroll_4_x86_64_avx2", "x86_64_avx2", starch_subtract_n_unroll_4_x86_64_avx2, supports_x86_avx2 },
    { 4, "bad_implementation_x86_64_avx2", "x86_64_avx2", starch_subtract_n_bad_implementation_x86_64_avx2, supports_x86_avx2 },
    { 5, "generic_x86_64_avx", "x86_64_avx", starch_subtract_n_generic_x86_64_avx, supports_x86_avx },
    { 6, "no_opt_x86_64_avx", "x86_64_avx", starch_subtract_n_no_opt_x86_64_avx, supports_x86_avx },
    { 7, "no_tree_vec_x86_64_avx", "x86_64_avx", starch_subtract_n_no_tree_vec_x86_64_avx, supports_x86_avx },
    { 8, "unroll_4_x86_64_avx", "x86_64_avx", starch_subtract_n_unroll_4_x86_64_avx, supports_x86_avx },
    { 9, "bad_implementation_x86_64_avx", "x86_64_avx", starch_subtract_n_bad_implementation_x86_64_avx, supports_x86_avx },
    { 10, "generic_generic", "generic", starch_subtract_n_generic_generic, NULL },
    { 11, "no_opt_generic", "generic", starch_subtract_n_no_opt_generic, NULL },
    { 12, "no_tree_vec_generic", "generic", starch_subtract_n_no_tree_vec_generic, NULL },
    { 13, "unroll_4_generic", "generic", starch_subtract_n_unroll_4_generic, NULL },
    { 14, "bad_implementation_generic", "generic", starch_subtract_n_bad_implementation_generic, NULL },
#endif /* STARCH_MIX_X86_64 */
    { 0, NULL, NULL, NULL, NULL }
};


int starch_read_wisdom (const char * path)
{
    FILE *fp = fopen(path, "r");
    if (!fp)
        return -1;

    /* reset all ranks to identify entries not listed in the wisdom file; we'll assign ranks at the end to produce a stable sort */
    int rank_subtract_n = 0;
    for (starch_subtract_n_regentry *entry = starch_subtract_n_registry; entry->name; ++entry) {
        entry->rank = 0;
    }

    char linebuf[512];
    while (fgets(linebuf, sizeof(linebuf), fp)) {
        /* split name and impl on whitespace, handle comments etc */
        char *name = linebuf;
        while (*name && isspace(*name))
            ++name;

        if (!*name || *name == '#')
            continue;

        char *end = name;
        while (*end && !isspace(*end))
            ++end;

        if (!*end)
            continue;
        *end = 0;

        char *impl = end + 1;
        while (*impl && isspace(*impl))
            ++impl;

        if (!*impl)
           continue;

        end = impl;
        while (*end && !isspace(*end))
            ++end;

        *end = 0;

        /* try to find a matching registry entry */
        if (!strcmp(name, "subtract_n")) {
            for (starch_subtract_n_regentry *entry = starch_subtract_n_registry; entry->name; ++entry) {
                if (!strcmp(impl, entry->name)) {
                    entry->rank = ++rank_subtract_n;
                    break;
                }
            }
            continue;
        }
    }

    if (ferror(fp)) {
        fclose(fp);
        return -1;
    }

    fclose(fp);

    /* assign ranks to unmatched items to (stable) sort them last; re-sort everything */
    {
        starch_subtract_n_regentry *entry;
        for (entry = starch_subtract_n_registry; entry->name; ++entry) {
            if (!entry->rank)
                entry->rank = ++rank_subtract_n;
        }
        qsort(starch_subtract_n_registry, entry - starch_subtract_n_registry, sizeof(starch_subtract_n_regentry), starch_regentry_rank_compare);

        /* reset the implementation pointer so the next call will re-select */
        starch_subtract_n = starch_subtract_n_dispatch;
    }

    return 0;
}

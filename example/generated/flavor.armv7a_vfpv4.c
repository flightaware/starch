
/* starch generated code. Do not edit. */

#define STARCH_FLAVOR_ARMV7A_VFPV4
#define STARCH_FEATURE_NEON

#define STARCH_SYMBOL(_name) starch_ ## _name ## _ ## armv7a_vfpv4
#define STARCH_IMPL(_function,_impl) starch_ ## _function ## _ ## _impl ## _ ## armv7a_vfpv4
#define STARCH_IMPL_REQUIRES(_function,_impl,_feature) STARCH_IMPL(_function,_impl)

#include "starch.h"

#include "../impl/subtract_n.c"


/* starch generated code. Do not edit. */

#define STARCH_FLAVOR_GENERIC

#define STARCH_SYMBOL(_name) starch_ ## _name ## _ ## generic
#define STARCH_IMPL(_function,_impl) starch_ ## _function ## _ ## _impl ## _ ## generic
#define STARCH_IMPL_REQUIRES(_function,_impl,_feature) STARCH_IMPL(_function,_impl)

#include "starch.h"

#include "../impl/subtract_n.c"

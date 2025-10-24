
/* starch generated code. Do not edit. */

#define STARCH_FLAVOR_ARMV7A_ASIMD

#include "starch.h"

#define STARCH_SYMBOL(_name) starch_ ## _name ## _ ## armv7a_asimd
#define STARCH_IMPL(_function,_impl) starch_ ## _function ## _ ## _impl ## _ ## armv7a_asimd

#include "../impl/subtract_n.generic.c"
#include "../impl/subtract_n.neon.c"

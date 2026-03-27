
/* starch generated code. Do not edit. */

#define STARCH_FLAVOR_GENERIC

#include "starch.h"

#define STARCH_SYMBOL(_name) starch_ ## _name ## _ ## generic
#define STARCH_IMPL(_function,_impl) starch_ ## _function ## _ ## _impl ## _ ## generic

#include "../impl/subtract_n.generic.c"

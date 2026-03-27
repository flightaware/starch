# -*- cmake -*-


# starch generated cmake include file.
# This is a generated file, do not edit.

# This include file provides cmake macros for building starch-generated
# code from a specific mix. There is a separate include file for each mix;
# only one is expected to be included.

# Two macros are provided:

# add_starch_library(target ...):
#   create a new library target named "target" that contains the starch
#   generated code, and set appropriate include/compile settings on the
#   target. Additional args (e.g. STATIC) are passed through to
#   add_library()

# add_starch_benchmark(target starchlib ...):
#   add a new target named "target" which builds the benchmarking executable.
#   starchlib should be the corresponding starch library added via
#   "add_starch_library". Additional args are passed through to
#   add_executable()




set_source_files_properties(generated/flavor.x86_64_avx2.c PROPERTIES COMPILE_OPTIONS "-mavx2;-ffast-math")

set_source_files_properties(generated/flavor.x86_64_avx.c PROPERTIES COMPILE_OPTIONS "-mavx;-ffast-math")

set_source_files_properties(generated/flavor.generic.c PROPERTIES COMPILE_OPTIONS "")

macro(add_starch_library target)
  add_library(${target} ${ARGN} generated/flavor.x86_64_avx2.c generated/flavor.x86_64_avx.c generated/flavor.generic.c generated/dispatcher.c)
  target_include_directories(${target} PUBLIC "generated")
  target_compile_definitions(${target} PUBLIC "STARCH_MIX_X86_64")
endmacro()

macro(add_starch_benchmark target starchlib)
  add_executable(${target} ${ARGN} generated/benchmark.c)
  target_link_libraries(${target} ${starchlib})
endmacro()

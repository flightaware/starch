#!/usr/bin/env python3

import sys
import os
import glob

example_dir = os.path.dirname(sys.argv[0])
starch_dir = os.path.join(example_dir, '..')
sys.path.append(starch_dir)
import starch

gen = starch.Generator(runtime_dir = example_dir,
                       output_dir = os.path.join(example_dir, 'generated'))

gen.add_include('<stdint.h>')

gen.add_function(name = 'subtract_n', argtypes = ['const uint16_t *', 'unsigned', 'uint16_t', 'uint16_t *'])

gen.add_flavor(name = 'generic',
               description = 'Generic build, default compiler options',
               compile_flags = [])
armv7a_asimd = gen.add_flavor(name = 'armv7a_asimd',
                              description = 'ARMv7-A with Advanced SIMD (Neon)',
                              compile_flags = ['-march=armv7-a+simd'],
                              test_function = 'supports_asimd')
# -march=armv8-a actually includes ASIMD by default, but for the sake of
# illustration here, we assume the default compiler options don't use it and it
# needs an additional flag. Try building with
# e.g. CFLAGS="-O3 -march=armv8-a+nosimd"
armv8a_asimd =  gen.add_flavor(name = 'armv8a_asimd',
                               description = 'ARMv8-A with Advanced SIMD',
                               compile_flags = ['-march=armv8-a+simd'],
                               test_function = 'supports_asimd')
gen.add_flavor(name = 'x86_64_avx',
               description = 'x86-64 with AVX',
               compile_flags = ['-mavx', '-ffast-math'],
               test_function = 'supports_x86_avx')
gen.add_flavor(name = 'x86_64_avx2',
               description = 'x86-64 with AVX2',
               compile_flags = ['-mavx2', '-ffast-math'],
               test_function = 'supports_x86_avx2')

gen.add_mix(name = 'generic',
            description = 'Generic build, compiler defaults only',
            flavors = ['generic'])
gen.add_mix(name = 'arm',
            description = 'ARM, 32-bit',
            flavors = ['armv7a_asimd', 'generic'])
gen.add_mix(name = 'aarch64',
            description = 'ARM, 64-bit',
            flavors = ['armv8a_asimd', 'generic'])
gen.add_mix(name = 'x86_64',
            description = 'x64-64',
            flavors = ['x86_64_avx2', 'x86_64_avx', 'generic'])

gen.scan_impls('impl/*.generic.c')
gen.scan_impls('impl/*.neon.c', flavors=[armv7a_asimd, armv8a_asimd])
gen.scan_benchmarks('impl/*.benchmark.c')

gen.generate()

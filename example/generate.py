#!/usr/bin/env python3

import starch

gen = starch.Generator(base_dir='.',
                       template_dir='../templates')

gen.add_include('<stdint.h>')

gen.add_feature(name='neon',
                description='ARM NEON v2')

gen.add_flavor(name = 'generic',
               description = 'Generic build, default compiler options',
               compile_flags = [])
gen.add_flavor(name = 'armv7a',
               description = 'ARMv7-A with NEON',
               compile_flags = ['-march=armv7-a+neon-vfpv4', '-mfpu=neon-vfpv4', '-ffast-math'],
               features = ['neon'],
               test_function = 'supports_armv7a_neon')
gen.add_flavor(name = 'armv8a',
               description = 'ARMv8-A with NEON',
               compile_flags = ['-march=armv8-a+simd', '-mfpu=neon-fp-armv8', '-ffast-math'],
               features = ['neon'],
               test_function = 'supports_armv8a_neon')
gen.add_flavor(name = 'x86_64_avx',
               description = 'x86-64 with AVX',
               compile_flags = ['-mavx', '-ffast-math'],
               features = [],
               test_function = 'supports_x86_avx')
gen.add_flavor(name = 'x86_64_avx2',
               description = 'x86-64 with AVX2',
               compile_flags = ['-mavx2', '-ffast-math'],
               features = [],
               test_function = 'supports_x86_avx2')

gen.add_mix(name = 'generic',
            description = 'Generic build, no specific architecture',
            flavors = ['generic'])

gen.add_mix(name = 'arm',
            description = 'ARM 32-bit build',
            flavors = ['armv8a', 'armv7a', 'generic'])

gen.add_mix(name = 'x86_64',
            description = 'x64-64 build',
            flavors = ['x86_64_avx2', 'x86_64_avx', 'generic'])

gen.add_function(name = 'subtract_n', argtypes = ['const uint16_t *', 'unsigned', 'uint16_t', 'uint16_t *'])

gen.scan_file('impl/subtract_n.c')
gen.generate()

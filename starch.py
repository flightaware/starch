# starch: framework glue for selecting ISA-specific code at runtime

# Copyright (c) 2020, FlightAware LLC.
# All rights reserved.
# See the LICENSE file for licensing terms.

import sys
import re
import os
import glob
from functools import total_ordering
from typing import Optional, Union, Iterable, Sequence, MutableSequence, Mapping, MutableMapping, FrozenSet, Dict, List

import mako.lookup
import mako.exceptions

@total_ordering
class BuildFlavor(object):
    """BuildFlavor models code built with specific compiler flags.
Shared implementation code will be built multiple times, once per flavor.

Each flavor has an associated test function that is called at runtime to
check if the current hardware supports the code emitted by the flavor. If
the test function returns false, no code built with the flavor will be executed.
"""

    gen: 'Generator'
    name: str
    description: str
    compile_flags: Sequence[str]
    test_function: Optional[str]
    impls: Sequence['FunctionImpl']

    def __init__(self,
                 gen: 'Generator',
                 name: str,
                 description: str,
                 compile_flags: Iterable[str] = (),
                 test_function: Optional[str] = None):

        self.gen = gen
        self.name = name
        self.compile_flags = tuple(compile_flags)
        self.test_function = test_function
        self.impls = []

    @property
    def macro(self) -> str:
        return 'STARCH_FLAVOR_' + self.name.upper()

    @property
    def test_function_expr(self) -> str:
        if self.test_function is None:
            return "NULL"
        else:
            return self.test_function

    @property
    def cflags(self) -> str:
        return ' '.join(self.compile_flags)

    def __lt__(self, other: object) -> bool:
        if not isinstance(other, BuildFlavor):
            return NotImplemented
        return self.name < other.name


@total_ordering
class Function(object):
    """A user-callable function that will be dispatched to
one of the many possible implementations based on runtime feature
support."""

    gen: 'Generator'
    name: str
    returntype: str
    argtypes: Sequence[str]
    argnames: Sequence[str]
    impls: Sequence['FunctionImpl']
    benchmark: Optional['SourceFile'] = None
    benchmark_verify: Optional['SourceFile'] = None

    def __init__(self,
                 gen: 'Generator',
                 name: str,
                 argtypes: Iterable[str],
                 returntype: str = 'void',
                 argnames: Optional[Iterable[str]] = None):

        self.gen = gen
        self.name = name
        self.returntype = returntype
        self.argtypes = tuple(argtypes)
        self.impls = []

        if argnames is None:
            self.argnames = tuple( f'arg{n}' for n in range(len(self.argtypes)) )
        else:
            self.argnames = tuple(argnames)
            if len(self.argnames) != len(self.argtypes):
                raise ValueError('length of argnames must match length of argtypes')

    @property
    def declaration_arglist(self) -> str:
        return ', '.join([f'{typename} {argname}' for typename, argname in zip(self.argtypes, self.argnames)])

    @property
    def named_arglist(self) -> str:
        return ', '.join(self.argnames)

    @property
    def callable_symbol(self) -> str:
        if self.gen.prefix_function_symbols:
            return self.gen.sym(self.name)
        else:
            return self.name

    @property
    def select_symbol(self) -> str:
        return self.gen.sym(self.name + '_select')

    @property
    def dispatcher_symbol(self) -> str:
        return self.gen.sym(self.name + '_dispatch')

    @property
    def pointer_type(self) -> str:
        return self.gen.sym(self.name + '_ptr')

    @property
    def regentry_type(self) -> str:
        return self.gen.sym(self.name + '_regentry')

    @property
    def registry_symbol(self) -> str:
        return self.gen.sym(self.name + '_registry')

    @property
    def set_wisdom_symbol(self) -> str:
        return self.gen.sym(self.name + '_set_wisdom')

    @property
    def benchmark_symbol(self) -> str:
        return self.gen.sym(self.name + '_benchmark')

    @property
    def benchmark_verify_symbol(self) -> str:
        return self.gen.sym(self.name + '_benchmark_verify')

    def __lt__(self, other: object) -> bool:
        if not isinstance(other, Function):
            return NotImplemented
        return self.name < other.name


class FunctionImpl(object):
    """A possible implementation of a function, associated with a particular flavor.
The same source file might be buildable by several different flavors; in that case,
each flavor will have a separate FunctionImpl."""

    gen: 'Generator'
    function: Function
    name: str
    flavor: BuildFlavor
    source: 'SourceFile'
    lineno: int

    def __init__(self,
                 gen: 'Generator',
                 function: Function,
                 name: str,
                 flavor: BuildFlavor,
                 source: 'SourceFile',
                 lineno: int):
        self.gen = gen
        self.function = function
        self.name = name
        self.flavor = flavor
        self.source = source
        self.lineno = lineno

    @property
    def wisdom_name(self) -> str:
        return self.name + '_' + self.flavor.name

    @property
    def impl_symbol(self) -> str:
        return self.gen.sym(self.function.name + '_' + self.name + '_' + self.flavor.name)


@total_ordering
class SourceFile(object):
    """A scanned source file that contains implementation code."""

    path: str
    impls: Sequence[FunctionImpl]

    def __init__(self, path):
        self.path = path
        self.impls = []

    def __lt__(self, other: object) -> bool:
        if not isinstance(other, SourceFile):
            return NotImplemented
        return self.path < other.path


@total_ordering
class BuildMix(object):
    """A combination of build flavors that make up one possible way of building all
the code. The output of a mix is a library that dispatches functions within the
mixed flavors. For example, when building a binary that is intended to run on
generic ARM systems, a mix could be used that includes flavors for ARMv6, ARMv7,
and ARMv8.

The order of flavors within a mix is significant. At runtime, flavors will be tried
in order until a supported flavor is found; so more efficient flavors should be
specified first."""

    name: str
    description: str
    flavors: Sequence[BuildFlavor]
    wisdom: Mapping[Function,Sequence[str]]

    def __init__(self,
                 name: str,
                 description: str,
                 flavors: Iterable[BuildFlavor],
                 wisdom: Mapping[Function,Iterable[str]] = {}):
        self.name = name
        self.description = description
        self.flavors = tuple(flavors)
        self.wisdom = dict( (k,tuple(v)) for k, v in wisdom.items() )

    @property
    def macro(self):
        return 'STARCH_MIX_' + self.name.upper()

    def function_wisdom(self, function) -> Sequence[str]:
        return self.wisdom.get(function, [])

    def __lt__(self, other: object) -> bool:
        if not isinstance(other, BuildMix):
            return NotImplemented
        return self.name < other.name


class Generator(object):
    functions: MutableMapping[str, Function]
    flavors: MutableMapping[str, BuildFlavor]
    function_impls: MutableMapping[str, FunctionImpl]
    benchmark_files: MutableSequence[SourceFile]
    mixes: MutableMapping[str, BuildMix]
    symbol_prefix: str
    templates: mako.lookup.TemplateLookup
    generated_include_path: str
    generated_flavor_pattern: str
    generated_dispatcher_path: str
    generated_benchmark_path: str
    generated_makefile_pattern: str
    generated_cmake_pattern: str
    includes: MutableSequence[str] = []

    def __init__(self,
                 runtime_dir: str,
                 output_dir: str,
                 template_dir: Optional[str] = None,
                 mako_dir: Optional[str] = None,
                 generated_include_path: str = 'starch.h',
                 generated_flavor_pattern: str = 'flavor.{0}.c',
                 generated_dispatcher_path: str = 'dispatcher.c',
                 generated_benchmark_path: str = 'benchmark.c',
                 generated_makefile_pattern: str = 'makefile.{0}',
                 generated_cmake_pattern: str = 'starch_{0}.cmake',
                 symbol_prefix: str = 'starch_',
                 prefix_function_symbols: bool = True):
        self.runtime_dir = runtime_dir
        self.output_dir = output_dir
        self.generated_include_path = os.path.join(output_dir, generated_include_path)
        self.generated_flavor_pattern = generated_flavor_pattern
        self.generated_dispatcher_path = os.path.join(output_dir, generated_dispatcher_path)
        self.generated_benchmark_path = os.path.join(output_dir, generated_benchmark_path)
        self.generated_makefile_pattern = generated_makefile_pattern
        self.generated_cmake_pattern = generated_cmake_pattern
        self.symbol_prefix = symbol_prefix
        self.prefix_function_symbols = prefix_function_symbols

        if template_dir is None and '__file__' in globals():
            template_dir = os.path.join(os.path.dirname(__file__), 'templates')
        if template_dir is None:
            raise RuntimeError('cannot determine template directory location, please specify template_dir')
        self.templates = mako.lookup.TemplateLookup(directories = [template_dir], module_directory = mako_dir, imports=['import os'])

        self.functions = {}
        self.flavors = {}
        self.function_impls = {}
        self.impl_files = []
        self.benchmark_files = []
        self.mixes = {}
        self.includes = []

    def generated_flavor_path(self, flavor: BuildFlavor) -> str:
        return os.path.join(self.output_dir, self.generated_flavor_pattern.format(flavor.name))

    def generated_makefile_path(self, mix: BuildMix) -> str:
        return os.path.join(self.output_dir, self.generated_makefile_pattern.format(mix.name))

    def generated_cmake_path(self, mix: BuildMix) -> str:
        return os.path.join(self.output_dir, self.generated_cmake_pattern.format(mix.name))

    def add_include(self, what):
        if what[0] == '<' or what[0] == '"':
            self.includes.append(what)
        else:
            self.includes.append('"' + what + '"')

    def add_function(self, 
                     name: str,
                     argtypes: Iterable[str],
                     returntype: str = 'void',
                     argnames: Optional[Iterable[str]] = None):
        if name in self.functions:
            raise RuntimeError('duplicated function: ' + name)
        fn = Function(self, name, argtypes, returntype, argnames)
        self.functions[name] = fn
        return fn

    def get_function(self, key: Union[str, Function]) -> Function:
        if isinstance(key, Function):
            return key
        return self.functions[key]                        

    def add_flavor(self, 
                   name: str,
                   description: str,
                   compile_flags: Iterable[str] = (),
                   test_function: Optional[str] = None):
        if name in self.flavors:
            raise RuntimeError('duplicated flavor: ' + name)
        flavor = BuildFlavor(self, name, description, compile_flags, test_function)
        self.flavors[name] = flavor
        return flavor

    def get_flavor(self, key: Union[str, BuildFlavor]) -> BuildFlavor:
        if isinstance(key, BuildFlavor):
            return key
        return self.flavors[key]                        

    def load_wisdom(self, path: str) -> Mapping[Function,Sequence[str]]:
        results: Dict[Function,List[str]] = {}

        try:
            f = open(path, 'r')
        except IOError:
            self.warning(None, None, f"ignoring missing wisdom file {path}")
            return results

        with f:
            for line in f:
                line = line.strip()
                if line == '' or line.startswith('#'):
                    continue

                parts = re.split('\s+', line)
                if len(parts) < 2:
                    continue

                func, impl = parts[:2]
                if func in self.functions:
                    results.setdefault(self.functions[func], []).append(impl)
                else:
                    self.warning(None, None, f"ignoring unknown function {func} in wisdom file {path}")

        return results

    def add_mix(self,
                name: str,
                description: str,
                flavors: Iterable[Union[BuildFlavor,str]],
                wisdom: Mapping[Union[Function,str],Iterable[str]] = {},
                wisdom_file: Optional[str] = None):
        if name in self.mixes:
            raise RuntimeError('duplicated mix: ' + name)

        resolved_flavors = map(self.get_flavor, flavors)
        if wisdom_file:
            resolved_wisdom = self.load_wisdom(wisdom_file)
        else:
            resolved_wisdom = dict( (self.get_function(name), list(values)) for name,values in wisdom.items() )
        mix = BuildMix(name, description, resolved_flavors, resolved_wisdom)
        self.mixes[name] = mix
        return mix

    def sym(self, symbol: str) -> str:
        return self.symbol_prefix + symbol

    def _build_impls(self, source: SourceFile, lineno: int, function_name: str, impl_name: str, flavors: Sequence[BuildFlavor]) -> Sequence[FunctionImpl]:
        if function_name not in self.functions:
            self.warning(source, lineno, f"implementation defined for unknown function '{function_name}', skipped")
            return []

        return [FunctionImpl(gen = self,
                             function = self.functions[function_name],
                             name = impl_name,
                             source = source,
                             lineno = lineno,
                             flavor = flavor) for flavor in flavors]

    def add_impl(self, impl):
        key = (impl.function, impl.name, impl.flavor.name)
        old = self.function_impls.get(key)
        if old:
            self.warning(impl.source, impl.lineno, f'duplicate definition of {impl.function.name} / {impl.name} / {impl.flavor.name}, previously defined at {old.source.path}:{old.lineno}')
            return
        self.function_impls[key] = impl
        impl.function.impls.append(impl)
        impl.flavor.impls.append(impl)
        impl.source.impls.append(impl)
        return impl

    def warning(self, source: Optional[SourceFile], lineno: Optional[int], message):
        if source is not None:
            if lineno is not None:
                print(f'{source.path}:{lineno+1}: warning: {message}', file=sys.stderr)
            else:
                print(f'{source.path}: warning: {message}', file=sys.stderr)
        else:
            print(f'warning: {message}', file=sys.stderr)

    def scan_benchmark_file(self, path: str):
        source = SourceFile(path)

        match_benchmark = re.compile(r'''[^a-zA-Z0-9_]+ STARCH_BENCHMARK \s* \( \s*      # macro call
                                         ([a-zA-Z0-9_]+) \s* \)                          # function name
                                      ''', re.VERBOSE)

        match_verify = re.compile(r'''[^a-zA-Z0-9_]+ STARCH_BENCHMARK_VERIFY \s* \( \s*  # macro call
                                         ([a-zA-Z0-9_]+) \s* \)                          # function name
                                      ''', re.VERBOSE)

        if source in self.benchmark_files:
            self.warning(source, None, "benchmark/verify source already added")
            return

        self.benchmark_files.append(source)
        with open(path, 'r') as f:
            for lineno, line in enumerate(f):
                if line[0] == '#':
                    continue   # ignore preprocessor lines

                for match in match_benchmark.finditer(line):
                    function_name = match.group(1)
                    if function_name in self.functions:
                        function = self.functions[function_name]
                        if function.benchmark:
                            self.warning(source, lineno, f"duplicate benchmark defined for unknown function {function_name}")
                        function.benchmark = source
                    else:
                        self.warning(source, lineno, f"benchmark defined for unknown function {function_name}, ignored")

                for match in match_verify.finditer(line):
                    function_name = match.group(1)
                    if function_name in self.functions:
                        function = self.functions[function_name]
                        if function.benchmark_verify:
                            self.warning(source, lineno, f"duplicate benchmark verifier defined for unknown function {function_name}")
                        function.benchmark_verify = source
                    else:
                        self.warning(source, lineno, f"benchmark verifier defined for unknown function {function_name}, ignored")

    def scan_benchmarks(self, pattern: str):
        for path in glob.glob(pattern):
            self.scan_benchmark_file(path)

    def scan_impl_file(self, path: str, flavors: Optional[Sequence[Union[BuildFlavor,str]]] = None):
        source = SourceFile(path)

        match_impl = re.compile(r'''[^a-zA-Z0-9_]+ STARCH_IMPL \s* \( \s*      # macro call
                                    ([a-zA-Z0-9_]+) \s* , \s*                  # function name
                                    ([a-zA-Z0-9_]+) \s* \)                     # implementation name
                                 ''', re.VERBOSE)

        if flavors is None:
            resolved_flavors = self.flavors.values()
        else:
            resolved_flavors = map(self.get_flavor, flavors)

        with open(path, 'r') as f:
            for lineno, line in enumerate(f):
                if line[0] == '#':
                    continue   # ignore preprocessor lines

                for match in match_impl.finditer(line):
                    for impl in self._build_impls(source, lineno, match.group(1), match.group(2), resolved_flavors):
                        self.add_impl(impl)

    def scan_impls(self, pattern: str, flavors: Optional[Sequence[Union[BuildFlavor,str]]] = None):
        for path in glob.glob(pattern):
            self.scan_impl_file(path, flavors)

    def _render(self, template_path, output_path, **kwargs):
        t = self.templates.get_template(template_path)
        try:
            result = t.render(gen=self, current_dir=os.path.dirname(output_path), **kwargs).replace('\r\n', '\n')
        except:
            print(f'Exception rendering template {template_path}:', file=sys.stderr)
            print(mako.exceptions.text_error_template().render(), file=sys.stderr)
            raise

        if os.path.exists(output_path):
            with open(output_path, 'r') as f:
                contents = f.read()
            if contents == result:
                print(f'unchanged: {output_path}', file=sys.stderr)
                return

        with open(output_path, 'w') as f:
            f.write(result)
        print(f'    wrote: {output_path}', file=sys.stderr)

    def generate(self, makefiles=True, cmake=True):
        if not self.functions:
            self.warning(None, None, 'no functions defined')
        if not self.flavors:
            self.warning(None, None, 'no flavors defined')
        if not self.mixes:
            self.warning(None, None, 'no mixes defined')
        for function in self.functions.values():
            if not function.impls:
                self.warning(None, None, f'no implementations of function {function.name} provided')

        self._render('/starch.h.template', self.generated_include_path)

        for flavor in self.flavors.values():
            self._render('/flavor.c.template', self.generated_flavor_path(flavor), flavor=flavor)

        self._render('/dispatcher.c.template', self.generated_dispatcher_path)
        self._render('/benchmark.c.template', self.generated_benchmark_path)

        for mix in self.mixes.values():
            if makefiles:
                self._render('/makefile.template', self.generated_makefile_path(mix), mix=mix)
            if cmake:
                self._render('/cmake.template', self.generated_cmake_path(mix), mix=mix)

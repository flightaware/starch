# starch: framework glue for selecting ISA-specific code at runtime

import sys
import re
import os
import mako.lookup

from typing import Optional, Union, Iterable, Sequence, MutableSequence, Mapping, MutableMapping, FrozenSet

class Feature(object):
    """Feature represents a type of code that can only be built with
certain compiler flags. For example, code that uses NEON intrinsics
can only be compiled if the compiler is building for an ARM instruction
set that supports NEON. Implementation code should be conditionally
compiled using the corresponding macro name, and should declare
themselves using the STARCH_IMPL_REQUIRES macro."""

    gen: 'Generator'
    name: str
    description: str
    
    def __init__(self,
                 gen: 'Generator',
                 name: str,
                 description: str):
        self.gen = gen
        self.name = name
        self.description = description

    @property
    def macro(self) -> str:
        return 'STARCH_FEATURE_' + self.name.upper()


class BuildFlavor(object):
    """BuildFlavor models code built with specific compiler flags.
Shared implementation code will be built multiple times, once per flavor.

Each flavor has an associated test function that is called at runtime to
check if the current hardware supports the code emitted by the flavor. If
the test function returns false, no code built with the flavor will be executed.

Each flavor has a (possibly empty) list of optional Features that may
be present at runtime. This list controls which feature-dependent code is
compiled for this flavor (e.g. an x86 flavor might try to build code that
depends on SSE, but should not try to build code that depends on ARM NEON
intrinsics)"""

    gen: 'Generator'
    name: str
    description: str
    compile_flags: Sequence[str]
    features: FrozenSet[Feature]
    test_function: Optional[str]

    def __init__(self,
                 gen: 'Generator',
                 name: str,
                 description: str,
                 compile_flags: Iterable[str] = (),
                 features: Iterable[Feature] = (),
                 test_function: Optional[str] = None):

        self.gen = gen
        self.name = name
        self.compile_flags = tuple(compile_flags)
        self.features = frozenset(features)
        self.test_function = test_function

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


class Function(object):
    """A user-callable function that will be dispatched to
one of the many possible implementations based on runtime feature
support."""

    gen: 'Generator'
    name: str
    returntype: str
    argtypes: Sequence[str]
    argnames: Sequence[str]
    impls: Sequence['FunctionImpl'] = []
    has_benchmark: bool = False

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
        # nb: no prefix!
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


class FunctionImpl(object):
    """A possible implementation of a function, not built in any particular way yet."""

    gen: 'Generator'
    function: Function
    name: str
    feature: Optional[Feature]
    source: 'SourceFile'
    lineno: int

    def __init__(self,
                 gen: 'Generator',
                 function: Function,
                 name: str,
                 feature: Optional[Feature],
                 source: 'SourceFile',
                 lineno: int):
        self.gen = gen
        self.function = function
        self.name = name
        self.feature = feature
        self.source = source
        self.lineno = lineno

    def wisdom_name(self, flavor) -> str:
        return self.name + '_' + flavor.name

    def impl_symbol(self, flavor) -> str:
        return self.gen.sym(self.function.name + '_' + self.name + '_' + flavor.name)


class SourceFile(object):
    """A scanned source file that contains implementation code."""

    path: str
    impls: Sequence[FunctionImpl] = []

    def __init__(self, path):
        self.path = path


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

class Generator(object):
    functions: MutableMapping[str, Function] = {}
    features: MutableMapping[str, Feature] = {}
    features_by_macro: MutableMapping[str, Feature] = {}
    flavors: MutableMapping[str, BuildFlavor] = {}
    function_impls: MutableMapping[str, FunctionImpl] = {}
    source_files: MutableSequence[SourceFile] = []
    mixes: MutableMapping[str, BuildMix] = {}
    symbol_prefix: str
    templates: mako.lookup.TemplateLookup
    generated_include_path: str
    generated_flavor_pattern: str
    generated_dispatcher_path: str
    generated_benchmark_path: str
    generated_makefile_pattern: str
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
                 symbol_prefix: str = 'starch_'):
        self.runtime_dir = runtime_dir
        self.output_dir = output_dir
        self.generated_include_path = os.path.join(output_dir, generated_include_path)
        self.generated_flavor_pattern = generated_flavor_pattern
        self.generated_dispatcher_path = os.path.join(output_dir, generated_dispatcher_path)
        self.generated_benchmark_path = os.path.join(output_dir, generated_benchmark_path)
        self.generated_makefile_pattern = generated_makefile_pattern
        self.symbol_prefix = symbol_prefix

        if template_dir is None and '__file__' in globals():
            template_dir = os.path.join(os.path.dirname(__file__), 'templates')
        if template_dir is None:
            raise RuntimeError('cannot determine template directory location, please specify template_dir')
        self.templates = mako.lookup.TemplateLookup(directories = [template_dir], module_directory = mako_dir, imports=['import os'])

    def generated_flavor_path(self, flavor: BuildFlavor) -> str:
        return os.path.join(self.output_dir, self.generated_flavor_pattern.format(flavor.name))

    def generated_makefile_path(self, mix: BuildMix) -> str:
        return os.path.join(self.output_dir, self.generated_makefile_pattern.format(mix.name))

    def add_include(self, what):
        if what[0] == '<':
            self.includes.append(what)
        else:
            self.includes.append('"' + what + '"')

    def add_feature(self,
                    name: str,
                    description: str):
        if name in self.features:
            raise RuntimeError('duplicated flavor: ' + name)
        feature = Feature(self, name, description)
        self.features[name] = self.features_by_macro[feature.macro] = feature

    def get_feature(self, key: Union[str, Feature]) -> Feature:
        if isinstance(key, Feature):
            return key
        return self.features[key]                        
        
    def get_feature_macro(self, key: str) -> Optional[Feature]:
        return self.features_by_macro.get(key, None)
        
    def add_function(self, 
                     name: str,
                     argtypes: Iterable[str],
                     returntype: str = 'void',
                     argnames: Optional[Iterable[str]] = None):
        if name in self.functions:
            raise RuntimeError('duplicated function: ' + name)
        self.functions[name] = Function(self, name, argtypes, returntype, argnames)

    def get_function(self, key: Union[str, Function]) -> Function:
        if isinstance(key, Function):
            return key
        return self.functions[key]                        
        
    def add_flavor(self, 
                   name: str,
                   description: str,
                   compile_flags: Iterable[str] = (),
                   features: Iterable[Union[Feature,str]] = (),
                   test_function: Optional[str] = None):
        if name in self.flavors:
            raise RuntimeError('duplicated flavor: ' + name)
        resolved_features = map(self.get_feature, features)
        self.flavors[name] = BuildFlavor(self, name, description, compile_flags, resolved_features, test_function)

    def get_flavor(self, key: Union[str, BuildFlavor]) -> BuildFlavor:
        if isinstance(key, BuildFlavor):
            return key
        return self.flavors[key]                        
        
    def add_mix(self,
                name: str,
                description: str,
                flavors: Iterable[Union[BuildFlavor,str]],
                wisdom: Mapping[Union[Function,str],Iterable[str]] = {}):
        if name in self.mixes:
            raise RuntimeError('duplicated mix: ' + name)

        resolved_flavors = map(self.get_flavor, flavors)
        resolved_wisdom = dict( (self.get_function(name), values) for name,values in wisdom.items() )
        self.mixes[name] = BuildMix(name, description, resolved_flavors, resolved_wisdom)

    def sym(self, symbol: str) -> str:
        return self.symbol_prefix + symbol
    
    def build_impl(self, source: SourceFile, lineno: int, function_name: str, impl_name: str, feature_name: Optional[str] =  None) -> Optional[FunctionImpl]:
        if function_name not in self.functions:
            self.warning(source, lineno, f"implementation defined for unknown function '{function_name}', skipped")
            return None

        function = self.functions[function_name]

        feature: Optional[Feature] = None        
        if feature_name is not None:
            if feature_name not in self.features_by_macro:
                self.warning(source, lineno, f"implementation {function_name} ({impl_name}) requires unknown feature '{feature_name}', skipped")
                return None
            feature = self.features_by_macro.get(feature_name)

        return FunctionImpl(gen = self,
                            function = function,
                            name = impl_name,
                            source = source,
                            lineno = lineno,
                            feature = feature)

    def add_impl(self, impl):
        key = (impl.function, impl.name)
        old = self.function_impls.get(key)
        if old:
            self.warning('duplicate definition of {impl.function.name} / {impl.name}, previously defined at {old.location[0]}:{old.location[1]}')
            return
        self.function_impls[key] = impl
        impl.function.impls.append(impl)
        impl.source.impls.append(impl)

    def warning(self, source: Optional[SourceFile], lineno: Optional[int], message):
        if source is not None:
            if lineno is not None:
                print(f'{source.path}:{lineno}: warning: {message}', file=sys.stderr)
            else:
                print(f'{source.path}: warning: {message}', file=sys.stderr)
        else:
            print(f'warning: {message}', file=sys.stderr)

    def scan_file(self, path: str):
        source = SourceFile(path)
        self.source_files.append(source)

        match_impl = re.compile(r'''[^a-zA-Z0-9_]+ STARCH_IMPL \s* \( \s*      # macro call
                                    ([a-zA-Z0-9_]+) \s* , \s*                  # function name
                                    ([a-zA-Z0-9_]+) \s* \)                     # implementation name
                                 ''', re.VERBOSE)
        match_impl_requires = re.compile(r'''[^a-zA-Z0-9_]+ STARCH_IMPL_REQUIRES \s* \( \s*      # macro call
                                             ([a-zA-Z0-9_]+) \s* , \s*                           # function name
                                             ([a-zA-Z0-9_]+) \s* , \s*                           # implementation name
                                             ([a-zA-Z0-9_]+) \s* \)                              # feature name
                                          ''', re.VERBOSE)

        match_benchmark = re.compile(r'''[^a-zA-Z0-9_]+ STARCH_BENCHMARK \s* \( \s*      # macro call
                                         ([a-zA-Z0-9_]+) \s* \)                          # function name
                                      ''', re.VERBOSE)

        with open(path, 'r') as f:
            for lineno, line in enumerate(f):
                if line[0] == '#':
                    continue   # ignore preprocessor lines

                for match in match_impl.finditer(line):
                    impl = self.build_impl(source, lineno, match.group(1), match.group(2))
                    if impl:
                        self.add_impl(impl)

                for match in match_impl_requires.finditer(line):
                    impl = self.build_impl(source, lineno, match.group(1), match.group(2), match.group(3))
                    if impl:
                        self.add_impl(impl)

                for match in match_benchmark.finditer(line):
                    function_name = match.group(1)
                    if function_name in self.functions:
                        self.functions[function_name].has_benchmark = True
                    else:
                        self.warning(source, lineno, f"benchmark defined for unknown function {function_name}, ignored")

    def render(self, template_path, output_path, **kwargs):
        t = self.templates.get_template(template_path)
        result = t.render(gen=self, current_dir=os.path.dirname(output_path), **kwargs)

        with open(output_path, 'w') as f:
            f.write(result)
        print(f'wrote {output_path}', file=sys.stderr)

    def generate(self):
        if not self.functions:
            self.warning(None, None, 'no functions defined')
        if not self.flavors:
            self.warning(None, None, 'no flavors defined')
        if not self.mixes:
            self.warning(None, None, 'no mixes defined')
        for function in self.functions.values():
            if not function.impls:
                self.warning(None, None, f'no implementations of function {function.name} provided')

        self.render('/starch.h.template', self.generated_include_path)

        for name, flavor in self.flavors.items():
            self.render('/flavor.c.template', self.generated_flavor_path(flavor), flavor=flavor)

        self.render('/dispatcher.c.template', self.generated_dispatcher_path)
        self.render('/benchmark.c.template', self.generated_benchmark_path)

        for name, mix in self.mixes.items():
            self.render('/makefile.template', self.generated_makefile_path(mix), mix=mix)


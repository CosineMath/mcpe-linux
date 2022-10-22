
aglinker_include = include_directories('include/')

aglinker_src = [
    'dlfcn.cpp',
    'linker.cpp',
    'linker_phdr.cpp',
    'strlcpy.c',
    'strlcat.c',
]

aglinker_c_flags = []
if ['x86'].contains(host_machine.cpu_family())
    aglinker_c_flags += ['-DANDROID_X86_LINKER']
else
    error('Architecture not supported.')
endif


libaglinker = shared_library(
    'aglinker',
    aglinker_src,
    c_args: aglinker_c_flags,
    cpp_args: aglinker_c_flags,
    gnu_symbol_visibility : 'hidden',
    include_directories: [aglinker_include],
    dependencies: [],
    install: true,
)

dep_aglinker = declare_dependency(
    include_directories: [aglinker_include],
    link_with: [libaglinker],
)

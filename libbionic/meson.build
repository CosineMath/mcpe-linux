
libbionic = shared_library(
    'bionic',
    [
        'assert.c', 'ctype.c', 'errno.c', 'libgcc.c', 'pthread.c',
        'stat.c', 'stat_wrapper.c', 'stdio.c', 'stub.c', 'futex.c',
        'dirent.c', 'netdb.c',
        '__strlen_chk.cpp', '__stack_chk_guard.c',
        'strlcpy.c',
        'symbol_name.c',
    ],
    include_directories: [],
    dependencies: [],
    link_args : ['-pthread', '-shared-libgcc'],
    install: true,
)

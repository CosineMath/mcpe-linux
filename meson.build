project('mcpe-linux', ['c', 'cpp'],
        default_options: [
            'buildtype=debugoptimized',
            'c_std=gnu99',
            'cpp_std=gnu++11',
        ],
        version: '4.4',
        meson_version: '>= 0.52.0',
)

subdir('aglinker')
subdir('libbionic')
subdir('libjni')

subdir('libandroid')
subdir('liblog')
subdir('libfmod')

subdir('libminecraftpe_java')

executable(
    'main',
    ['main.c'],
    include_directories: [],
    dependencies: [dep_aglinker, dep_minecraftpe_java],
    link_args : ['-ldl'],
    install: true,
    install_rpath: '$ORIGIN/../lib',
)


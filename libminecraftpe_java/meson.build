
minecraftpe_java_include = [include_directories('include/')]

libminecraftpe_java = shared_library(
    'minecraftpe_java',
    [
        'android_app_NativeActivity.cpp',
        'com_mojang_minecraftpe_MainActivity.cpp',
        'launcher.cpp',
    ],
    include_directories: [minecraftpe_java_include],
    dependencies: [
        dependency('glfw3', version: '>= 3.2.1'),
        dependency('libpng16', version: '>= 1.6'),
        dep_aglinker,
        dep_android,
    ],
    install: true,
    install_rpath: '$ORIGIN/../lib',
)

dep_minecraftpe_java = declare_dependency(
    include_directories: [minecraftpe_java_include],
    link_with: [libminecraftpe_java],
)

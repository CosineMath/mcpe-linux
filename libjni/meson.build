
jni_include = include_directories('include/')

libjni = shared_library(
    'jni',
    ['libjni.c', 'hash_map.c'],
    include_directories: [jni_include],
    dependencies: [],
    install: true,
)

dep_jni = declare_dependency(
    include_directories: [jni_include],
    link_with: [libjni],
)

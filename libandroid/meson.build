
android_include = [include_directories('include/')]

libandroid = shared_library(
    'android',
    [
        'asset_manager.c', 'configuration.c', 'input.c', 'looper.c', 'native_activity.c', 'native_window.c',
    ],
    include_directories: [android_include],
    dependencies: [dep_jni],
    install: true,
)

dep_android = declare_dependency(
    include_directories: [android_include],
    link_with: [libandroid],
    dependencies: [dep_jni],
)

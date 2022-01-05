{
    'targets': [{
        'target_name': 'liblibltc-napi-limited-native',
        'sources': [
            'src/libltc_napi_limited.cc',
            'src/libltc/ltc.c',
            'src/libltc/decoder.c',
            'src/libltc/encoder.c',
            'src/libltc/timecode.c',
            'src/libltc/ltcdecode.c',
        ],
        'include_dirs': ["<!@(node -p \"require('node-addon-api').include\")"],
        'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
        'cflags!': ['-fno-exceptions'],
        'cflags_cc!': ['-fno-exceptions'],
        'xcode_settings': {
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
            'CLANG_CXX_LIBRARY': 'libc++',
            'MACOSX_DEPLOYMENT_TARGET': '10.7'
        },
        'msvs_settings': {
            'VCCLCompilerTool': { 'ExceptionHandling': 1 },
        }
    }]
}
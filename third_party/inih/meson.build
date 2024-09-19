project('inih',
    ['c'],
    license : 'BSD-3-Clause',
    version : '58',
    default_options : ['cpp_std=c++11']
)

#### options ####
arg_static = []
distro_install = get_option('distro_install')
extra_args = []

if distro_install
    pkg = import('pkgconfig')
else
    if not get_option('multi-line_entries')
        arg_static += ['-DINI_ALLOW_MULTILINE=0']
    endif
    if not get_option('utf-8_bom')
        arg_static += ['-DINI_ALLOW_BOM=0']
    endif
    if not get_option('inline_comments')
        arg_static += ['-DINI_ALLOW_INLINE_COMMENTS=0']
    endif
    inline_comment_prefix = get_option('inline_comment_prefix')
    if inline_comment_prefix != ';'
        arg_static += ['-DINI_INLINE_COMMENT_PREFIXES="' + inline_comment_prefix + '"']
    endif
    sol_comment_prefix = get_option('start-of-line_comment_prefix')
    if sol_comment_prefix != ';#'
        arg_static += ['-DINI_START_COMMENT_PREFIXES="' + sol_comment_prefix + '"']
    endif
    if get_option('allow_no_value')
        arg_static += ['-DINI_ALLOW_NO_VALUE=1']
    endif
    if get_option('stop_on_first_error')
        arg_static += ['-DINI_STOP_ON_FIRST_ERROR=1']
    endif
    if get_option('report_line_numbers')
        arg_static += ['-DINI_HANDLER_LINENO=1']
    endif
    if get_option('call_handler_on_new_section')
        arg_static += ['-DINI_CALL_HANDLER_ON_NEW_SECTION=1']
    endif
    if get_option('use_heap')
        arg_static += ['-DINI_USE_STACK=0']
    endif
    max_line_length = get_option('max_line_length')
    if max_line_length != 200
        arg_static += ['-DINI_MAX_LINE=' + max_line_length.to_string()]
    endif
    initial_malloc_size = get_option('initial_malloc_size')
    if initial_malloc_size != 200
        arg_static += ['-DINI_INITIAL_ALLOC=' + initial_malloc_size.to_string()]
    endif
    if get_option('allow_realloc')
        arg_static += ['-DINI_ALLOW_REALLOC=1']
    endif
endif

if host_machine.system() == 'windows'
    lib = get_option('default_library')
    if lib == 'both'
        error('default_library=both is not supported on Windows')
    elif lib == 'shared'
        extra_args += '-DINI_SHARED_LIB'
        add_project_arguments('-DINI_SHARED_LIB_BUILDING', language: ['c', 'cpp'])
    endif
endif

#### inih ####
inc_inih = include_directories('.')

lib_inih = library('inih',
    ['ini.c'],
    include_directories : inc_inih,
    c_args : [arg_static, extra_args],
    install : distro_install,
    soversion : '0',
    gnu_symbol_visibility: 'hidden'
)

if distro_install
    install_headers('ini.h')

    pkg.generate(lib_inih,
        name : 'inih',
        description : 'simple .INI file parser',
        extra_cflags : extra_args,
    )
endif

inih_dep = declare_dependency(
    link_with : lib_inih,
    compile_args : arg_static + extra_args,
    include_directories : inc_inih
)

#### INIReader ####
if get_option('with_INIReader')
    add_languages('cpp')
    inc_INIReader = include_directories('cpp')

    lib_INIReader = library('INIReader',
        ['cpp/INIReader.cpp'],
        cpp_args : extra_args,
        include_directories : inc_INIReader,
        dependencies : inih_dep,
        install : distro_install,
        soversion : '0',
        gnu_symbol_visibility: 'hidden'
    )

    if distro_install
        install_headers('cpp/INIReader.h')

        pkg.generate(lib_INIReader,
            name : 'INIReader',
            description : 'simple .INI file parser for C++',
            extra_cflags : extra_args,
        )
    endif

    INIReader_dep = declare_dependency(
        link_with : lib_INIReader,
        include_directories : inc_INIReader,
        compile_args : extra_args
    )
endif

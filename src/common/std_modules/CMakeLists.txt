file(GLOB std_src *.cppm)

add_library(std)

target_sources(std
        PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES FILES
        ${std_src}
)

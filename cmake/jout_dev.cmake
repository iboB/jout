# standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# misc config
set_property(GLOBAL PROPERTY USE_FOLDERS ON) # use solution folders
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # binaries to bin

if(MSVC)
    # /Zc:preprocessor - incompatible with Windows.h
    add_compile_options(
        /W4
        -D_CRT_SECURE_NO_WARNINGS /Zc:__cplusplus /permissive-
        /volatile:iso /Zc:throwingNew /Zc:templateScope /utf-8 -DNOMINMAX=1
        /wd4251 /wd4275
    )
else()
    add_compile_options(-Wall -Wextra)
endif()

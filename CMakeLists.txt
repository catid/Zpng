cmake_minimum_required(VERSION 3.5)
project(ccat)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(MSVC)
else()
    set(CMAKE_CXX_FLAGS "-Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -march=native")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native")
endif()

set(CMAKE_CXX_STANDARD 11)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Zstd library source files
set(ZSTD_LIB_SRCFILES
        zstd/bitstream.h
        zstd/compiler.h
        zstd/cover.c
        zstd/cpu.h
        zstd/divsufsort.c
        zstd/divsufsort.h
        zstd/entropy_common.c
        zstd/error_private.c
        zstd/error_private.h
        zstd/fse.h
        zstd/fse_compress.c
        zstd/fse_decompress.c
        zstd/huf.h
        zstd/huf_compress.c
        zstd/huf_decompress.c
        zstd/mem.h
        zstd/pool.c
        zstd/pool.h
        zstd/threading.c
        zstd/threading.h
        zstd/xxhash.c
        zstd/xxhash.h
        zstd/zdict.c
        zstd/zdict.h
        zstd/zstd.h
        zstd/zstdmt_compress.c
        zstd/zstdmt_compress.h
        zstd/zstd_common.c
        zstd/zstd_compress.c
        zstd/zstd_compress_internal.h
        zstd/zstd_decompress.c
        zstd/zstd_double_fast.c
        zstd/zstd_double_fast.h
        zstd/zstd_errors.h
        zstd/zstd_fast.c
        zstd/zstd_fast.h
        zstd/zstd_internal.h
        zstd/zstd_lazy.c
        zstd/zstd_lazy.h
        zstd/zstd_ldm.c
        zstd/zstd_ldm.h
        zstd/zstd_opt.c
        zstd/zstd_opt.h
)

# Zpng library source files
set(ZPNG_LIB_SRCFILES
        zpng.cpp
        zpng.h
)

# Zpng unit tester
set(ZPNG_TEST_SRCFILES
        apps/zpng_test.cpp
)

# Zpng app
set(ZPNG_APP_SRCFILES
        apps/zpng_app.cpp
)

add_library(zpnglib ${ZPNG_LIB_SRCFILES} ${ZSTD_LIB_SRCFILES})

add_executable(unit_test ${ZPNG_TEST_SRCFILES})
target_link_libraries(unit_test zpnglib Threads::Threads)

add_executable(zpng ${ZPNG_APP_SRCFILES})
target_link_libraries(zpng zpnglib Threads::Threads)

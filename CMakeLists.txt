cmake_minimum_required(VERSION 3.10)

if (PROJECT_NAME)
    set(RMGR_FIB_IS_TOP_LEVEL 0)
else()
    set(RMGR_FIB_IS_TOP_LEVEL 1)
endif()


###################################################################################################
# Options

include(CMakeDependentOption)
option(RMGR_FIB_BUILD_TESTS "Whether to build unit tests" ${RMGR_FIB_IS_TOP_LEVEL})


###################################################################################################
# Project configuration

project(rmgr-fib
    VERSION   1.0
    LANGUAGES CXX
)

include(GNUInstallDirs)


###################################################################################################
# Build flags

include(CheckCXXSymbolExists)
if (MSVC OR (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC"))
    check_cxx_symbol_exists("_M_IX86"  "" _M_IX86)
    check_cxx_symbol_exists("_M_AMD64" "" _M_AMD64)
    if (_M_IX86 OR _M_AMD64)
        set(RMGR_FIB_ARCH_IS_X86  1)
        if (_M_IX86)
            set(RMGR_FIB_SSE2_FLAGS  "/arch:SSE2")
            set(RMGR_FIB_SSE3_FLAGS  "/arch:SSE2")
            set(RMGR_FIB_SSSE3_FLAGS "/arch:SSE2")
            set(RMGR_FIB_SSE41_FLAGS "/arch:SSE2")
            set(RMGR_FIB_SSE42_FLAGS "/arch:SSE2")
        endif()
        set(RMGR_FIB_AVX_FLAGS       "/arch:AVX")
        set(RMGR_FIB_FMA_FLAGS       "/arch:AVX")
        set(RMGR_FIB_AVX2_FLAGS      "/arch:AVX2")
        set(RMGR_FIB_AVX512F_FLAGS   "/arch:AVX512")
        set(RMGR_FIB_AVX512DQ_FLAGS  "/arch:AVX512")
        set(RMGR_FIB_AVX512VL_FLAGS  "/arch:AVX512")

        if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            list(APPEND RMGR_FIB_SSE3_FLAGS  "-msse3")
            list(APPEND RMGR_FIB_SSSE3_FLAGS "-mssse3")
            list(APPEND RMGR_FIB_SSE41_FLAGS "-msse4.1")
            list(APPEND RMGR_FIB_SSE42_FLAGS "-msse4.2")
            list(APPEND RMGR_FIB_FMA_FLAGS   "-mfma")
        endif()
    else()
        check_cxx_symbol_exists("_M_ARM"   "" _M_ARM)
        check_cxx_symbol_exists("_M_ARM64" "" _M_ARM64)
        if (_M_ARM OR _M_ARM64)
            set(RMGR_FIB_ARCH_IS_ARM  1)
            set(RMGR_FIB_NEON_FLAGS   "")
        endif()
    endif()
elseif (CMAKE_COMPILER_IS_GNUCXX OR (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang"))
    check_cxx_symbol_exists("__i386__"  "" __i386__)
    check_cxx_symbol_exists("__amd64__" "" __amd64__)
    if (__i386__ OR __amd64__)
        set(RMGR_FIB_ARCH_IS_X86      1)
        if (_M_IX86)
            set(RMGR_FIB_SSE2_FLAGS  "-msse2")
        endif()
        set(RMGR_FIB_SSE3_FLAGS      "-msse3")
        set(RMGR_FIB_SSSE3_FLAGS     "-mssse3")
        set(RMGR_FIB_SSE41_FLAGS     "-msse4.1")
        set(RMGR_FIB_SSE42_FLAGS     "-msse4.2")
        set(RMGR_FIB_AVX_FLAGS       "-mavx")
        set(RMGR_FIB_FMA_FLAGS       "-mfma")
        set(RMGR_FIB_AVX_FLAGS       "-mavx2")
        set(RMGR_FIB_AVX512DQ_FLAGS  "-mavx512dq")
        set(RMGR_FIB_AVX512VL_FLAGS  "-mavx512vl")
        if (CMAKE_COMPILER_IS_GNUCXX AND (WIN32 OR CYGWIN))
            list(APPEND RMGR_FIB_AVX512_FLAGS "-fno-exceptions" "-fno-asynchronous-unwind-tables") # Fixes a build error in AVX-512 code
        endif()
    else()
        check_cxx_symbol_exists("__aarch64__" "" __aarch64__)
        check_cxx_symbol_exists("__arm__"     "" __arm__)
        if (__aarch64__)
            set(RMGR_FIB_ARCH_IS_ARM  1)
            set(RMGR_FIB_NEON_FLAGS   "")
        elseif(__arm__)
            set(RMGR_FIB_ARCH_IS_ARM  1)
            set(RMGR_FIB_NEON_FLAGS   "-mfpu=neon")
            check_cxx_symbol_exists("__ARM_PCS_VFP" "" __ARM_PCS_VFP)
            if (__ARM_PCS_VFP)
                set(RMGR_FIB_ARM_COMPILE_OPTIONS "${RMGR_FIB_NEON_FLAGS} -mfloat-abi=hard")
            else()
                set(RMGR_FIB_ARM_COMPILE_OPTIONS "${RMGR_FIB_NEON_FLAGS} -mfloat-abi=softfp")
            endif()
        endif()
    endif()
endif()


set(RMGR_FIB_COMPILE_OPTIONS)
if (MSVC OR (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC"))
    list(APPEND RMGR_FIB_COMPILE_OPTIONS "/W4")
    list(APPEND RMGR_FIB_COMPILE_OPTIONS "/MP") # Multi-processor build
elseif (CMAKE_COMPILER_IS_GNUCXX OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
    list(APPEND RMGR_FIB_COMPILE_OPTIONS "-Wall")
endif()


###################################################################################################
# rmgr-fib

add_library(rmgr-fib INTERFACE)

target_include_directories(rmgr-fib INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_FULL_INCLUDE_DIR}>"
)


###################################################################################################
# Unit tests

if (RMGR_FIB_BUILD_TESTS)
    add_subdirectory(tests)
    set_directory_properties(PROPERTIES VS_STARTUP_PROJECT rmgr-fib-tests)
endif()

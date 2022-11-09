cmake_minimum_required(VERSION 3.10)

function(rmgr_download_and_extract url path createFolder)
    # Create destination folder if needed
    get_filename_component(folder "${path}" DIRECTORY)
    if (NOT EXISTS "${folder}")
        file(MAKE_DIRECTORY "${folder}")
    endif()

    # Download
    get_filename_component(filename "${path}" NAME)
    if (NOT EXISTS "${path}")
        message(STATUS "Downloading ${filename}")
        file(DOWNLOAD "${url}" "${path}" STATUS downloadStatus)
        list(GET downloadStatus 0 downloadStatusValue)
        if (NOT downloadStatusValue EQUAL 0)
            list(GET downloadStatus 1 downloadStatusMsg)
            message(FATAL_ERROR "Failed to download ${filename}: ${downloadStatusMsg}")
        endif()
    endif()

    # Extract
    if (createFolder)
        get_filename_component(name "${filename}" NAME_WE)
        set(folder "${folder}/${name}")
        if (NOT EXISTS "${folder}")
            file(MAKE_DIRECTORY "${folder}")
        endif()
    endif()
    message(STATUS "Extracting ${filename}")
    execute_process(COMMAND           "${CMAKE_COMMAND}" -E tar xf "${path}"
                    WORKING_DIRECTORY "${folder}"
                    RESULT_VARIABLE   extractResult
                    ERROR_VARIABLE    extractError)
    if (NOT extractResult EQUAL 0)
        message(FATAL_ERROR "Failed to extract ${filename}: ${extractError}")
    endif()
endfunction()


################################################################################
# GTest

if (TARGET gtest)
    set(GTEST_LIBRARIES    gtest)
    set(GTEST_INCLUDE_DIRS "${gtest_SOURCE_DIR}/include")
else()
    find_package(GTest)
    if (NOT GTEST_FOUND)
		set(GTEST_VERSION 1.12.1)
        set(GTEST_DIR     "${CMAKE_CURRENT_SOURCE_DIR}/gtest/googletest-release-${GTEST_VERSION}")
        set(GTEST_URL     "https://github.com/google/googletest/archive/release-${GTEST_VERSION}.tar.gz")

        if (NOT EXISTS "${GTEST_DIR}")
            rmgr_download_and_extract("${GTEST_URL}" "${GTEST_DIR}.tar.gz" FALSE)
        endif()

        if (MSVC)
            set(gtest_force_shared_crt ON CACHE BOOL "")
        endif()

        add_subdirectory("${GTEST_DIR}" EXCLUDE_FROM_ALL)
        set(GTEST_LIBRARIES    gtest)
        set(GTEST_INCLUDE_DIRS "${gtest_SOURCE_DIR}/include")
    endif()
endif()


###############################################################################
# A bit of platform detection

if (MSVC OR CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    include(CheckCXXSymbolExists)
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
        set(RMGR_FIB_AVX512DQ_FLAGS  "/arch:AVX512")
        set(RMGR_FIB_AVX512VL_FLAGS  "/arch:AVX512")
    else()
        check_cxx_symbol_exists("_M_ARM"   "" _M_ARM)
        check_cxx_symbol_exists("_M_ARM64" "" _M_ARM64)
        if (_M_ARM OR _M_ARM64)
            set(RMGR_FIB_ARCH_IS_ARM  1)
            set(RMGR_FIB_NEON_FLAGS   "")
        endif()
    endif()
elseif (CMAKE_COMPILER_IS_GNUCXX OR (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang"))
    include(CheckCXXSymbolExists)
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


###############################################################################
# Main Target

set(RMGR_FIB_TESTS_FILES
    "main.cpp"
)

if (RMGR_FIB_ARCH_IS_X86)
    set(RMGR_FIB_IS_LIST SSE2 SSE3 SSSE3 SSE41 SSE42)
    foreach (is ${RMGR_FIB_IS_LIST})
        configure_file("${CMAKE_CURRENT_SOURCE_DIR}/x86_tests.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/${is}_tests.cpp")
        list(APPEND RMGR_FIB_TESTS_FILES "${CMAKE_CURRENT_BINARY_DIR}/${is}_tests.cpp")
        set_property(SOURCE "${CMAKE_CURRENT_BINARY_DIR}/${is}_tests.cpp" PROPERTY COMPILE_OPTIONS     ${RMGR_FIB_${is}_FLAGS})
        set_property(SOURCE "${CMAKE_CURRENT_BINARY_DIR}/${is}_tests.cpp" PROPERTY COMPILE_DEFINITIONS "IS=${is}" "RMGR_FIB_ENABLE_${is}=1")
    endforeach()
endif()

source_group("Source Files" FILES ${RMGR_FIB_TESTS_FILES})

add_executable(rmgr-fib-tests ${RMGR_FIB_TESTS_FILES})

target_link_libraries(rmgr-fib-tests PRIVATE
    ${GTEST_LIBRARIES}
    rmgr-fib
)

target_include_directories(rmgr-fib-tests PRIVATE ${GTEST_INCLUDE_DIRS})
target_compile_options(rmgr-fib-tests PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${RMGR_FIB_COMPILE_OPTIONS}>)

if (MSVC)
    # Silence warnings
    target_compile_options(rmgr-fib-tests PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:/wd4530>") # C++ exception handler used, but unwind semantics are not enabled
endif()

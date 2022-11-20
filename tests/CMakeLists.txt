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

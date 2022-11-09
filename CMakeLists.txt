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

set(RMGR_FIB_COMPILE_OPTIONS)
if (CMAKE_COMPILER_IS_GNUCXX OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
    list(APPEND RMGR_FIB_COMPILE_OPTIONS "-Wall")
elseif (MSVC)
    list(APPEND RMGR_FIB_COMPILE_OPTIONS "/W4")
    list(APPEND RMGR_FIB_COMPILE_OPTIONS "/MP") # Multi-processor build
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

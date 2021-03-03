cmake_minimum_required(VERSION 3.10.0)

set(name CommonCode)

# Add local sources

include(src.cmake)
include(../CommonDrivers/src/src.cmake)
prepend_list(INCLUDE_PATHS "../CommonDrivers/src/")
prepend_list(SOURCE_PATHS  "../CommonDrivers/src/")

# define target

add_library(${name} STATIC ${LOCAL_SOURCE_PATHS} ${SOURCE_PATHS})
set_property(TARGET ${name} PROPERTY CXX_STANDARD 17)

target_compile_options(${name} PRIVATE ${COMMON_FLAGS})
target_compile_options(${name} INTERFACE ${COMMON_FLAGS})

separate_arguments(ADDITIONAL_FLAGS)
target_compile_options(${name} PRIVATE ${ADDITIONAL_FLAGS})
target_compile_options(${name} INTERFACE ${ADDITIONAL_FLAGS})

# Addlocal headers

target_include_directories(${name} PRIVATE ${LOCAL_INCLUDE_PATHS})
target_include_directories(${name} INTERFACE ${LOCAL_INCLUDE_PATHS})
target_include_directories(${name} PRIVATE ${INCLUDE_PATHS})
target_include_directories(${name} INTERFACE ${INCLUDE_PATHS})
target_include_directories(${name} PRIVATE .)
target_include_directories(${name} INTERFACE .)
target_include_directories(${name} PRIVATE ../CommonDrivers/unitTests)

cmake_minimum_required(VERSION 3.10.0)

set(name unitTests)

find_package(GTest REQUIRED)

# Add local sources

include(src.cmake)

# define target

add_executable(${name} ${TESTS_SRC})
target_link_libraries(${name} LINK_PRIVATE ${GTEST_LIBRARIES} CommonCode)
set_property(TARGET ${name} PROPERTY CXX_STANDARD 17)

# Addlocal headers

include_directories(${name} PRIVATE ${GTEST_INCLUDE_DIRS})
include_directories(${name} PRIVATE ../tools)

# Execute after compile

add_custom_command(TARGET ${name} POST_BUILD
  COMMAND 
    ./${name}
COMMENT "Running unit tests."
)

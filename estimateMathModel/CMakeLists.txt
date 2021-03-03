cmake_minimum_required(VERSION 3.10.0)

set(name mathModel)

find_package(GTest REQUIRED)
find_package(PythonLibs 2.7)

# Add local sources

list(APPEND TESTS_SRC
  main.cpp
  scenarios/EnginesModels.cpp
  scenarios/RobotMovement.cpp
  scenarios/RobotRotation.cpp
  ../Src/platform/PID.cpp
  ../tools/matplotlib.cpp
  ../tools/Models.cpp
)

# define target

add_executable(${name} ${TESTS_SRC})
target_link_libraries(${name} LINK_PRIVATE ${GTEST_LIBRARIES} ${PYTHON_LIBRARIES} pthread)
set_property(TARGET ${name} PROPERTY CXX_STANDARD 14)

target_compile_options(${name} PRIVATE ${COMMON_FLAGS})
target_compile_options(${name} PRIVATE
  -pthread
  -I/usr/include/python2.7
  -lpython2.7
)

# Addlocal headers

target_include_directories(${name} PRIVATE ${GTEST_INCLUDE_DIRS})
target_include_directories(${name} PRIVATE ${PYTHON_INCLUDE_DIRS})
target_include_directories(${name} PRIVATE .)
target_include_directories(${name} PRIVATE scenarios)
target_include_directories(${name} PRIVATE ../tools)
target_include_directories(${name} PRIVATE ../Src/platform)

# Create directory and copy content

execute_process(
  COMMAND
    cp -r ${CMAKE_CURRENT_SOURCE_DIR}/dataFiles ${CMAKE_CURRENT_BINARY_DIR}/../.
)

execute_process(
  COMMAND 
    mkdir ${CMAKE_CURRENT_BINARY_DIR}/../mathTestsOutputData
)

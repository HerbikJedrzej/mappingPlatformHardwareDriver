cmake_minimum_required(VERSION 3.10.0)

set(name moduleTests)

find_package(GTest REQUIRED)
find_package(PythonLibs 2.7)

# Add local sources

list(APPEND TESTS_SRC
  ../tools/matplotlib.cpp
  ../tools/Models.cpp
  ../tools/uartToFile.cpp
  main.cpp
  mocks/ModelEngine.cpp
  scenarios/NormalMode_scenario.cpp
  scenarios/TuneMovePID_scenario.cpp
  scenarios/TuneRotatePID_scenario.cpp
)

# define target

add_executable(${name} ${TESTS_SRC})
target_link_libraries(${name} LINK_PRIVATE ${GTEST_LIBRARIES} ${PYTHON_LIBRARIES} CommonCode pthread)
set_property(TARGET ${name} PROPERTY CXX_STANDARD 14)

target_compile_options(${name} PRIVATE
  -pthread
  -I/usr/include/python2.7
  -lpython2.7
)

# Addlocal headers

target_include_directories(${name} PRIVATE ${GTEST_INCLUDE_DIRS})
target_include_directories(${name} PRIVATE ${PYTHON_INCLUDE_DIRS})
target_include_directories(${name} PRIVATE mocks)
target_include_directories(${name} PRIVATE ../tools)

# Create directory

execute_process(
  COMMAND 
    mkdir ${CMAKE_CURRENT_BINARY_DIR}/../moduleTestsOutputData
)

cmake_minimum_required(VERSION 3.2)
project(vm)
#add_subdirectory(lib/googletest)

#include_directories(${DIVISION_HEADERS_DIR})
#include_directories(lib/googletest/googletest/include)

set(SOURCE_FILES cpu.cpp main.cpp ram.cpp)

add_executable(lowSpag2VM ${SOURCE_FILES})
set_target_properties(lowSpag2VM PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../")
#target_link_libraries(divider_tests division gtest)
#install(TARGETS divider_tests DESTINATION bin)

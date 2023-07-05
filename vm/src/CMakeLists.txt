cmake_minimum_required(VERSION 3.2)
project(lowSpag2VM)
#add_subdirectory(lib/googletest)

#include_directories(${DIVISION_HEADERS_DIR})
#include_directories(lib/googletest/googletest/include)

include_directories(../dependencies/minifb/include)
set(SOURCE_FILES cpu.cpp main.cpp ram.cpp)

add_executable(lowSpag2VM ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME}
    minifb
)

set_target_properties(lowSpag2VM PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../")
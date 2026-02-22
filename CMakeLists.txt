# cmake version
cmake_minimum_required(VERSION 3.12)

# project name
project(fp_text_editor2)

# cpp version
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add all .cpp .h .hpp files in src/ folder
file(GLOB_RECURSE SRC_FILES 
    "fp_text_editor2/src/*.cpp"
    # ${PROJECT_NAME} + "/src/*.cpp"
    # ${PROJECT_NAME} + "/src/*.h"
    # ${PROJECT_NAME} + "/src/*.hpp"
)

# create executable
add_executable(${PROJECT_NAME} ${SRC_FILES})

# include dirs
target_include_directories(${PROJECT_NAME} PRIVATE
    "fp_text_editor2/src/"
)

# preproc definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE 
    "PROJECT_DIR=R\"(${CMAKE_SOURCE_DIR}\\${PROJECT_NAME}\\)\""
)

# link libs for unix
if(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
endif()

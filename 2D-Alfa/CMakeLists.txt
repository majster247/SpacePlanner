cmake_minimum_required(VERSION 3.18)
project(SpacePlanner)

set(CMAKE_CXX_STANDARD 17)

# SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# FastNoiseLite - header only
include_directories(${CMAKE_SOURCE_DIR}/extern/FastNoiseLite)

# ONNX Runtime (opcjonalnie)
# set(ONNXRUNTIME_DIR "/path/to/onnxruntime")
# include_directories(${ONNXRUNTIME_DIR}/include)
# link_directories(${ONNXRUNTIME_DIR}/lib)

file(GLOB SRC
    src/*.cpp
    src/*/*.cpp
)

add_executable(spaceplanner ${SRC})

target_link_libraries(spaceplanner
    ${SDL2_LIBRARIES}
    # onnxruntime (opcjonalnie)
)

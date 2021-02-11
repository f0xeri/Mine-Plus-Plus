cmake_minimum_required(VERSION 3.16)
project(mine++)
set(CMAKE_CXX_STANDARD 17)

INCLUDE_DIRECTORIES(
        ${GL_INCLUDE_DIR}
        ${GLFW_INCLUDE_DIR})

add_subdirectory(lib/imgui)

add_executable(mine++ src/main.cpp src/Window/Window.cpp src/Window/Window.hpp src/Logger.hpp
        src/Model/Model.cpp src/Model/Model.hpp
        src/Program/Program.cpp src/Program/Program.hpp
        src/Texture/Texture.cpp src/Texture/Texture.hpp
        src/stb_image.h
        src/Camera/Camera.cpp src/Camera/Camera.h
        src/Voxels/block.cpp src/Voxels/block.hpp
        src/Voxels/Chunk.cpp src/Voxels/Chunk.hpp
        src/BlockRenderer/BlockRenderer.cpp src/BlockRenderer/BlockRenderer.hpp
        src/ChunkManager/ChunkManager.cpp src/ChunkManager/ChunkManager.hpp
        src/CubeRenderer/CubeRenderer.cpp src/CubeRenderer/CubeRenderer.hpp src/Player/Player.cpp src/Player/Player.hpp src/Controls/Controls.cpp src/Controls/Controls.h)

target_link_libraries(mine++ imgui opengl32 glfw3 glew32)

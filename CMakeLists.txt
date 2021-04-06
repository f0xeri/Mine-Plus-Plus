cmake_minimum_required(VERSION 3.16)
project(mine++)
set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_BUILD_TYPE Debug)

INCLUDE_DIRECTORIES(
        ${GL_INCLUDE_DIR}
        ${GLFW_INCLUDE_DIR})

add_subdirectory(lib/imgui)

add_executable(mine++ src/main.cpp src/Window/Window.cpp src/Window/Window.hpp src/Logger.hpp
        src/Model/Mesh.cpp src/Model/Mesh.hpp
        src/Shader/Shader.cpp src/Shader/Shader.hpp
        src/Texture/Texture.cpp src/Texture/Texture.hpp
        src/stb_image.h
        src/Camera/Camera.cpp src/Camera/Camera.h
        src/Voxels/Block.cpp src/Voxels/Block.hpp
        src/Voxels/Chunk.cpp src/Voxels/Chunk.hpp
        src/BlockRenderer/BlockRenderer.cpp src/BlockRenderer/BlockRenderer.hpp
        src/ChunkManager/ChunkManager.cpp src/ChunkManager/ChunkManager.hpp
        src/Controls/Controls.cpp src/Controls/Controls.h src/State.hpp src/GUI/GUIRenderer.cpp src/GUI/GUIRenderer.hpp src/Shadows/ShadowMap.cpp src/Shadows/ShadowMap.hpp)

target_link_libraries(mine++ imgui opengl32 glfw3 glew32)

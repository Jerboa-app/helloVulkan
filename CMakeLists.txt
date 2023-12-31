cmake_minimum_required(VERSION 3.5)
project(Jerboa)

option(RELEASE On)

set(CMAKE_CXX_STANDARD 17)
if (RELEASE)
    set(CMAKE_BUILD_TYPE Release)
else()
    set(CMAKE_BUILD_TYPE Debug)
endif()

string(TIMESTAMP TODAY "%Y-%m-%d:%H:%M:%S")
add_compile_definitions(TIMESTAMP="${TODAY}")

if (RELEASE)
    add_compile_definitions(BUILD_TYPE="Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-trapping-math -fno-rounding-math -fno-signaling-nans -fno-signed-zeros")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -O3 -Wall")
else ()
    add_compile_definitions(BUILD_TYPE="Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -g -Wall")
endif ()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_BUILD_RPATH ".")

find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIR})

SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so")
add_subdirectory(include/vendored/glm)

add_subdirectory(include/vendored/glfw)
include_directories(include/vendored/glfw/include)

include_directories(include include/vendored)
include_directories(include include/vendored/VulkanSDK/Include)

include_directories(src)

file(GLOB SRC 
    "src/*.cpp"
    "src/Renderer/*.cpp"
    "src/Shader/*.cpp"
)
if (WINDOWS)
    add_compile_definitions(WINDOWS)
    
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")

    if (RELEASE)
        # launch as windows, not console app - so cmd does not open as well
        add_link_options(-mwindows)
    endif ()

    link_directories(include/vendored/VulkanSDK/Windows/Lib)
else()
    # so nautilus etc recognise target as executable rather than .so
    add_link_options(-no-pie)
    link_directories(include/vendored/VulkanSDK/Linux/Lib)
endif()

if (VALIDATION)
    add_compile_definitions(VALIDATION)
endif()

add_executable(HelloVK ${SRC})

target_link_libraries(HelloVK glm ${Vulkan_LIBRARIES} glfw shaderc_combined)
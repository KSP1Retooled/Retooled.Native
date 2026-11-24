cmake_minimum_required(VERSION 3.24)

# The game ships x86_64 only, and a fresh macOS configure on an arm64 host would otherwise build for arm64.
if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "" FORCE)
    # Unity's macOS player runs on macOS 12, so the plug-in must not need anything newer. check-links.py verifies it.
    set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0" CACHE STRING "" FORCE)
endif()

project(RetooledNative CXX)
if(APPLE)
    enable_language(OBJCXX)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OBJCXX_STANDARD 20)

# Unity's plug-in headers are licensed for Unity-dependent projects and change with each editor, so they are read
# from the editor that builds the project instead of being copied here.
set(UNITY_PLUGIN_API "" CACHE PATH "Editor/Data/PluginAPI of the project's Unity editor")
if(NOT EXISTS "${UNITY_PLUGIN_API}/IUnityGraphics.h")
    message(FATAL_ERROR "UNITY_PLUGIN_API must point at the editor's PluginAPI folder")
endif()

add_library(RetooledNative SHARED src/Plugin.cpp)
target_include_directories(RetooledNative PRIVATE src "${UNITY_PLUGIN_API}")
set_target_properties(RetooledNative PROPERTIES PREFIX "" CXX_VISIBILITY_PRESET hidden OBJCXX_VISIBILITY_PRESET hidden)

if(WIN32 OR (UNIX AND NOT APPLE))
    # IUnityGraphicsVulkan.h needs Vulkan's headers: the Vulkan SDK's when it is installed, Khronos' otherwise. The
    # plug-in calls Vulkan through Unity's loader, so it links to nothing.
    find_package(Vulkan QUIET)
    if(NOT TARGET Vulkan::Headers)
        include(FetchContent)
        FetchContent_Declare(VulkanHeaders
            GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers.git
            GIT_TAG v1.4.309
            GIT_SHALLOW TRUE)
        FetchContent_MakeAvailable(VulkanHeaders)
    endif()
    target_sources(RetooledNative PRIVATE src/GpuVulkan.cpp)
    target_link_libraries(RetooledNative PRIVATE Vulkan::Headers)
endif()

if(WIN32)
    target_sources(RetooledNative PRIVATE src/GpuD3D.cpp)
    target_compile_definitions(RetooledNative PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
elseif(APPLE)
    target_sources(RetooledNative PRIVATE src/GpuMetal.mm)
    target_link_libraries(RetooledNative PRIVATE "-framework Metal" "-framework Foundation")
else()
    # Unity's Linux player does not load libstdc++, so the plug-in carries its own copy and keeps that copy's symbols
    # private, so that they cannot clash with another plug-in's. -z defs makes a missing library fail the link
    # instead of failing the load.
    target_link_options(RetooledNative PRIVATE -static-libstdc++ -Wl,--exclude-libs,ALL -Wl,-z,defs)
endif()

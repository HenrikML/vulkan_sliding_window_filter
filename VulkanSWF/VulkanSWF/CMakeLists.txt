﻿
cmake_minimum_required(VERSION 3.25.0)
set(CMAKE_CXX_STANDARD 17)

set(NAME VulkanSWF)

project(${NAME} VERSION 0.1.0)

find_package( Vulkan REQUIRED COMPONENTS glslc )

if (NOT Vulkan_FOUND)
    message( FATAL_ERROR "Missing library: Vulkan")
else()
    message( "Found Vulkan library")
    message( "    version: ${Vulkan_VERSION}")
    message( "    path: ${Vulkan_LIBRARIES}")
endif()

if ( ${Vulkan_VERSION} VERSION_LESS "1.3.256" )
    message( FATAL_ERROR "Minimum required Vulkan version for C++ modules is 1.3.256. "
        "Found ${Vulkan_VERSION}.")
endif()

if (NOT Vulkan_glslc_FOUND)
    message( FATAL_ERROR "Missing component: glslc")
else()
    message( "Found glslc")
    message( "    path: ${Vulkan_GLSLC_EXECUTABLE}")
endif()

file(GLOB_RECURSE SHADERS ${PROJECT_SOURCE_DIR}/shaders/*.comp)
foreach(SHADER ${SHADERS})
    get_filename_component(FILE_NAME ${SHADER} NAME)
    message( "Compiling ${SHADER}")
    add_custom_command(
        OUTPUT ${PROJECT_BINARY_DIR}/shaders/${FILE_NAME}.spv
        DEPENDS ${SHADER}
        COMMAND ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} -V ${SHADER} -o ${PROJECT_BINARY_DIR}/shaders/${FILE_NAME}.spv
        )
    #file(COPY ${PROJECT_SOURCE_DIR}/shaders/${FILE_NAME}.spv DESTINATION shaders)
    list(APPEND SPIRV_BINARY_FILES shaders/${FILE_NAME}.spv)
endforeach(SHADER)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(COPY images/input.jpg DESTINATION img)

add_executable(${PROJECT_NAME} ${SOURCES})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build")

if (WIN32)
    target_include_directories(${PROJECT_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/external
    ${PROJECT_SOURCE_DIR}/shaders
    ${Vulkan_INCLUDE_DIRS}
    )

    target_link_directories(${PROJECT_NAME} PUBLIC ${Vulkan_LIBRARIES})

    target_link_libraries(${PROJECT_NAME} PUBLIC Vulkan::Vulkan)
elseif (UNIX)
    target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src)
    target_link_libraries(${PROJECT_NAME} PUBLIC Vulkan::Vulkan Vulkan::Headers)
endif()

add_custom_target(shaders ALL DEPENDS ${SPIRV_BINARY_FILES})
add_dependencies(${NAME} shaders)
cmake_minimum_required(VERSION 3.21)

# Отключаем автоматическое подключение vcpkg
# только в проектах, создаваемых этой CMake-сборкой.
set(CMAKE_VS_GLOBALS "VcpkgEnabled=false")

project(CatRoom LANGUAGES CXX)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

add_executable(CatRoom
    Main.cpp
    Util.cpp
)

target_compile_features(CatRoom PRIVATE cxx_std_14)

set_target_properties(CatRoom PROPERTIES
    CXX_EXTENSIONS OFF
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:CatRoom>"
)

target_link_libraries(CatRoom PRIVATE
    OpenGL::GL
    Threads::Threads
)

if(WIN32)
    # Подготовленные библиотеки Windows предназначены для MSVC x64.
    if(NOT MSVC OR NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
        message(FATAL_ERROR "Use MSVC with the x64 platform.")
    endif()

    set(GLFW_DIR
        "${CMAKE_CURRENT_SOURCE_DIR}/packages/glfw.3.4.0/build/native")
    set(GLEW_DIR
        "${CMAKE_CURRENT_SOURCE_DIR}/packages/glew-2.2.0.2.2.0.1/build/native")
    set(FT_DIR
        "${CMAKE_CURRENT_SOURCE_DIR}/external/freetype")

    target_include_directories(CatRoom PRIVATE
        "${GLFW_DIR}/include"
        "${GLEW_DIR}/include"
        "${FT_DIR}/include"
    )

    target_compile_definitions(CatRoom PRIVATE GLFW_DLL)
    target_compile_options(CatRoom PRIVATE /utf-8)

    set_property(TARGET CatRoom PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
    )

    target_link_libraries(CatRoom PRIVATE
        "${GLFW_DIR}/lib/dynamic/v143/x64/glfw3dll.lib"
        "${GLEW_DIR}/lib/Release/x64/glew32.lib"
        debug "${FT_DIR}/lib/Debug/freetyped.lib"
        optimized "${FT_DIR}/lib/Release/freetype.lib"
    )

    # Копируем DLL рядом с программой.
    add_custom_command(TARGET CatRoom POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${GLFW_DIR}/bin/dynamic/v143/x64/glfw3.dll"
            "${GLEW_DIR}/bin/Release/x64/glew32.dll"
            "$<TARGET_FILE_DIR:CatRoom>"
        VERBATIM
    )
else()
    # На macOS используем библиотеки, установленные для этой системы.
    find_package(glfw3 3.4 CONFIG REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(Freetype REQUIRED)

    target_link_libraries(CatRoom PRIVATE
        glfw
        GLEW::GLEW
        Freetype::Freetype
    )
endif()

# Обновляем ресурсы при каждой сборке программы.
add_custom_target(CatRoomAssets
    COMMAND "${CMAKE_COMMAND}" -E make_directory
        "$<TARGET_FILE_DIR:CatRoom>"

    COMMAND "${CMAKE_COMMAND}" -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/res"
        "$<TARGET_FILE_DIR:CatRoom>/res"

    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/basic.vert"
        "${CMAKE_CURRENT_SOURCE_DIR}/basic.frag"
        "$<TARGET_FILE_DIR:CatRoom>"

    VERBATIM
)

add_dependencies(CatRoom CatRoomAssets)
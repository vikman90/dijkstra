include(FetchContent)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    URL https://github.com/glfw/glfw/archive/refs/tags/3.4.zip
    DOWNLOAD_EXTRACT_TIMESTAMP ON
)

FetchContent_Declare(
    imgui
    URL https://github.com/ocornut/imgui/archive/refs/tags/v1.91.8.zip
    DOWNLOAD_EXTRACT_TIMESTAMP ON
)

FetchContent_MakeAvailable(glfw imgui)

# ImGui library target
add_library(imgui-lib STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui-lib SYSTEM PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui-lib PUBLIC
    glfw
    OpenGL::GL
)

# Suppress third-party warnings in ImGui
if(NOT MSVC)
    target_compile_options(imgui-lib PRIVATE -w)
endif()

# GUI Application Executable
add_executable(dijkstra-gui
    main.cpp
    app.cpp
    canvas.cpp
    presets.cpp
    step_tracer.cpp
)

target_link_libraries(dijkstra-gui PRIVATE
    dijkstra-lib
    imgui-lib
    dijkstra-compiler-flags
)

if(NOT MSVC)
    target_compile_options(dijkstra-gui PRIVATE -Wno-old-style-cast)
endif()

target_include_directories(dijkstra-gui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

install(TARGETS dijkstra-gui
    RUNTIME DESTINATION bin
)

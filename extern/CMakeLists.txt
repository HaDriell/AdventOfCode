# This file is here because some extern projects are not using CMake 
# I can't version the CMakeLists.txt file inside the git submodule tree

# GLAD
add_subdirectory(GLAD)

# GLFW
add_subdirectory(GLFW)

# ImGui
add_library(imgui STATIC
    "ImGui/imgui.cpp"
    "ImGui/imgui_demo.cpp"
    "ImGui/imgui_draw.cpp"
    "ImGui/imgui_tables.cpp"
    "ImGui/imgui_widgets.cpp"
    "ImGui/backends/imgui_impl_glfw.cpp"
    "ImGui/backends/imgui_impl_opengl3.cpp"
    "ImGui/misc/cpp/imgui_stdlib.cpp"
)
target_link_libraries(imgui PUBLIC glfw)

target_include_directories(imgui
PUBLIC
    "ImGui"
)
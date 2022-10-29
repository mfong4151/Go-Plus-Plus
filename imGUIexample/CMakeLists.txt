cmake_minimum_required (VERSION 3.8)

add_executable (imGUIexample "main.cpp""UseImGui.h"  "UseImGui.cpp")

find_package(glad CONFIG REQUIRED)
target_link_libraries(imGUIexample PRIVATE glad::glad)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(imGUIexample PRIVATE glfw)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(imGUIexample PRIVATE imgui::imgui)

find_path(STB_INCLUDE_DIRS "stb_c_lexer.h")
target_include_directories(imGUIexample PRIVATE ${STB_INCLUDE_DIRS})

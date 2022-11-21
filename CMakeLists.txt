cmake_minimum_required(VERSION 3.22)
project(maple)

set(CMAKE_CXX_STANDARD 23)

find_package(fmt CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

add_library(maple SHARED
        src/main.cpp src/dinput8.cpp src/rendering.cpp
        src/hooking/bootstrap.cpp src/hooking/packets.cpp src/hooking/string_pool.cpp
        src/hooking/graphics.cpp
        src/util/hooking/guard_pages.cpp
        src/util/logging/console_logger_sink.cpp src/util/logging/file_logger_sink.cpp
        src/util/logging/logger.cpp src/util/logging/stack_trace.cpp
        src/util/rendering/imgui_impl_dx8.cpp)
target_link_libraries(maple PRIVATE dbghelp.dll fmt::fmt fmt::fmt-header-only imgui::imgui)
target_compile_definitions(maple PRIVATE -D_CRT_SECURE_NO_WARNINGS)

set_target_properties(maple PROPERTIES OUTPUT_NAME dinput8)
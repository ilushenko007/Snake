# =====================================================================
# CMakeLists.txt для игры "Змейка"
# Курсовой проект, 1 курс, кафедра АСОИУ, АГТУ
# =====================================================================
cmake_minimum_required(VERSION 3.25)

project(Snake
    VERSION     1.0.0
    LANGUAGES   CXX
    DESCRIPTION "Snake game (SFML 3.1)"
)

set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# ---------------------------------------------------------------------
# CMakeRC — встраивание ресурсов (шрифт) прямо в EXE.
# ---------------------------------------------------------------------
# cmake/cmrc.cmake добавляет функцию cmrc_add_resource_library().
# Она «компилирует» бинарные файлы в .cpp/.obj и линкует в целевой бинарь.
# После этого папка assets/ на диске рядом с Snake.exe НЕ нужна.
include(cmake/cmrc.cmake)

cmrc_add_resource_library(SnakeResources
    NAMESPACE snake_res
    WHENCE    "${CMAKE_SOURCE_DIR}"
    assets/DejaVuSans.ttf
)

# ---------------------------------------------------------------------
# Поиск SFML 3
# ---------------------------------------------------------------------
find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

# ---------------------------------------------------------------------
# Исходники
# ---------------------------------------------------------------------
set(SNAKE_SOURCES
    src/main.cpp
    src/Game.cpp
    src/Snake.cpp
    src/Fruit.cpp
    src/Field.cpp
    src/ScoreManager.cpp
    src/Menu.cpp
)

set(SNAKE_HEADERS
    include/Config.hpp
    include/Game.hpp
    include/Snake.hpp
    include/Fruit.hpp
    include/Field.hpp
    include/ScoreManager.hpp
    include/Menu.hpp
)

add_executable(Snake ${SNAKE_SOURCES} ${SNAKE_HEADERS})

target_include_directories(Snake PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(Snake PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    SnakeResources          # <-- встроенные ресурсы
)

# ---------------------------------------------------------------------
# MSVC: предупреждения, UTF-8, статическая линковка CRT (/MT)
# ---------------------------------------------------------------------
if(MSVC)
    target_compile_options(Snake PRIVATE
        /W4
        /utf-8
        /permissive-
    )
    # Статическая CRT нужна при сборке с triplet x64-windows-static,
    # чтобы VCRUNTIME*.dll тоже зашился в EXE (см. CMakePresets.json).
    set_property(TARGET Snake PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
endif()

# ПРИМЕЧАНИЕ: команда copy_directory assets/ удалена, так как шрифт
# теперь вшит в EXE через cmrc. Папка assets/ на диске больше не нужна.

# ---------------------------------------------------------------------
message(STATUS "")
message(STATUS "================ Snake ================")
message(STATUS "  Версия:      ${PROJECT_VERSION}")
message(STATUS "  SFML:        ${SFML_VERSION}")
message(STATUS "  Компилятор:  ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  Сборка в:    ${CMAKE_BINARY_DIR}")
message(STATUS "  Ресурсы:     встроены в EXE (cmrc)")
message(STATUS "=======================================")
message(STATUS "")

# =====================================================================
# CMakeLists.txt для игры "Змейка"
# Курсовой проект, 1 курс, кафедра АСОИУ, АГТУ
# =====================================================================
# Минимальная версия CMake. Используем 3.25+, потому что наши CMakePresets.json
# написаны по схеме версии 6, а она требует CMake 3.25+. В Visual Studio 2026
# встроен CMake 4.1+, так что эта планка пройдёт с запасом.
cmake_minimum_required(VERSION 3.25)

# Название проекта и язык. VERSION ставится для удобства (можно увидеть
# в окне About / в свойствах исполняемого файла).
project(Snake
    VERSION     1.0.0
    LANGUAGES   CXX
    DESCRIPTION "Snake game (SFML 3.1)"
)

# Используем стандарт C++17. Этого достаточно для всего, что мы пишем,
# и его уверенно поддерживает актуальный компилятор MSVC из Visual Studio 2026.
set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

# Чтобы исполняемый файл и копируемые ресурсы всегда лежали в одной
# папке (bin/), независимо от того, какой конфигурацией собираем
# (Debug / Release). Это упрощает запуск из любой IDE.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# ---------------------------------------------------------------------
# Поиск библиотеки SFML 3.1 (устанавливаем её через vcpkg).
# ---------------------------------------------------------------------
# vcpkg сам подсунет нужный CMAKE_TOOLCHAIN_FILE и CMake найдёт SFML
# командой find_package. Нам нужны 3 компонента:
#   Graphics  - окно, спрайты, шрифты, текст;
#   Window    - обработка событий клавиатуры;
#   System    - sf::Clock, sf::Time, базовые типы.
find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

# ---------------------------------------------------------------------
# Собираем список исходников и заголовков.
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

# Создаём исполняемый файл.
add_executable(Snake ${SNAKE_SOURCES} ${SNAKE_HEADERS})

# Папка include/ должна быть видна препроцессору, чтобы #include "Game.hpp"
# и т.д. находил наши заголовочные файлы.
target_include_directories(Snake PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# Подключаем SFML 3 (в SFML 3 цели названы как SFML::Graphics и т.д.,
# с заглавными буквами - это отличие от SFML 2).
target_link_libraries(Snake PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)

# Для MSVC включаем строгие предупреждения и кодировку UTF-8 в исходниках
# (наши .cpp/.hpp написаны в UTF-8, в них есть кириллица в строках).
if(MSVC)
    target_compile_options(Snake PRIVATE
        /W4         # уровень предупреждений
        /utf-8      # исходники в UTF-8
        /permissive-
    )

    # ---------------------------------------------------------------------
    # Статическая линковка CRT (/MT вместо /MD).
    # ---------------------------------------------------------------------
    # При сборке с triplet x64-windows-static vcpkg собирает SFML статически.
    # Чтобы и CRT (VCRUNTIME, MSVCP) тоже зашилась в EXE, нужно переключить
    # runtime-библиотеку с динамической (/MD) на статическую (/MT).
    # Без этой строчки линкер выдаст ошибку о несовместимости библиотек.
    #
    # В обычных пресетах (x64-windows) этот параметр не влияет на результат -
    # там SFML динамическая и CRT динамическая, всё согласовано.
    set_property(TARGET Snake PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )

    # На Windows запускаем как обычное консольное приложение - так удобнее
    # видеть отладочный вывод. Если захотите окно без консоли, замените
    # на WIN32 в add_executable выше и подцепите /SUBSYSTEM:WINDOWS.
endif()

# ---------------------------------------------------------------------
# Копирование папки assets/ рядом с исполняемым файлом.
# ---------------------------------------------------------------------
# Игра грузит шрифт из assets/arial.ttf. После сборки папка assets/
# должна оказаться рядом с Snake.exe, иначе шрифт не найдётся.
add_custom_command(TARGET Snake POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:Snake>/assets"
    COMMENT "Копирую папку assets/ рядом с Snake.exe"
)

# ---------------------------------------------------------------------
# Удобный вывод после конфигурации - чтобы новичок видел, что нашлось.
# ---------------------------------------------------------------------
message(STATUS "")
message(STATUS "================ Snake ================")
message(STATUS "  Версия:      ${PROJECT_VERSION}")
message(STATUS "  SFML:        ${SFML_VERSION}")
message(STATUS "  Компилятор:  ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  Сборка в:    ${CMAKE_BINARY_DIR}")
message(STATUS "=======================================")
message(STATUS "")

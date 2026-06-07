// =====================================================================
// Config.hpp
// ---------------------------------------------------------------------
// Здесь собраны ВСЕ настраиваемые параметры игры в одном месте.
// =====================================================================
#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/System/String.hpp>
#include <cmrc/cmrc.hpp>
#include <cstdint>
#include <stdexcept>
#include <string>

// Объявляем виртуальную файловую систему ресурсов, скомпилированных
// командой cmrc_add_resource_library(SnakeResources NAMESPACE snake_res ...).
CMRC_DECLARE(snake_res);

namespace cfg {

// ---------------------------------------------------------------------
// Размеры игрового поля.
// ---------------------------------------------------------------------
constexpr int CELL_SIZE = 24;
constexpr int COLS      = 25;
constexpr int ROWS      = 20;
constexpr int HUD_HEIGHT = 40;
constexpr int WINDOW_WIDTH  = COLS * CELL_SIZE;
constexpr int WINDOW_HEIGHT = ROWS * CELL_SIZE + HUD_HEIGHT;

// ---------------------------------------------------------------------
// Параметры геймплея.
// ---------------------------------------------------------------------
constexpr int   SNAKE_START_LENGTH       = 4;
constexpr float INITIAL_STEPS_PER_SECOND = 8.0f;
constexpr float MAX_STEPS_PER_SECOND     = 22.0f;
constexpr float SPEED_INCREMENT          = 0.4f;
constexpr int   FRUIT_SCORE              = 10;

// ---------------------------------------------------------------------
// Цвета (палитра).
// ---------------------------------------------------------------------
inline const sf::Color COLOR_BACKGROUND   {  20,  20,  30 };
inline const sf::Color COLOR_HUD          {  10,  10,  20 };
inline const sf::Color COLOR_GRID         {  35,  35,  50 };
inline const sf::Color COLOR_BORDER       { 200,  60,  60 };
inline const sf::Color COLOR_SNAKE_HEAD   {  80, 200, 120 };
inline const sf::Color COLOR_SNAKE_BODY   {  50, 160,  90 };
inline const sf::Color COLOR_FRUIT        { 230,  90,  90 };
inline const sf::Color COLOR_TEXT         { 230, 230, 230 };
inline const sf::Color COLOR_TEXT_DIM     { 150, 150, 160 };
inline const sf::Color COLOR_TEXT_ACCENT  { 255, 200,  90 };

// ---------------------------------------------------------------------
// Прочие файлы.
// ---------------------------------------------------------------------
// FONT_PATH убран — шрифт теперь вшит в EXE через cmrc.
// Используйте loadFontFromResources() для загрузки шрифта.
inline const std::string SCORES_FILE = "scores.txt";
constexpr int MAX_SCORES = 10;
inline const std::string WINDOW_TITLE = "Snake - Змейка";

// ---------------------------------------------------------------------
// Загрузка встроенного шрифта из ресурсов (cmrc).
// ---------------------------------------------------------------------
// Вместо openFromFile("assets/DejaVuSans.ttf") вызываем эту функцию.
// Она открывает виртуальную ФС cmrc, находит шрифт по пути
// "assets/DejaVuSans.ttf" и загружает его через openFromMemory().
inline void loadFontFromResources(sf::Font& font) {
    auto fs   = cmrc::snake_res::get_filesystem();
    auto file = fs.open("assets/DejaVuSans.ttf");
    // openFromMemory принимает указатель и размер.
    // Данные живут в статической памяти EXE — не освобождаются.
    if (!font.openFromMemory(file.begin(), file.size())) {
        throw std::runtime_error(
            "Не удалось загрузить встроенный шрифт DejaVuSans.ttf."
        );
    }
}

// ---------------------------------------------------------------------
// Вспомогательная функция для корректного отображения кириллицы.
// ---------------------------------------------------------------------
inline sf::String toSfString(const std::string& s) {
    return sf::String::fromUtf8(s.begin(), s.end());
}

inline sf::String toSfString(const char* s) {
    std::string tmp(s);
    return sf::String::fromUtf8(tmp.begin(), tmp.end());
}

} // namespace cfg

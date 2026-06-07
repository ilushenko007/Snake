// =====================================================================
// Field.cpp
// =====================================================================
#include "Field.hpp"
#include "Config.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/VertexArray.hpp>

// ---------------------------------------------------------------------
// Отрисовка фона, сетки и границы.
// ---------------------------------------------------------------------
void Field::draw(sf::RenderTarget& target) const {
    // 1) Фон самого игрового поля (под HUD рисуем отдельно).
    sf::RectangleShape bg({
        static_cast<float>(cfg::WINDOW_WIDTH),
        static_cast<float>(cfg::ROWS * cfg::CELL_SIZE)
    });
    bg.setPosition({ 0.0f, static_cast<float>(cfg::HUD_HEIGHT) });
    bg.setFillColor(cfg::COLOR_BACKGROUND);
    target.draw(bg);

    // 2) Сетка. Рисуем линиями для эффективности
    //    (VertexArray режима sf::PrimitiveType::Lines).
    sf::VertexArray grid(sf::PrimitiveType::Lines);

    // Вертикальные линии.
    for (int c = 0; c <= cfg::COLS; ++c) {
        const float x = static_cast<float>(c * cfg::CELL_SIZE);
        grid.append(sf::Vertex{ { x, static_cast<float>(cfg::HUD_HEIGHT) }, cfg::COLOR_GRID });
        grid.append(sf::Vertex{ { x, static_cast<float>(cfg::HUD_HEIGHT + cfg::ROWS * cfg::CELL_SIZE) }, cfg::COLOR_GRID });
    }
    // Горизонтальные линии.
    for (int r = 0; r <= cfg::ROWS; ++r) {
        const float y = static_cast<float>(cfg::HUD_HEIGHT + r * cfg::CELL_SIZE);
        grid.append(sf::Vertex{ { 0.0f, y }, cfg::COLOR_GRID });
        grid.append(sf::Vertex{ { static_cast<float>(cfg::WINDOW_WIDTH), y }, cfg::COLOR_GRID });
    }
    target.draw(grid);

    // 3) Граница поля (красная рамка) - чтобы было визуально понятно,
    //    куда нельзя заезжать.
    sf::RectangleShape border({
        static_cast<float>(cfg::COLS * cfg::CELL_SIZE) - 2.0f,
        static_cast<float>(cfg::ROWS * cfg::CELL_SIZE) - 2.0f
    });
    border.setPosition({ 1.0f, static_cast<float>(cfg::HUD_HEIGHT) + 1.0f });
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineColor(cfg::COLOR_BORDER);
    border.setOutlineThickness(2.0f);
    target.draw(border);
}

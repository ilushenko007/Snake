// =====================================================================
// Fruit.cpp
// =====================================================================
#include "Fruit.hpp"
#include "Config.hpp"

#include <SFML/Graphics/CircleShape.hpp>

// ---------------------------------------------------------------------
// Конструктор. Инициализируем генератор случайных чисел случайным
// значением из std::random_device - это даёт разное расположение
// фруктов от запуска к запуску.
// ---------------------------------------------------------------------
Fruit::Fruit(const Snake& snake)
    : m_cell{ 0, 0 }
    , m_rng(std::random_device{}())
{
    respawn(snake);
}

// ---------------------------------------------------------------------
// Случайное перемещение фрукта на свободную клетку.
// Алгоритм: подбираем случайные координаты, пока не попадём в клетку,
// не занятую телом змейки.
// На малом поле это занимает очень мало попыток.
// ---------------------------------------------------------------------
void Fruit::respawn(const Snake& snake) {
    // std::uniform_int_distribution даёт целые числа в заданном
    // диапазоне ВКЛЮЧИТЕЛЬНО.
    std::uniform_int_distribution<int> distX(0, cfg::COLS - 1);
    std::uniform_int_distribution<int> distY(0, cfg::ROWS - 1);

    Cell candidate{ 0, 0 };
    do {
        candidate.x = distX(m_rng);
        candidate.y = distY(m_rng);
    } while (snake.occupies(candidate));

    m_cell = candidate;
}

// ---------------------------------------------------------------------
// Рисуем фрукт в виде кружка, вписанного в клетку.
// ---------------------------------------------------------------------
void Fruit::draw(sf::RenderTarget& target) const {
    const float pad    = 3.0f;
    const float radius = (static_cast<float>(cfg::CELL_SIZE) - 2.0f * pad) / 2.0f;

    sf::CircleShape circle(radius);
    circle.setFillColor(cfg::COLOR_FRUIT);

    const float px = static_cast<float>(m_cell.x * cfg::CELL_SIZE) + pad;
    const float py = static_cast<float>(m_cell.y * cfg::CELL_SIZE) + pad + cfg::HUD_HEIGHT;
    circle.setPosition({ px, py });
    target.draw(circle);
}

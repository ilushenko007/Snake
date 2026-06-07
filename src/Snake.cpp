// =====================================================================
// Snake.cpp
// =====================================================================
#include "Snake.hpp"
#include "Config.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

// ---------------------------------------------------------------------
// Конструктор: просто вызываем reset(), чтобы не дублировать код
// инициализации. reset() мы и так будем вызывать при каждом рестарте.
// ---------------------------------------------------------------------
Snake::Snake() {
    reset();
}

// ---------------------------------------------------------------------
// Привести змейку к начальному состоянию.
// ---------------------------------------------------------------------
void Snake::reset() {
    m_body.clear();

    // Размещаем змейку горизонтально в центре поля, головой смотрит
    // вправо. Голова - первый элемент дека, хвост - последний.
    const int startX = cfg::COLS / 2;
    const int startY = cfg::ROWS / 2;
    for (int i = 0; i < cfg::SNAKE_START_LENGTH; ++i) {
        // i=0 - голова, дальше уходят влево.
        m_body.push_back(Cell{ startX - i, startY });
    }

    m_direction        = Direction::Right;
    m_pendingDirection = Direction::Right;
}

// ---------------------------------------------------------------------
// Запрос смены направления. Реальная смена пройдёт в step() - так мы
// гарантируем, что игрок не успеет нажать "вверх" + "влево" за один
// кадр и не развернёт голову на 180°.
// ---------------------------------------------------------------------
void Snake::setDirection(Direction newDirection) {
    // Запрещаем разворот на 180° относительно ТЕКУЩЕГО направления
    // (а не относительно отложенного - так логичнее для игрока).
    if (isOpposite(newDirection, m_direction)) {
        return;
    }
    m_pendingDirection = newDirection;
}

// ---------------------------------------------------------------------
// Один игровой шаг.
// ---------------------------------------------------------------------
void Snake::step(bool grow) {
    // Применяем отложенный поворот.
    m_direction = m_pendingDirection;

    // Считаем новые координаты головы.
    Cell newHead = m_body.front();
    switch (m_direction) {
        case Direction::Up:    newHead.y -= 1; break;
        case Direction::Down:  newHead.y += 1; break;
        case Direction::Left:  newHead.x -= 1; break;
        case Direction::Right: newHead.x += 1; break;
    }

    // Добавляем новую голову в начало дека.
    m_body.push_front(newHead);

    // Если фрукт НЕ съеден - убираем хвост (длина не меняется).
    // Если съеден - оставляем хвост на месте (змейка выросла на 1).
    if (!grow) {
        m_body.pop_back();
    }
}

// ---------------------------------------------------------------------
// Заглянуть на одну клетку вперёд от головы - без изменения состояния.
// Направление берём "отложенное" (куда реально шагнём на этом кадре).
// ---------------------------------------------------------------------
Cell Snake::peekNextHead() const {
    Cell next = m_body.front();
    switch (m_pendingDirection) {
        case Direction::Up:    next.y -= 1; break;
        case Direction::Down:  next.y += 1; break;
        case Direction::Left:  next.x -= 1; break;
        case Direction::Right: next.x += 1; break;
    }
    return next;
}

// ---------------------------------------------------------------------
// Проверка самопересечения: совпадает ли голова с каким-то сегментом
// тела (начиная со второго - сама с собой голова всегда совпадает).
// ---------------------------------------------------------------------
bool Snake::collidesWithSelf() const {
    const Cell& h = m_body.front();
    for (std::size_t i = 1; i < m_body.size(); ++i) {
        if (m_body[i] == h) {
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------
// Проверка, занята ли клетка телом змейки (нужно при генерации фрукта).
// ---------------------------------------------------------------------
bool Snake::occupies(const Cell& cell) const {
    for (const Cell& seg : m_body) {
        if (seg == cell) return true;
    }
    return false;
}

// ---------------------------------------------------------------------
// Отрисовка. Каждый сегмент - прямоугольник в клетке поля.
// Голову подсвечиваем чуть другим цветом, чтобы было понятно, куда
// смотрит змейка.
// ---------------------------------------------------------------------
void Snake::draw(sf::RenderTarget& target) const {
    // Размер квадратика сегмента чуть меньше клетки - так появляется
    // визуальный зазор и сегменты не сливаются в "одну колбасу".
    const float pad = 2.0f;
    sf::RectangleShape rect({
        static_cast<float>(cfg::CELL_SIZE) - 2.0f * pad,
        static_cast<float>(cfg::CELL_SIZE) - 2.0f * pad
    });

    for (std::size_t i = 0; i < m_body.size(); ++i) {
        const Cell& seg = m_body[i];
        // Y-координата начинается ниже на HUD_HEIGHT, потому что
        // сверху мы отрисовываем полосу со счётом.
        const float px = static_cast<float>(seg.x * cfg::CELL_SIZE) + pad;
        const float py = static_cast<float>(seg.y * cfg::CELL_SIZE) + pad + cfg::HUD_HEIGHT;
        rect.setPosition({ px, py });
        rect.setFillColor(i == 0 ? cfg::COLOR_SNAKE_HEAD : cfg::COLOR_SNAKE_BODY);
        target.draw(rect);
    }
}

// ---------------------------------------------------------------------
// Вспомогательная функция: противоположные ли направления?
// ---------------------------------------------------------------------
bool Snake::isOpposite(Direction a, Direction b) {
    return (a == Direction::Up    && b == Direction::Down)  ||
           (a == Direction::Down  && b == Direction::Up)    ||
           (a == Direction::Left  && b == Direction::Right) ||
           (a == Direction::Right && b == Direction::Left);
}

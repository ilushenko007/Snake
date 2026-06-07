// =====================================================================
// Menu.hpp
// ---------------------------------------------------------------------
// Стартовое меню и экран таблицы рекордов.
// Игрок видит три пункта:
//   1. Новая игра
//   2. Рекорды
//   3. Выход
// Управление: стрелки Вверх/Вниз - выбор пункта, Enter - подтверждение.
// =====================================================================
#pragma once

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

class ScoreManager;   // forward-declaration: знаем, что класс есть,
                      // полное определение нам тут не требуется.

// Какой пункт меню выбрал игрок (или "ещё не выбрал").
enum class MenuChoice {
    None,        // выбор ещё не сделан
    StartGame,
    ShowScores,
    Quit
};

class Menu {
public:
    explicit Menu(const sf::Font& font);

    // Сдвинуть выделение вниз/вверх по списку пунктов.
    void moveDown();
    void moveUp();

    // Вернуть выбор, который сделал игрок (или None, если он ещё
    // не нажимал Enter).
    MenuChoice confirm() const;

    // Нарисовать стартовую заставку (титульный экран с подписью автора).
    void drawTitleScreen(sf::RenderTarget& target) const;

    // Нарисовать главное меню.
    void drawMain(sf::RenderTarget& target) const;

    // Нарисовать экран таблицы рекордов.
    void drawScores(sf::RenderTarget& target, const ScoreManager& scores) const;

    // Текущий индекс выделенного пункта (0..2).
    int selectedIndex() const { return m_selected; }

private:
    const sf::Font& m_font;
    int             m_selected = 0;  // 0 - "Новая игра", 1 - "Рекорды", 2 - "Выход"
};

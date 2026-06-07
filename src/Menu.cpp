// =====================================================================
// Menu.cpp
// =====================================================================
#include "Menu.hpp"
#include "Config.hpp"
#include "ScoreManager.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

#include <array>
#include <string>

namespace {

// Названия пунктов меню. constexpr-массив - удобно перебирать в цикле.
const std::array<const char*, 3> kItems = {
    "Новая игра",
    "Рекорды",
    "Выход"
};

constexpr int kItemCount = 3;

// Помощник: центрировать sf::Text по горизонтали в окне.
void centerHorizontally(sf::Text& text, float y) {
    const auto bounds = text.getLocalBounds();
    // В SFML 3 FloatRect имеет поля .position и .size.
    const float x = (static_cast<float>(cfg::WINDOW_WIDTH) - bounds.size.x) * 0.5f - bounds.position.x;
    text.setPosition({ x, y });
}

} // namespace

// ---------------------------------------------------------------------
Menu::Menu(const sf::Font& font)
    : m_font(font)
{}

void Menu::moveDown() {
    m_selected = (m_selected + 1) % kItemCount;
}

void Menu::moveUp() {
    m_selected = (m_selected - 1 + kItemCount) % kItemCount;
}

MenuChoice Menu::confirm() const {
    switch (m_selected) {
        case 0: return MenuChoice::StartGame;
        case 1: return MenuChoice::ShowScores;
        case 2: return MenuChoice::Quit;
        default: return MenuChoice::None;
    }
}

// ---------------------------------------------------------------------
// Стартовая заставка (титульный экран с подписью автора).
// Показывается один раз при запуске игры. Любая клавиша - в главное меню.
// ---------------------------------------------------------------------
void Menu::drawTitleScreen(sf::RenderTarget& target) const {
    // Фон.
    sf::RectangleShape bg({
        static_cast<float>(cfg::WINDOW_WIDTH),
        static_cast<float>(cfg::WINDOW_HEIGHT)
    });
    bg.setFillColor(cfg::COLOR_BACKGROUND);
    target.draw(bg);

    // Сверху - крупная надпись "КУРСОВОЙ ПРОЕКТ" (выделена жёлтым).
    sf::Text headline(m_font, cfg::toSfString("КУРСОВОЙ ПРОЕКТ"), 36);
    headline.setFillColor(cfg::COLOR_TEXT_ACCENT);
    headline.setStyle(sf::Text::Bold);
    centerHorizontally(headline, 60.0f);
    target.draw(headline);

    // Дальше - белые строки обычным шрифтом, с пустыми строками для отступов.
    // Пустая строка ("") рисуется как пробел - это простой способ сделать
    // вертикальный отступ между блоками текста.
    const std::array<const char*, 7> lines = {
        "на тему:",
        "Разработка игры «Змейка»",
        "по дисциплине «Основы алгоритмизации",
        "и программирование»",
        "",
        "Проект выполнен",
    "студенткой группы ДИНРБ11 Илюшенко Викторией"
    };

    float y = 140.0f;
    const float stepY = 44.0f;
    for (const auto* str : lines) {
        if (std::string(str).empty()) {
            y += stepY;   // пустая строка = просто отступ
            continue;
        }
        sf::Text line(m_font, cfg::toSfString(str), 22);
        line.setFillColor(cfg::COLOR_TEXT);
        centerHorizontally(line, y);
        target.draw(line);
        y += stepY;
    }

    // Подсказка внизу.
    sf::Text hint(m_font,
                  cfg::toSfString("Нажмите любую клавишу для продолжения"),
                  18);
    hint.setFillColor(cfg::COLOR_TEXT_DIM);
    centerHorizontally(hint, static_cast<float>(cfg::WINDOW_HEIGHT) - 60.0f);
    target.draw(hint);
}

// ---------------------------------------------------------------------
// Главное меню.
// ---------------------------------------------------------------------
void Menu::drawMain(sf::RenderTarget& target) const {
    // Фон.
    sf::RectangleShape bg({
        static_cast<float>(cfg::WINDOW_WIDTH),
        static_cast<float>(cfg::WINDOW_HEIGHT)
    });
    bg.setFillColor(cfg::COLOR_BACKGROUND);
    target.draw(bg);

    // Заголовок.
    sf::Text title(m_font, cfg::toSfString("ЗМЕЙКА"), 64);
    title.setFillColor(cfg::COLOR_TEXT_ACCENT);
    title.setStyle(sf::Text::Bold);
    centerHorizontally(title, 80.0f);
    target.draw(title);

    // Подпись.
    sf::Text subtitle(m_font, cfg::toSfString("C++ / SFML 3.1 / CMake"), 18);
    subtitle.setFillColor(cfg::COLOR_TEXT_DIM);
    centerHorizontally(subtitle, 160.0f);
    target.draw(subtitle);

    // Пункты меню.
    const float firstItemY = 250.0f;
    const float itemStep   = 60.0f;
    for (int i = 0; i < kItemCount; ++i) {
        sf::Text item(m_font, cfg::toSfString(kItems[i]), 32);
        if (i == m_selected) {
            item.setFillColor(cfg::COLOR_TEXT_ACCENT);
            item.setStyle(sf::Text::Bold);
        } else {
            item.setFillColor(cfg::COLOR_TEXT);
        }
        centerHorizontally(item, firstItemY + i * itemStep);
        target.draw(item);
    }

    // Подсказка снизу.
    sf::Text hint(m_font,
                  cfg::toSfString("Стрелки - выбор   |   Enter - подтвердить   |   Esc - выход"),
                  14);
    hint.setFillColor(cfg::COLOR_TEXT_DIM);
    centerHorizontally(hint, static_cast<float>(cfg::WINDOW_HEIGHT) - 40.0f);
    target.draw(hint);
}

// ---------------------------------------------------------------------
// Экран таблицы рекордов.
// ---------------------------------------------------------------------
void Menu::drawScores(sf::RenderTarget& target, const ScoreManager& scores) const {
    sf::RectangleShape bg({
        static_cast<float>(cfg::WINDOW_WIDTH),
        static_cast<float>(cfg::WINDOW_HEIGHT)
    });
    bg.setFillColor(cfg::COLOR_BACKGROUND);
    target.draw(bg);

    sf::Text title(m_font, cfg::toSfString("Таблица рекордов"), 36);
    title.setFillColor(cfg::COLOR_TEXT_ACCENT);
    title.setStyle(sf::Text::Bold);
    centerHorizontally(title, 30.0f);
    target.draw(title);

    const auto& entries = scores.entries();
    if (entries.empty()) {
        sf::Text empty(m_font, cfg::toSfString("Пока никто не играл. Будьте первым."), 18);
        empty.setFillColor(cfg::COLOR_TEXT_DIM);
        centerHorizontally(empty, 200.0f);
        target.draw(empty);
    } else {
        const float startY = 100.0f;
        const float stepY  = 30.0f;
        int rank = 1;
        for (const auto& e : entries) {
            // Показываем только номер и очки, без имени игрока.
            std::string line = std::to_string(rank) + ".   " +
                               std::to_string(e.score) + " очков";
            sf::Text row(m_font, cfg::toSfString(line), 20);
            row.setFillColor(rank == 1 ? cfg::COLOR_TEXT_ACCENT : cfg::COLOR_TEXT);
            centerHorizontally(row, startY + (rank - 1) * stepY);
            target.draw(row);
            ++rank;
            if (rank > cfg::MAX_SCORES) break;
        }
    }

    sf::Text hint(m_font, cfg::toSfString("Esc / Enter - назад в меню"), 14);
    hint.setFillColor(cfg::COLOR_TEXT_DIM);
    centerHorizontally(hint, static_cast<float>(cfg::WINDOW_HEIGHT) - 40.0f);
    target.draw(hint);
}

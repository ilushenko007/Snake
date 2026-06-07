// =====================================================================
// Game.hpp
// ---------------------------------------------------------------------
// Класс Game - "режиссёр" всего приложения. Он:
//   - создаёт окно;
//   - хранит общие ресурсы (шрифт, таблицу рекордов);
//   - управляет переключением между состояниями
//     (TitleScreen / Menu / Scores / Playing / Paused / GameOver / EnterName);
//   - запускает главный игровой цикл (метод run()).
//
// Метод run() - это и есть тот самый "game loop", о котором написано в
// разделе 1.4 пояснительной записки.
// =====================================================================
#pragma once

#include "Snake.hpp"
#include "Fruit.hpp"
#include "Field.hpp"
#include "ScoreManager.hpp"
#include "Menu.hpp"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include <optional>
#include <string>

// Возможные состояния игры. Стейт-машина - удобный способ описать,
// что именно нужно отрисовать и какие клавиши обрабатывать в каждый
// момент времени.
enum class GameState {
    TitleScreen, // заставка с подписью автора (показывается один раз при старте)
    Menu,        // главное меню
    Scores,      // экран таблицы рекордов
    Playing,     // идёт игра
    Paused,      // игра на паузе
    GameOver,    // змейка погибла, ждём ввода имени или возврата в меню
    EnterName    // игрок попал в топ - вводит имя
};

class Game {
public:
    Game();

    // Запустить игру. Возвращает код выхода (0 - нормальное завершение).
    int run();

private:
    // --- Обработка ввода ---
    void handleEvents();
    void onKeyPressedTitleScreen(sf::Keyboard::Key key);
    void onKeyPressedMenu(sf::Keyboard::Key key);
    void onKeyPressedPlaying(sf::Keyboard::Key key);
    void onKeyPressedPaused(sf::Keyboard::Key key);
    void onKeyPressedGameOver(sf::Keyboard::Key key);
    void onKeyPressedScores(sf::Keyboard::Key key);
    void onKeyPressedEnterName(const sf::Event::KeyPressed& kp);
    void onTextEnteredEnterName(const sf::Event::TextEntered& te);

    // --- Игровая логика ---
    void updateGame(float dt);   // шаг змейки по таймеру
    void startNewGame();          // обнулить состояние, перейти в Playing
    void onSnakeDied();           // обработать гибель змейки

    // --- Рисование ---
    void render();
    void drawHud();
    void drawCenteredMessage(const std::string& title,
                             const std::string& subtitle = "");
    void drawEnterName();

    // --- Утилиты ---
    void updateWindowTitle();

private:
    sf::RenderWindow      m_window;
    sf::Font              m_font;
    bool                  m_fontLoaded = false;

    GameState             m_state;

    Field                 m_field;
    Snake                 m_snake;
    // std::optional удобен, потому что в меню ещё нет фрукта,
    // а создать его без змейки нельзя.
    std::optional<Fruit>  m_fruit;
    ScoreManager          m_scores;
    Menu                  m_menu;

    int                   m_score              = 0;
    float                 m_stepsPerSecond     = 0.0f;
    float                 m_accumulator        = 0.0f;  // накопитель для шага змейки
    sf::Clock             m_frameClock;

    std::string           m_playerName;        // вводимое имя для топа
};

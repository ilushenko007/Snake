// =====================================================================
// Game.cpp
// ---------------------------------------------------------------------
// Главный класс игры: создаёт окно, держит состояние, крутит игровой
// цикл. Состояние - простая стейт-машина (см. enum GameState).
// =====================================================================
#include "Game.hpp"
#include "Config.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/VideoMode.hpp>

#include <algorithm>
#include <stdexcept>
#include <string>

// ---------------------------------------------------------------------
// Конструктор: создаём окно, грузим шрифт и таблицу рекордов.
// ---------------------------------------------------------------------
Game::Game()
    : m_window(
          sf::VideoMode({
              static_cast<unsigned>(cfg::WINDOW_WIDTH),
              static_cast<unsigned>(cfg::WINDOW_HEIGHT)
          }),
          cfg::toSfString(cfg::WINDOW_TITLE))
    , m_state(GameState::TitleScreen)
    , m_menu(m_font)
    , m_stepsPerSecond(cfg::INITIAL_STEPS_PER_SECOND)
{
    m_window.setFramerateLimit(60);

    // Загружаем шрифт из встроенных ресурсов (cmrc).
    // Шрифт вшит в EXE при компиляции — файл assets/ на диске не нужен.
    cfg::loadFontFromResources(m_font);
    m_fontLoaded = true;

    m_scores.load();
}

// ---------------------------------------------------------------------
// Главный цикл игры.
// ---------------------------------------------------------------------
int Game::run() {
    while (m_window.isOpen()) {
        handleEvents();
        const float dt = m_frameClock.restart().asSeconds();
        if (m_state == GameState::Playing) {
            updateGame(dt);
        }
        render();
        updateWindowTitle();
    }
    return 0;
}

// ---------------------------------------------------------------------
// Обработка всех событий за кадр.
// ---------------------------------------------------------------------
void Game::handleEvents() {
    while (const std::optional event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
            continue;
        }

        if (const auto* te = event->getIf<sf::Event::TextEntered>()) {
            if (m_state == GameState::EnterName) {
                onTextEnteredEnterName(*te);
            }
            continue;
        }

        if (const auto* kp = event->getIf<sf::Event::KeyPressed>()) {
            const sf::Keyboard::Key key = kp->code;

            if (key == sf::Keyboard::Key::Escape && m_state != GameState::EnterName) {
                if (m_state == GameState::Menu) {
                    m_window.close();
                } else if (m_state == GameState::TitleScreen) {
                    m_state = GameState::Menu;
                } else {
                    m_state = GameState::Menu;
                }
                continue;
            }

            switch (m_state) {
                case GameState::TitleScreen: onKeyPressedTitleScreen(key); break;
                case GameState::Menu:       onKeyPressedMenu(key);       break;
                case GameState::Playing:    onKeyPressedPlaying(key);    break;
                case GameState::Paused:     onKeyPressedPaused(key);     break;
                case GameState::GameOver:   onKeyPressedGameOver(key);   break;
                case GameState::Scores:     onKeyPressedScores(key);     break;
                case GameState::EnterName:  onKeyPressedEnterName(*kp);  break;
            }
        }
    }
}

void Game::onKeyPressedTitleScreen(sf::Keyboard::Key /*key*/) {
    m_state = GameState::Menu;
}

void Game::onKeyPressedMenu(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Key::Up || key == sf::Keyboard::Key::W) {
        m_menu.moveUp();
    } else if (key == sf::Keyboard::Key::Down || key == sf::Keyboard::Key::S) {
        m_menu.moveDown();
    } else if (key == sf::Keyboard::Key::Enter || key == sf::Keyboard::Key::Space) {
        switch (m_menu.confirm()) {
            case MenuChoice::StartGame:  startNewGame();                break;
            case MenuChoice::ShowScores: m_state = GameState::Scores;   break;
            case MenuChoice::Quit:       m_window.close();              break;
            default:                                                     break;
        }
    }
}

void Game::onKeyPressedPlaying(sf::Keyboard::Key key) {
    switch (key) {
        case sf::Keyboard::Key::Up:
        case sf::Keyboard::Key::W:
            m_snake.setDirection(Direction::Up);
            break;
        case sf::Keyboard::Key::Down:
        case sf::Keyboard::Key::S:
            m_snake.setDirection(Direction::Down);
            break;
        case sf::Keyboard::Key::Left:
        case sf::Keyboard::Key::A:
            m_snake.setDirection(Direction::Left);
            break;
        case sf::Keyboard::Key::Right:
        case sf::Keyboard::Key::D:
            m_snake.setDirection(Direction::Right);
            break;
        case sf::Keyboard::Key::Space:
            m_state = GameState::Paused;
            break;
        default:
            break;
    }
}

void Game::onKeyPressedPaused(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Key::Space) {
        m_state = GameState::Playing;
        m_accumulator = 0.0f;
        m_frameClock.restart();
    }
}

void Game::onKeyPressedGameOver(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Key::Space) {
        startNewGame();
    } else if (key == sf::Keyboard::Key::Enter) {
        m_state = GameState::Menu;
    }
}

void Game::onKeyPressedScores(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Key::Enter || key == sf::Keyboard::Key::Space) {
        m_state = GameState::Menu;
    }
}

void Game::onKeyPressedEnterName(const sf::Event::KeyPressed& kp) {
    if (kp.code == sf::Keyboard::Key::Enter) {
        if (m_playerName.empty()) {
            m_playerName = "Player";
        }
        m_scores.tryAdd(m_playerName, m_score);
        m_scores.save();
        m_state = GameState::GameOver;
    } else if (kp.code == sf::Keyboard::Key::Escape) {
        m_state = GameState::GameOver;
    }
}

void Game::onTextEnteredEnterName(const sf::Event::TextEntered& te) {
    const char32_t ch = te.unicode;
    if (ch == 8) {
        if (!m_playerName.empty()) {
            m_playerName.pop_back();
        }
        return;
    }
    if (ch < 32 || ch == 127) return;
    if (m_playerName.size() >= 14) return;
    if (ch < 128) {
        m_playerName.push_back(static_cast<char>(ch));
    }
}

void Game::updateGame(float dt) {
    m_accumulator += dt;
    const float stepDuration = 1.0f / m_stepsPerSecond;

    while (m_accumulator >= stepDuration) {
        m_accumulator -= stepDuration;

        const Cell nextHead = m_snake.peekNextHead();
        const bool willEat = (m_fruit && nextHead == m_fruit->position());

        m_snake.step(/*grow=*/willEat);

        const Cell h = m_snake.head();
        if (h.x < 0 || h.x >= cfg::COLS || h.y < 0 || h.y >= cfg::ROWS) {
            onSnakeDied();
            return;
        }

        if (m_snake.collidesWithSelf()) {
            onSnakeDied();
            return;
        }

        if (willEat) {
            m_score += cfg::FRUIT_SCORE;
            m_stepsPerSecond = std::min(
                m_stepsPerSecond + cfg::SPEED_INCREMENT,
                cfg::MAX_STEPS_PER_SECOND
            );
            m_fruit->respawn(m_snake);
        }
    }
}

void Game::startNewGame() {
    m_snake.reset();
    m_fruit.emplace(m_snake);
    m_score          = 0;
    m_stepsPerSecond = cfg::INITIAL_STEPS_PER_SECOND;
    m_accumulator    = 0.0f;
    m_state          = GameState::Playing;
    m_playerName.clear();
    m_frameClock.restart();
}

void Game::onSnakeDied() {
    bool madeTop = false;
    const auto& entries = m_scores.entries();
    if (entries.size() < static_cast<std::size_t>(cfg::MAX_SCORES)) {
        madeTop = (m_score > 0);
    } else {
        madeTop = (m_score > entries.back().score);
    }

    if (madeTop) {
        m_scores.tryAdd("", m_score);
        m_scores.save();
    }
    m_state = GameState::GameOver;
}

void Game::render() {
    m_window.clear(cfg::COLOR_BACKGROUND);

    switch (m_state) {
        case GameState::TitleScreen:
            m_menu.drawTitleScreen(m_window);
            break;
        case GameState::Menu:
            m_menu.drawMain(m_window);
            break;
        case GameState::Scores:
            m_menu.drawScores(m_window, m_scores);
            break;
        case GameState::Playing:
        case GameState::Paused:
            drawHud();
            m_field.draw(m_window);
            if (m_fruit) m_fruit->draw(m_window);
            m_snake.draw(m_window);
            if (m_state == GameState::Paused) {
                drawCenteredMessage("ПАУЗА", "Пробел - продолжить, Esc - в меню");
            }
            break;
        case GameState::GameOver:
            drawHud();
            m_field.draw(m_window);
            if (m_fruit) m_fruit->draw(m_window);
            m_snake.draw(m_window);
            drawCenteredMessage(
                "ИГРА ОКОНЧЕНА",
                "Счёт: " + std::to_string(m_score) +
                "   |   Пробел - заново, Enter - в меню"
            );
            break;
        case GameState::EnterName:
            drawHud();
            m_field.draw(m_window);
            if (m_fruit) m_fruit->draw(m_window);
            m_snake.draw(m_window);
            drawEnterName();
            break;
    }

    m_window.display();
}

void Game::drawHud() {
    sf::RectangleShape hud({
        static_cast<float>(cfg::WINDOW_WIDTH),
        static_cast<float>(cfg::WINDOW_HEIGHT)
    });
    hud.setPosition({ 0.0f, 0.0f });
    hud.setFillColor(cfg::COLOR_HUD);
    m_window.draw(hud);

    std::string info =
        "Счёт: "  + std::to_string(m_score) +
        "    Длина: " + std::to_string(m_snake.length()) +
        "    Скорость: " + std::to_string(static_cast<int>(m_stepsPerSecond));

    sf::Text text(m_font, cfg::toSfString(info), 18);
    text.setFillColor(cfg::COLOR_TEXT);
    text.setPosition({ 12.0f, 9.0f });
    m_window.draw(text);

    if (m_scores.bestScore() > 0) {
        std::string best = "Рекорд: " + std::to_string(m_scores.bestScore());
        sf::Text bestText(m_font, cfg::toSfString(best), 18);
        bestText.setFillColor(cfg::COLOR_TEXT_DIM);
        const auto b = bestText.getLocalBounds();
        bestText.setPosition({
            static_cast<float>(cfg::WINDOW_WIDTH) - b.size.x - b.position.x - 12.0f,
            9.0f
        });
        m_window.draw(bestText);
    }
}

void Game::drawCenteredMessage(const std::string& title, const std::string& subtitle) {
    sf::RectangleShape veil({
        static_cast<float>(cfg::WINDOW_WIDTH),
        static_cast<float>(cfg::WINDOW_HEIGHT)
    });
    veil.setFillColor(sf::Color(0, 0, 0, 150));
    m_window.draw(veil);

    sf::Text titleText(m_font, cfg::toSfString(title), 48);
    titleText.setFillColor(cfg::COLOR_TEXT_ACCENT);
    titleText.setStyle(sf::Text::Bold);
    const auto tb = titleText.getLocalBounds();
    titleText.setPosition({
        (static_cast<float>(cfg::WINDOW_WIDTH) - tb.size.x) * 0.5f - tb.position.x,
        static_cast<float>(cfg::WINDOW_HEIGHT) * 0.4f
    });
    m_window.draw(titleText);

    if (!subtitle.empty()) {
        sf::Text subText(m_font, cfg::toSfString(subtitle), 18);
        subText.setFillColor(cfg::COLOR_TEXT);
        const auto sb = subText.getLocalBounds();
        subText.setPosition({
            (static_cast<float>(cfg::WINDOW_WIDTH) - sb.size.x) * 0.5f - sb.position.x,
            static_cast<float>(cfg::WINDOW_HEIGHT) * 0.4f + 70.0f
        });
        m_window.draw(subText);
    }
}

void Game::drawEnterName() {
    sf::RectangleShape veil({
        static_cast<float>(cfg::WINDOW_WIDTH),
        static_cast<float>(cfg::WINDOW_HEIGHT)
    });
    veil.setFillColor(sf::Color(0, 0, 0, 180));
    m_window.draw(veil);

    sf::Text title(m_font, cfg::toSfString("НОВЫЙ РЕКОРД!"), 40);
    title.setFillColor(cfg::COLOR_TEXT_ACCENT);
    title.setStyle(sf::Text::Bold);
    auto tb = title.getLocalBounds();
    title.setPosition({
        (static_cast<float>(cfg::WINDOW_WIDTH) - tb.size.x) * 0.5f - tb.position.x,
        static_cast<float>(cfg::WINDOW_HEIGHT) * 0.30f
    });
    m_window.draw(title);

    sf::Text scoreLabel(m_font, cfg::toSfString("Ваш счёт: " + std::to_string(m_score)), 22);
    scoreLabel.setFillColor(cfg::COLOR_TEXT);
    auto sb = scoreLabel.getLocalBounds();
    scoreLabel.setPosition({
        (static_cast<float>(cfg::WINDOW_WIDTH) - sb.size.x) * 0.5f - sb.position.x,
        static_cast<float>(cfg::WINDOW_HEIGHT) * 0.30f + 60.0f
    });
    m_window.draw(scoreLabel);

    sf::Text prompt(m_font, cfg::toSfString("Введите имя и нажмите Enter:"), 18);
    prompt.setFillColor(cfg::COLOR_TEXT_DIM);
    auto pb = prompt.getLocalBounds();
    prompt.setPosition({
        (static_cast<float>(cfg::WINDOW_WIDTH) - pb.size.x) * 0.5f - pb.position.x,
        static_cast<float>(cfg::WINDOW_HEIGHT) * 0.30f + 110.0f
    });
    m_window.draw(prompt);

    const bool cursorVisible = (static_cast<int>(m_frameClock.getElapsedTime().asMilliseconds() / 500) % 2) == 0;
    std::string shown = m_playerName + (cursorVisible ? "_" : " ");
    sf::Text nameText(m_font, cfg::toSfString(shown), 28);
    nameText.setFillColor(cfg::COLOR_TEXT_ACCENT);
    auto nb = nameText.getLocalBounds();
    nameText.setPosition({
        (static_cast<float>(cfg::WINDOW_WIDTH) - nb.size.x) * 0.5f - nb.position.x,
        static_cast<float>(cfg::WINDOW_HEIGHT) * 0.30f + 150.0f
    });
    m_window.draw(nameText);

    sf::Text hint(m_font, cfg::toSfString("Esc - пропустить ввод"), 14);
    hint.setFillColor(cfg::COLOR_TEXT_DIM);
    auto hb = hint.getLocalBounds();
    hint.setPosition({
        (static_cast<float>(cfg::WINDOW_WIDTH) - hb.size.x) * 0.5f - hb.position.x,
        static_cast<float>(cfg::WINDOW_HEIGHT) - 50.0f
    });
    m_window.draw(hint);
}

void Game::updateWindowTitle() {
    std::string state;
    switch (m_state) {
        case GameState::TitleScreen: state = "Заставка";      break;
        case GameState::Menu:        state = "Меню";          break;
        case GameState::Scores:      state = "Рекорды";       break;
        case GameState::Playing:     state = "Игра";          break;
        case GameState::Paused:      state = "Пауза";         break;
        case GameState::GameOver:    state = "Конец";         break;
        case GameState::EnterName:   state = "Новый рекорд"; break;
    }

    std::string title = cfg::WINDOW_TITLE + "   |   " + state;
    if (m_state == GameState::Playing || m_state == GameState::Paused ||
        m_state == GameState::GameOver || m_state == GameState::EnterName)
    {
        title += "   |   Счёт: " + std::to_string(m_score);
        title += "   |   Длина: " + std::to_string(m_snake.length());
    }
    m_window.setTitle(cfg::toSfString(title));
}

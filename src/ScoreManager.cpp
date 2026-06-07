// =====================================================================
// ScoreManager.cpp
// =====================================================================
#include "ScoreManager.hpp"
#include "Config.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>

// ---------------------------------------------------------------------
// Загрузить таблицу из файла. Если файла нет - просто оставляем
// пустой вектор. Имя читаем как одно слово (без пробелов), затем
// очки. Строки с некорректным форматом пропускаем.
// ---------------------------------------------------------------------
void ScoreManager::load() {
    m_entries.clear();

    std::ifstream in(cfg::SCORES_FILE);
    if (!in.is_open()) {
        return;  // первого запуска ещё не было - это нормально
    }

    std::string line;
    while (std::getline(in, line)) {
        std::istringstream ss(line);
        ScoreEntry e;
        if (ss >> e.name >> e.score) {
            m_entries.push_back(e);
        }
    }

    // На всякий случай сортируем сразу после загрузки (вдруг файл
    // был отредактирован руками).
    std::sort(m_entries.begin(), m_entries.end(),
              [](const ScoreEntry& a, const ScoreEntry& b) {
                  return a.score > b.score;
              });

    // Оставляем не больше MAX_SCORES записей.
    if (m_entries.size() > static_cast<std::size_t>(cfg::MAX_SCORES)) {
        m_entries.resize(cfg::MAX_SCORES);
    }
}

// ---------------------------------------------------------------------
// Сохранение в файл. Перезаписываем целиком.
// ---------------------------------------------------------------------
void ScoreManager::save() const {
    std::ofstream out(cfg::SCORES_FILE, std::ios::trunc);
    if (!out.is_open()) {
        return;   // нет прав на запись - молча игнорируем
    }
    for (const auto& e : m_entries) {
        out << e.name << ' ' << e.score << '\n';
    }
}

// ---------------------------------------------------------------------
// Попытаться добавить новый результат в таблицу.
// Возвращает true, если результат попал в топ.
// ---------------------------------------------------------------------
bool ScoreManager::tryAdd(const std::string& name, int score) {
    // Заменяем пробелы в имени на подчёркивания, чтобы формат файла
    // (имя + очки на одной строке) не сломался.
    std::string safeName = name.empty() ? std::string("Player") : name;
    for (char& c : safeName) {
        if (c == ' ' || c == '\t') c = '_';
    }

    m_entries.push_back(ScoreEntry{ safeName, score });
    std::sort(m_entries.begin(), m_entries.end(),
              [](const ScoreEntry& a, const ScoreEntry& b) {
                  return a.score > b.score;
              });

    const bool madeTop = std::any_of(
        m_entries.begin(),
        m_entries.begin() + std::min<std::size_t>(m_entries.size(), cfg::MAX_SCORES),
        [&](const ScoreEntry& e) { return e.name == safeName && e.score == score; });

    if (m_entries.size() > static_cast<std::size_t>(cfg::MAX_SCORES)) {
        m_entries.resize(cfg::MAX_SCORES);
    }

    return madeTop;
}

// ---------------------------------------------------------------------
// Лучший результат из таблицы.
// ---------------------------------------------------------------------
int ScoreManager::bestScore() const {
    return m_entries.empty() ? 0 : m_entries.front().score;
}

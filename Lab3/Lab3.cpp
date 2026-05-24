#include <SFML/Graphics.hpp>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <random>
#include <queue>
#include <optional>

const int FIELD_WIDTH = 8;
const int FIELD_HEIGHT = 8;
const int TS = 64;
const int OFFSET_X = 48, OFFSET_Y = 48;

enum class BonusType { None, Recolor, Bomb };
enum class GameState { WaitingForInput, SwapAnimation, BonusActivation, DropAnimation };

struct Gem {
    int color;
    bool isMatch;
    BonusType bonus;
    bool isEmpty;

    float offsetX;
    float offsetY;

    Gem() {
        color = 0;
        isMatch = false;
        bonus = BonusType::None;
        isEmpty = false;
        offsetX = 0.0f;
        offsetY = 0.0f;
    }
};

Gem grid[FIELD_HEIGHT][FIELD_WIDTH];

const sf::Color gemColors[] = {
    sf::Color::Red,
    sf::Color::Blue,
    sf::Color::Green,
    sf::Color::Yellow,
    sf::Color::Magenta
};
const int NUM_COLORS = 5;

std::mt19937 g_rng(static_cast<unsigned int>(std::time(nullptr)));

int getRandom(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(g_rng);
}

bool isNeighbor(int r1, int c1, int r2, int c2) {
    return std::abs(r1 - r2) + std::abs(c1 - c2) == 1;
}

// Создание поля без готовых комбинаций со старта
void initGrid() {
    bool hasMatches;
    do {
        hasMatches = false;
        for (int r = 0; r < FIELD_HEIGHT; r++) {
            for (int c = 0; c < FIELD_WIDTH; c++) {
                grid[r][c].color = getRandom(0, NUM_COLORS - 1);
                grid[r][c].isMatch = false;
                grid[r][c].isEmpty = false;
                grid[r][c].bonus = BonusType::None;
                grid[r][c].offsetX = 0;
                grid[r][c].offsetY = 0;
            }
        }

        
        bool visited[FIELD_HEIGHT][FIELD_WIDTH] = { false };
        for (int r = 0; r < FIELD_HEIGHT; r++) {
            for (int c = 0; c < FIELD_WIDTH; c++) {
                if (visited[r][c]) continue;
                int targetColor = grid[r][c].color;
                int clusterSize = 0;
                std::queue<std::pair<int, int>> q;
                q.push({ r, c });
                visited[r][c] = true;

                while (!q.empty()) {
                    auto [currR, currC] = q.front();
                    q.pop();
                    clusterSize++;

                    int dr[] = { -1, 1, 0, 0 };
                    int dc[] = { 0, 0, -1, 1 };
                    for (int i = 0; i < 4; i++) {
                        int nr = currR + dr[i];
                        int nc = currC + dc[i];
                        if (nr >= 0 && nr < FIELD_HEIGHT && nc >= 0 && nc < FIELD_WIDTH) {
                            if (!visited[nr][nc] && grid[nr][nc].color == targetColor) {
                                visited[nr][nc] = true;
                                q.push({ nr, nc });
                            }
                        }
                    }
                }
                if (clusterSize >= 3) hasMatches = true;
            }
        }
    } while (hasMatches); // Перегенерация, если со старта есть совпадения
}

void swapGems(Gem& g1, Gem& g2) {
    std::swap(g1.color, g2.color);
    std::swap(g1.bonus, g2.bonus);
    std::swap(g1.isEmpty, g2.isEmpty);
}

// Бонус Перекраска
void triggerRecolor(int targetRow, int targetCol, int originalColor) {
    std::vector<std::pair<int, int>> validCells;
    for (int r = 0; r < FIELD_HEIGHT; r++) {
        for (int c = 0; c < FIELD_WIDTH; c++) {
            if (std::abs(r - targetRow) <= 3 && std::abs(c - targetCol) <= 3) {
                if (r != targetRow || c != targetCol) {
                    validCells.push_back({ r, c });
                }
            }
        }
    }

    grid[targetRow][targetCol].color = originalColor;
    grid[targetRow][targetCol].isEmpty = false;

    std::shuffle(validCells.begin(), validCells.end(), g_rng);
    int chosen = 0;
    for (const auto& cell : validCells) {
        if (chosen >= 2) break;
        if (!isNeighbor(targetRow, targetCol, cell.first, cell.second)) {
            grid[cell.first][cell.second].color = originalColor;
            grid[cell.first][cell.second].isEmpty = false;
            chosen++;
        }
    }
}

// Бонус Бомба
void triggerBomb(int targetRow, int targetCol) {
    grid[targetRow][targetCol].isMatch = true;
    grid[targetRow][targetCol].isEmpty = true;

    std::vector<std::pair<int, int>> allCells;
    for (int r = 0; r < FIELD_HEIGHT; r++) {
        for (int c = 0; c < FIELD_WIDTH; c++) {
            if (r != targetRow || c != targetCol) {
                allCells.push_back({ r, c });
            }
        }
    }

    std::shuffle(allCells.begin(), allCells.end(), g_rng);
    int toDestroy = std::min(5, (int)allCells.size());
    for (int i = 0; i < toDestroy; i++) {
        grid[allCells[i].first][allCells[i].second].isEmpty = true;
    }
}


int activeBonusRow = -1;
int activeBonusCol = -1;
BonusType activeBonusType = BonusType::None;
int activeBonusColor = 0;

// Функция поиска совпадений
bool findMatches() {
    bool hasMatches = false;
    bool visited[FIELD_HEIGHT][FIELD_WIDTH] = { false };

    // Сброс старых меток уничтожения
    for (int r = 0; r < FIELD_HEIGHT; r++) {
        for (int c = 0; c < FIELD_WIDTH; c++) {
            grid[r][c].isMatch = false;
        }
    }

    for (int r = 0; r < FIELD_HEIGHT; r++) {
        for (int c = 0; c < FIELD_WIDTH; c++) {
            if (grid[r][c].isEmpty || visited[r][c]) continue;

            int targetColor = grid[r][c].color;
            std::vector<std::pair<int, int>> currentCluster;
            std::queue<std::pair<int, int>> q;

            q.push({ r, c });
            visited[r][c] = true;

            while (!q.empty()) {
                auto [currR, currC] = q.front();
                q.pop();
                currentCluster.push_back({ currR, currC });

                int dr[] = { -1, 1, 0, 0 };
                int dc[] = { 0, 0, -1, 1 };

                for (int i = 0; i < 4; i++) {
                    int nr = currR + dr[i];
                    int nc = currC + dc[i];

                    if (nr >= 0 && nr < FIELD_HEIGHT && nc >= 0 && nc < FIELD_WIDTH) {
                        if (!grid[nr][nc].isEmpty && !visited[nr][nc] && grid[nr][nc].color == targetColor) {
                            visited[nr][nc] = true;
                            q.push({ nr, nc });
                        }
                    }
                }
            }

            // Если нашлась группа из 3+ смежных клеток
            if (currentCluster.size() >= 3) {
                hasMatches = true;
                for (const auto& cell : currentCluster) {
                    grid[cell.first][cell.second].isMatch = true;
                }
            }
        }
    }

    if (hasMatches) {
        // Проверка, выпадет ли случайный бонус (20% шанс)
        bool bonusDropped = false;
        if (getRandom(1, 100) <= 20) {
            // Выбираем случайную клетку из тех, что уничтожаются
            std::vector<std::pair<int, int>> matchCells;
            for (int r = 0; r < FIELD_HEIGHT; r++) {
                for (int c = 0; c < FIELD_WIDTH; c++) {
                    if (grid[r][c].isMatch) matchCells.push_back({ r, c });
                }
            }
            if (!matchCells.empty()) {
                auto luckyCell = matchCells[getRandom(0, matchCells.size() - 1)];

                // Параметры будущего бонуса
                activeBonusRow = luckyCell.first;
                activeBonusCol = luckyCell.second;
                activeBonusType = (getRandom(0, 1) == 0) ? BonusType::Recolor : BonusType::Bomb;
                activeBonusColor = grid[activeBonusRow][activeBonusCol].color;
                bonusDropped = true;
            }
        }

        // Уничтожение совпавших клеток
        for (int r = 0; r < FIELD_HEIGHT; r++) {
            for (int c = 0; c < FIELD_WIDTH; c++) {
                if (grid[r][c].isMatch) {
                    grid[r][c].isEmpty = true;
                    grid[r][c].isMatch = false;
                }
            }
        }

        // Если бонус выпал, клетка превращается в бонусную
        if (bonusDropped) {
            grid[activeBonusRow][activeBonusCol].isEmpty = false;
            grid[activeBonusRow][activeBonusCol].color = activeBonusColor;
            grid[activeBonusRow][activeBonusCol].bonus = activeBonusType;
        }
        else {
            activeBonusType = BonusType::None;
        }
    }

    return hasMatches;
}

bool stepDropAndSpawn() {
    bool moved = false;
    for (int c = 0; c < FIELD_WIDTH; c++) {
        for (int r = FIELD_HEIGHT - 1; r > 0; r--) {
            if (grid[r][c].isEmpty && !grid[r - 1][c].isEmpty) {
                grid[r][c].color = grid[r - 1][c].color;
                grid[r][c].bonus = grid[r - 1][c].bonus;
                grid[r][c].isEmpty = false;
                grid[r - 1][c].isEmpty = true;
                grid[r - 1][c].bonus = BonusType::None;
                moved = true;
            }
        }
    }
    for (int c = 0; c < FIELD_WIDTH; c++) {
        if (grid[0][c].isEmpty) {
            grid[0][c].color = getRandom(0, NUM_COLORS - 1);
            grid[0][c].bonus = BonusType::None;
            grid[0][c].isEmpty = false;
            moved = true;
        }
    }
    return moved;
}

int main() {
    sf::RenderWindow window(sf::VideoMode({ 600, 600 }), "GEMS - Visible Bonuses (SFML 3)");
    window.setFramerateLimit(60);

    initGrid();

    GameState state = GameState::WaitingForInput;
    int clickCount = 0;
    int row1 = -1, col1 = -1, row2 = -1, col2 = -1;

    float swapProgress = 0.0f;
    const float swapSpeed = 8.0f;

    sf::Clock animationClock;
    const sf::Time frameDelay = sf::milliseconds(80);
    const sf::Time bonusDelay = sf::milliseconds(600); // 0.6 секунды задержки для демонстрации бонуса

    sf::Clock pulseClock; // Для анимации пульсации бонуса

    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (state == GameState::WaitingForInput) {
                if (const auto* mouseClick = event->getIf<sf::Event::MouseButtonPressed>()) {
                    if (mouseClick->button == sf::Mouse::Button::Left) {
                        int mouseX = mouseClick->position.x - OFFSET_X;
                        int mouseY = mouseClick->position.y - OFFSET_Y;

                        int col = mouseX / TS;
                        int row = mouseY / TS;

                        if (row >= 0 && row < FIELD_HEIGHT && col >= 0 && col < FIELD_WIDTH) {
                            if (clickCount == 0) {
                                row1 = row; col1 = col;
                                clickCount = 1;
                            }
                            else if (clickCount == 1) {
                                row2 = row; col2 = col;

                                if (isNeighbor(row1, col1, row2, col2)) {
                                    state = GameState::SwapAnimation;
                                    swapProgress = 0.0f;
                                }
                                else {
                                    row1 = row; col1 = col;
                                    clickCount = 1;
                                }
                            }
                        }
                    }
                }
            }
        }


        // Перемещение квадратов
        if (state == GameState::SwapAnimation) {
            swapProgress += swapSpeed;

            float dirX = static_cast<float>(col2 - col1) * TS;
            float dirY = static_cast<float>(row2 - row1) * TS;

            if (swapProgress <= 100.0f) {
                float ratio = swapProgress / 100.0f;
                grid[row1][col1].offsetX = dirX * ratio;
                grid[row1][col1].offsetY = dirY * ratio;
                grid[row2][col2].offsetX = -dirX * ratio;
                grid[row2][col2].offsetY = -dirY * ratio;
            }
            else {
                grid[row1][col1].offsetX = 0; grid[row1][col1].offsetY = 0;
                grid[row2][col2].offsetX = 0; grid[row2][col2].offsetY = 0;

                swapGems(grid[row1][col1], grid[row2][col2]);

                if (findMatches()) {
                    // Если совпадение создало бонус, включается режим задержки
                    if (activeBonusType != BonusType::None) {
                        state = GameState::BonusActivation;
                        animationClock.restart();
                    }
                    else {
                        state = GameState::DropAnimation;
                        animationClock.restart();
                    }
                }
                else {
                    state = GameState::WaitingForInput;
                }
                clickCount = 0;
            }
        }

        // Показываем бонус перед его взрывом/эффектом
        if (state == GameState::BonusActivation) {
            if (animationClock.getElapsedTime() >= bonusDelay) {
                if (activeBonusType == BonusType::Bomb) {
                    triggerBomb(activeBonusRow, activeBonusCol);
                }
                else if (activeBonusType == BonusType::Recolor) {
                    triggerRecolor(activeBonusRow, activeBonusCol, activeBonusColor);
                }

                // Убираем маркеры и переходим к падению
                grid[activeBonusRow][activeBonusCol].bonus = BonusType::None;
                activeBonusType = BonusType::None;

                state = GameState::DropAnimation;
                animationClock.restart();
            }
        }

        //Падение квадратов
        if (state == GameState::DropAnimation) {
            if (animationClock.getElapsedTime() >= frameDelay) {
                bool fieldChanged = stepDropAndSpawn();
                animationClock.restart();

                if (!fieldChanged) {
                    // Проверка новых совпадений после падения
                    if (findMatches()) {
                        if (activeBonusType != BonusType::None) {
                            state = GameState::BonusActivation;
                        }
                        else {
                            state = GameState::DropAnimation;
                        }
                    }
                    else {
                        state = GameState::WaitingForInput;
                    }
                }
            }
        }

        // Отрисовка изображения
        window.clear(sf::Color(30, 30, 30));

        // Пульсации для активирующегося бонуса
        float timeSec = pulseClock.getElapsedTime().asSeconds();
        float pulseScale = 1.0f + 0.25f * std::sin(timeSec * 15.0f);

        for (int r = 0; r < FIELD_HEIGHT; r++) {
            for (int c = 0; c < FIELD_WIDTH; c++) {
                if (grid[r][c].isEmpty) continue;

                sf::RectangleShape tile(sf::Vector2f({ static_cast<float>(TS - 4), static_cast<float>(TS - 4) }));

                float renderX = c * TS + OFFSET_X + 2 + grid[r][c].offsetX;
                float renderY = r * TS + OFFSET_Y + 2 + grid[r][c].offsetY;

                tile.setPosition(sf::Vector2f({ renderX, renderY }));
                tile.setFillColor(gemColors[grid[r][c].color]);

                if (state == GameState::WaitingForInput && clickCount == 1 && r == row1 && c == col1) {
                    tile.setOutlineThickness(3);
                    tile.setOutlineColor(sf::Color::White);
                }
                else {
                    tile.setOutlineThickness(0);
                }

                window.draw(tile);

                // --- Визуализация бонусов ---
                if (grid[r][c].bonus != BonusType::None) {
                    bool isActiveNow = (state == GameState::BonusActivation && r == activeBonusRow && c == activeBonusCol);

                    if (grid[r][c].bonus == BonusType::Bomb) {
                        float radius = 14.0f;
                        if (isActiveNow) radius *= pulseScale; // Бомба увеличивается/пульсирует в момент активации

                        sf::CircleShape bombVisual(radius);
                        bombVisual.setFillColor(sf::Color(255, 255, 255, 220));
                        bombVisual.setOrigin({ radius, radius });
                        bombVisual.setPosition({ renderX + (TS - 4) / 2.0f, renderY + (TS - 4) / 2.0f });

                        //Момент активации, добавление красной обводки
                        if (isActiveNow) {
                            bombVisual.setOutlineThickness(3);
                            bombVisual.setOutlineColor(sf::Color::Red);
                        }
                        window.draw(bombVisual);
                    }
                    else if (grid[r][c].bonus == BonusType::Recolor) {
                        float size = 24.0f;
                        if (isActiveNow) size *= pulseScale;

                        sf::RectangleShape recolorVisual(sf::Vector2f({ size, size }));
                        recolorVisual.setOutlineThickness(3);
                        recolorVisual.setOutlineColor(isActiveNow ? sf::Color::Cyan : sf::Color::Black);
                        recolorVisual.setFillColor(sf::Color::Transparent);
                        recolorVisual.setOrigin({ size / 2.0f, size / 2.0f });
                        recolorVisual.setPosition({ renderX + (TS - 4) / 2.0f, renderY + (TS - 4) / 2.0f });
                        window.draw(recolorVisual);
                    }
                }
            }
        }

        window.display();
    }

    return 0;
}

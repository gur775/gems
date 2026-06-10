#include "Board.h"
#include "GemsFactory.h"
#include <algorithm>
#include <queue>
#include <ctime>

Board::Board() {
    rng.seed(static_cast<unsigned int>(std::time(nullptr)));
    initGrid();
}

int Board::getRandom(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

bool Board::isNeighbor(int r1, int c1, int r2, int c2) const {
    return std::abs(r1 - r2) + std::abs(c1 - c2) == 1;
}

Gems* Board::getGems(int r, int c) const {
    if (r >= 0 && r < FIELD_HEIGHT && c >= 0 && c < FIELD_WIDTH) return grid[r][c].get();
    return nullptr;
}

void Board::swapGemsInMatrix(int r1, int c1, int r2, int c2) {
    std::swap(grid[r1][c1], grid[r2][c2]);
}

void Board::initGrid() {
    bool hasMatches;
    do {
        hasMatches = false;
        for (int r = 0; r < FIELD_HEIGHT; r++) {
            for (int c = 0; c < FIELD_WIDTH; c++) {
                grid[r][c] = GemsFactory::createGem(getRandom(0, 4), BonusType::None);
            }
        }

        bool visited[FIELD_HEIGHT][FIELD_WIDTH] = { false };
        for (int r = 0; r < FIELD_HEIGHT; r++) {
            for (int c = 0; c < FIELD_WIDTH; c++) {
                if (visited[r][c]) continue;
                int targetColor = grid[r][c]->getColor();
                int clusterSize = 0;
                std::queue<std::pair<int, int>> q;
                q.push({ r, c });
                visited[r][c] = true;

                while (!q.empty()) {
                    auto [currR, currC] = q.front(); q.pop();
                    clusterSize++;
                    int dr[] = { -1, 1, 0, 0 }, dc[] = { 0, 0, -1, 1 };
                    for (int i = 0; i < 4; i++) {
                        int nr = currR + dr[i], nc = currC + dc[i];
                        if (nr >= 0 && nr < FIELD_HEIGHT && nc >= 0 && nc < FIELD_WIDTH) {
                            if (!visited[nr][nc] && grid[nr][nc]->getColor() == targetColor) {
                                visited[nr][nc] = true;
                                q.push({ nr, nc });
                            }
                        }
                    }
                }
                if (clusterSize >= 3) hasMatches = true;
            }
        }
    } while (hasMatches);
}

bool Board::findMatches(int& outBonusRow, int& outBonusCol, BonusType& outBonusType, int& outBonusColor) {
    bool hasMatches = false;
    bool visited[FIELD_HEIGHT][FIELD_WIDTH] = { false };

    for (int r = 0; r < FIELD_HEIGHT; r++) {
        for (int c = 0; c < FIELD_WIDTH; c++) {
            if (grid[r][c]) grid[r][c]->setIsMatch(false);
        }
    }

    for (int r = 0; r < FIELD_HEIGHT; r++) {
        for (int c = 0; c < FIELD_WIDTH; c++) {
            if (grid[r][c]->getIsEmpty() || visited[r][c]) continue;

            int targetColor = grid[r][c]->getColor();
            std::vector<std::pair<int, int>> currentCluster;
            std::queue<std::pair<int, int>> q;
            q.push({ r, c });
            visited[r][c] = true;

            while (!q.empty()) {
                auto [currR, currC] = q.front(); q.pop();
                currentCluster.push_back({ currR, currC });
                int dr[] = { -1, 1, 0, 0 }, dc[] = { 0, 0, -1, 1 };
                for (int i = 0; i < 4; i++) {
                    int nr = currR + dr[i], nc = currC + dc[i];
                    if (nr >= 0 && nr < FIELD_HEIGHT && nc >= 0 && nc < FIELD_WIDTH) {
                        if (!grid[nr][nc]->getIsEmpty() && !visited[nr][nc] && grid[nr][nc]->getColor() == targetColor) {
                            visited[nr][nc] = true;
                            q.push({ nr, nc });
                        }
                    }
                }
            }

            if (currentCluster.size() >= 3) {
                hasMatches = true;
                for (const auto& cell : currentCluster) {
                    grid[cell.first][cell.second]->setIsMatch(true);
                }
            }
        }
    }

    if (hasMatches) {
        bool bonusDropped = false;
        if (getRandom(1, 100) <= 20) {
            std::vector<std::pair<int, int>> matchCells;
            for (int r = 0; r < FIELD_HEIGHT; r++) {
                for (int c = 0; c < FIELD_WIDTH; c++) {
                    if (grid[r][c]->getIsMatch()) matchCells.push_back({ r, c });
                }
            }
            if (!matchCells.empty()) {
                auto luckyCell = matchCells[getRandom(0, matchCells.size() - 1)];
                outBonusRow = luckyCell.first;
                outBonusCol = luckyCell.second;
                outBonusType = (getRandom(0, 1) == 0) ? BonusType::Recolor : BonusType::Bomb;
                outBonusColor = grid[outBonusRow][outBonusCol]->getColor();
                bonusDropped = true;
            }
        }

        for (int r = 0; r < FIELD_HEIGHT; r++) {
            for (int c = 0; c < FIELD_WIDTH; c++) {
                if (grid[r][c]->getIsMatch()) {
                    grid[r][c]->setIsEmpty(true);
                    grid[r][c]->setIsMatch(false);
                }
            }
        }

        if (bonusDropped) {
            grid[outBonusRow][outBonusCol] = GemsFactory::createGem(outBonusColor, outBonusType);
        }
        else {
            outBonusType = BonusType::None;
        }
    }
    return hasMatches;
}

bool Board::stepDropAndSpawn() {
    bool moved = false;
    for (int c = 0; c < FIELD_WIDTH; c++) {
        for (int r = FIELD_HEIGHT - 1; r > 0; r--) {
            if (grid[r][c]->getIsEmpty() && !grid[r - 1][c]->getIsEmpty()) {
                grid[r][c] = std::move(grid[r - 1][c]);
                grid[r - 1][c] = GemsFactory::createGem(0, BonusType::None);
                grid[r - 1][c]->setIsEmpty(true);
                moved = true;
            }
        }
    }
    for (int c = 0; c < FIELD_WIDTH; c++) {
        if (grid[0][c]->getIsEmpty()) {
            grid[0][c] = GemsFactory::createGem(getRandom(0, 4), BonusType::None);
            moved = true;
        }
    }
    return moved;
}

void Board::triggerBombLogic(int targetRow, int targetCol) {
    grid[targetRow][targetCol]->setIsMatch(true);
    grid[targetRow][targetCol]->setIsEmpty(true);

    std::vector<std::pair<int, int>> allCells;
    for (int r = 0; r < FIELD_HEIGHT; r++) {
        for (int c = 0; c < FIELD_WIDTH; c++) {
            if (r != targetRow || c != targetCol) allCells.push_back({ r, c });
        }
    }
    std::shuffle(allCells.begin(), allCells.end(), rng);
    int toDestroy = std::min(5, (int)allCells.size());
    for (int i = 0; i < toDestroy; i++) {
        grid[allCells[i].first][allCells[i].second]->setIsEmpty(true);
    }
}

void Board::triggerRecolorLogic(int targetRow, int targetCol, int originalColor) {
    std::vector<std::pair<int, int>> validCells;
    for (int r = 0; r < FIELD_HEIGHT; r++) {
        for (int c = 0; c < FIELD_WIDTH; c++) {
            if (std::abs(r - targetRow) <= 3 && std::abs(c - targetCol) <= 3) {
                if (r != targetRow || c != targetCol) validCells.push_back({ r, c });
            }
        }
    }
    grid[targetRow][targetCol] = GemsFactory::createGem(originalColor, BonusType::None);
    std::shuffle(validCells.begin(), validCells.end(), rng);
    int chosen = 0;
    for (const auto& cell : validCells) {
        if (chosen >= 2) break;
        if (!isNeighbor(targetRow, targetCol, cell.first, cell.second)) {
            grid[cell.first][cell.second] = GemsFactory::createGem(originalColor, BonusType::None);
            chosen++;
        }
    }
}
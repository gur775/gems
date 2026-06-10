#ifndef BOARD_H
#define BOARD_H

#include <memory>
#include <vector>
#include <random>
#include "Gems.h"

const int FIELD_WIDTH = 8;
const int FIELD_HEIGHT = 8;

class Board {
private:
    std::unique_ptr<Gems> grid[FIELD_HEIGHT][FIELD_WIDTH];
    std::mt19937 rng;

    int getRandom(int min, int max);
    bool isNeighbor(int r1, int c1, int r2, int c2) const;

public:
    Board();
    void initGrid();

    Gems* getGems(int r, int c) const;
    void swapGemsInMatrix(int r1, int c1, int r2, int c2);

    bool findMatches(int& outBonusRow, int& outBonusCol, BonusType& outBonusType, int& outBonusColor);
    bool stepDropAndSpawn();

    // Методы, вызываемые из классов-наследников через полиморфизм
    void triggerBombLogic(int targetRow, int targetCol);
    void triggerRecolorLogic(int targetRow, int targetCol, int originalColor);
};

#endif

#ifndef GAME_PLAY_H
#define GAME_PLAY_H

#include <SFML/Graphics.hpp>
#include "Board.h"

enum class GameState { WaitingForInput, SwapAnimation, BonusActivation, DropAnimation };

class GamePlay {
private:
    Board board;
    GameState state;

    int clickCount;
    int row1, col1, row2, col2;
    float swapProgress;
    const float swapSpeed;
    bool isReverting;

    int activeBonusRow, activeBonusCol;
    BonusType activeBonusType;
    int activeBonusColor;

    sf::Clock animationClock;
    const sf::Time frameDelay;
    const sf::Time bonusDelay;

    bool isNeighbor(int r1, int c1, int r2, int c2) const;

public:
    GamePlay();
    void handleInput(const sf::Event& event, sf::RenderWindow& window);
    void update();

    const Board& getBoard() const;
    GameState getState() const;
    void getSelectedCell(int& r, int& c, int& clicks) const;
    void getActiveBonus(int& r, int& c, BonusType& type) const;
};

#endif

#include "BombGems.h"
#include "Board.h"

BombGems::BombGems(int col) : Gems(col) {}
BonusType BombGems::getBonusType() const { return BonusType::Bomb; }
void BombGems::activate(int r, int c, Board& board) {
    board.triggerBombLogic(r, c);
}
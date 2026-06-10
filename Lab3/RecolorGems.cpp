#include "RecolorGems.h"
#include "Board.h"

RecolorGems::RecolorGems(int col) : Gems(col) {}
BonusType RecolorGems::getBonusType() const { return BonusType::Recolor; }
void RecolorGems::activate(int r, int c, Board& board) {
    board.triggerRecolorLogic(r, c, color);
}
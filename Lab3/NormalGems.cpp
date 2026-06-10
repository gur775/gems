#include "NormalGems.h"

NormalGems::NormalGems(int col) : Gems(col) {}
BonusType NormalGems::getBonusType() const { return BonusType::None; }
void NormalGems::activate(int r, int c, Board& board) {}
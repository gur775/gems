#ifndef BOMB_GEMS_H
#define BOMB_GEMS_H

#include "Gems.h"

class BombGems : public Gems {
public:
    BombGems(int col);
    BonusType getBonusType() const override;
    void activate(int r, int c, Board& board) override;
};

#endif

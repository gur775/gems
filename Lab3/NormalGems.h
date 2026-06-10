#ifndef NORMAL_GEMS_H
#define NORMAL_GEMS_H

#include "Gems.h"

class NormalGems : public Gems {
public:
    NormalGems(int col);
    BonusType getBonusType() const override;
    void activate(int r, int c, Board& board) override; // Ничего не делает
};

#endif

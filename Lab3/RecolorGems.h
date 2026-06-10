#ifndef RECOLOR_GEMS_H
#define RECOLOR_GEMS_H

#include "Gems.h"

class RecolorGems : public Gems {
public:
    RecolorGems(int col);
    BonusType getBonusType() const override;
    void activate(int r, int c, Board& board) override;
};

#endif

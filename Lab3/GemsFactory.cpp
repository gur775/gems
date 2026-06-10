#include "GemsFactory.h"
#include "NormalGems.h"
#include "BombGems.h"
#include "RecolorGems.h"

std::unique_ptr<Gems> GemsFactory::createGem(int color, BonusType bonus) {
    switch (bonus) {
    case BonusType::Bomb:    return std::make_unique<BombGems>(color);
    case BonusType::Recolor: return std::make_unique<RecolorGems>(color);
    default:                 return std::make_unique<NormalGems>(color);
    }
}
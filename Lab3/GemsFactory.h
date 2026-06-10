#ifndef GEMS_FACTORY_H
#define GEMS_FACTORY_H

#include <memory>
#include "Gems.h"

class GemsFactory {
public:
    static std::unique_ptr<Gems> createGem(int color, BonusType bonus);
};

#endif

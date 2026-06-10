#include "Gems.h"

Gems::Gems(int col) : color(col), isMatch(false), isEmpty(false), offsetX(0.0f), offsetY(0.0f) {}

int Gems::getColor() const { return color; }
void Gems::setColor(int col) { color = col; }
bool Gems::getIsMatch() const { return isMatch; }
void Gems::setIsMatch(bool match) { isMatch = match; }
bool Gems::getIsEmpty() const { return isEmpty; }
void Gems::setIsEmpty(bool empty) { isEmpty = empty; }
BonusType Gems::getBonusType() const { return BonusType::None; }
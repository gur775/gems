#ifndef GEMS_H
#define GEMS_H

#include <SFML/Graphics.hpp>

enum class BonusType { None, Recolor, Bomb };

class Gems {
protected:
    int color;
    bool isMatch;
    bool isEmpty;
public:
    float offsetX;
    float offsetY;

    Gems(int col);
    virtual ~Gems() = default;

    int getColor() const;
    void setColor(int col);

    bool getIsMatch() const;
    void setIsMatch(bool match);

    bool getIsEmpty() const;
    void setIsEmpty(bool empty);

    virtual BonusType getBonusType() const;
    virtual void activate(int r, int c, class Board& board) = 0;
};

#endif

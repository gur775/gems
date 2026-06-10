#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/Graphics.hpp>
#include "GamePlay.h"

class Renderer {
private:
    sf::Clock pulseClock;
public:
    Renderer() = default;
    void render(sf::RenderWindow& window, const GamePlay& controller);
};

#endif

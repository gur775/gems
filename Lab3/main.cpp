#include <SFML/Graphics.hpp>
#include <optional>
#include "GamePlay.h"
#include "Renderer.h"

int main() {
    sf::RenderWindow window(sf::VideoMode({ 600, 600 }), "GEMS - Visible Bonuses (SFML 3)");
    window.setFramerateLimit(60);

    GamePlay controller;
    Renderer renderer;

    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            controller.handleInput(*event, window);
        }

        controller.update();
        renderer.render(window, controller);
    }

    return 0;
}
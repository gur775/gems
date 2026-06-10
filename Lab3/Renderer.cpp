#include "Renderer.h"

const int TS = 64;
const int OFFSET_X = 48, OFFSET_Y = 48;
const sf::Color gemsColors[] = { sf::Color::Red, sf::Color::Blue, sf::Color::Green, sf::Color::Yellow, sf::Color::Magenta };

void Renderer::render(sf::RenderWindow& window, const GamePlay& controller) {
    window.clear(sf::Color(30, 30, 30));
    const Board& board = controller.getBoard();

    float timeSec = pulseClock.getElapsedTime().asSeconds();
    float pulseScale = 1.0f + 0.25f * std::sin(timeSec * 15.0f);

    int selRow, selCol, clicks;
    controller.getSelectedCell(selRow, selCol, clicks);

    int bonusRow, bonusCol;
    BonusType bonusType;
    controller.getActiveBonus(bonusRow, bonusCol, bonusType);

    for (int r = 0; r < FIELD_HEIGHT; r++) {
        for (int c = 0; c < FIELD_WIDTH; c++) {
            Gems* gems = board.getGems(r, c);
            if (!gems || gems->getIsEmpty()) continue;

            sf::RectangleShape tile(sf::Vector2f({ static_cast<float>(TS - 4), static_cast<float>(TS - 4) }));
            float renderX = c * TS + OFFSET_X + 2 + gems->offsetX;
            float renderY = r * TS + OFFSET_Y + 2 + gems->offsetY;

            tile.setPosition({ renderX, renderY });
            tile.setFillColor(gemsColors[gems->getColor()]);

            if (controller.getState() == GameState::WaitingForInput && clicks == 1 && r == selRow && c == selCol) {
                tile.setOutlineThickness(3);
                tile.setOutlineColor(sf::Color::White);
            }

            window.draw(tile);

            if (gems->getBonusType() != BonusType::None) {
                bool isActiveNow = (controller.getState() == GameState::BonusActivation && r == bonusRow && c == bonusCol);

                if (gems->getBonusType() == BonusType::Bomb) {
                    float radius = 14.0f;
                    if (isActiveNow) radius *= pulseScale;
                    sf::CircleShape bombVisual(radius);
                    bombVisual.setFillColor(sf::Color(255, 255, 255, 220));
                    bombVisual.setOrigin({ radius, radius });
                    bombVisual.setPosition({ renderX + (TS - 4) / 2.0f, renderY + (TS - 4) / 2.0f });
                    if (isActiveNow) {
                        bombVisual.setOutlineThickness(3);
                        bombVisual.setOutlineColor(sf::Color::Red);
                    }
                    window.draw(bombVisual);
                }
                else if (gems->getBonusType() == BonusType::Recolor) {
                    float size = 24.0f;
                    if (isActiveNow) size *= pulseScale;
                    sf::RectangleShape recolorVisual(sf::Vector2f({ size, size }));
                    recolorVisual.setOutlineThickness(3);
                    recolorVisual.setOutlineColor(isActiveNow ? sf::Color::Cyan : sf::Color::Black);
                    recolorVisual.setFillColor(sf::Color::Transparent);
                    recolorVisual.setOrigin({ size / 2.0f, size / 2.0f });
                    recolorVisual.setPosition({ renderX + (TS - 4) / 2.0f, renderY + (TS - 4) / 2.0f });
                    window.draw(recolorVisual);
                }
            }
        }
    }
    window.display();
}
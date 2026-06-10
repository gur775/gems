#include "GamePlay.h"

const int TS = 64;
const int OFFSET_X = 48, OFFSET_Y = 48;

GamePlay::GamePlay()
    : state(GameState::WaitingForInput), clickCount(0),
    row1(-1), col1(-1), row2(-1), col2(-1), swapProgress(0.0f), swapSpeed(8.0f), isReverting(false),
    activeBonusRow(-1), activeBonusCol(-1), activeBonusType(BonusType::None), activeBonusColor(0),
    frameDelay(sf::milliseconds(80)), bonusDelay(sf::milliseconds(600)) {
}

bool GamePlay::isNeighbor(int r1, int c1, int r2, int c2) const {
    return std::abs(r1 - r2) + std::abs(c1 - c2) == 1;
}

const Board& GamePlay::getBoard() const { return board; }
GameState GamePlay::getState() const { return state; }
void GamePlay::getSelectedCell(int& r, int& c, int& clicks) const { r = row1; c = col1; clicks = clickCount; }
void GamePlay::getActiveBonus(int& r, int& c, BonusType& type) const { r = activeBonusRow; c = activeBonusCol; type = activeBonusType; }

void GamePlay::handleInput(const sf::Event& event, sf::RenderWindow& window) {
    if (state != GameState::WaitingForInput) return;

    if (const auto* mouseClick = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouseClick->button == sf::Mouse::Button::Left) {
            int mouseX = mouseClick->position.x - OFFSET_X;
            int mouseY = mouseClick->position.y - OFFSET_Y;
            int col = mouseX / TS;
            int row = mouseY / TS;

            if (row >= 0 && row < FIELD_HEIGHT && col >= 0 && col < FIELD_WIDTH) {
                if (clickCount == 0) {
                    row1 = row; col1 = col; clickCount = 1;
                }
                else if (clickCount == 1) {
                    row2 = row; col2 = col;
                    if (isNeighbor(row1, col1, row2, col2)) {
                        state = GameState::SwapAnimation;
                        swapProgress = 0.0f;
                    }
                    else {
                        row1 = row; col1 = col; clickCount = 1;
                    }
                }
            }
        }
    }
}

void GamePlay::update() {
    if (state == GameState::SwapAnimation) {
        swapProgress += swapSpeed;
        float dirX = static_cast<float>(col2 - col1) * TS;
        float dirY = static_cast<float>(row2 - row1) * TS;

        if (swapProgress <= 100.0f) {
            float ratio = swapProgress / 100.0f;
            Gems* g1 = board.getGems(row1, col1);
            Gems* g2 = board.getGems(row2, col2);
            if (g1 && g2) {
                float factor = isReverting ? (1.0f - ratio) : ratio;
                g1->offsetX = dirX * factor; g1->offsetY = dirY * factor;
                g2->offsetX = -dirX * factor; g2->offsetY = -dirY * factor;
            }
        }
        else {
            Gems* g1 = board.getGems(row1, col1);
            Gems* g2 = board.getGems(row2, col2);
            if (g1 && g2) { g1->offsetX = 0; g1->offsetY = 0; g2->offsetX = 0; g2->offsetY = 0; }

            if (!isReverting) {
                board.swapGemsInMatrix(row1, col1, row2, col2);
                if (board.findMatches(activeBonusRow, activeBonusCol, activeBonusType, activeBonusColor)) {
                    state = (activeBonusType != BonusType::None) ? GameState::BonusActivation : GameState::DropAnimation;
                    animationClock.restart();
                    clickCount = 0;
                }
                else {
                    board.swapGemsInMatrix(row1, col1, row2, col2);
                    swapProgress = 0.0f;
                    isReverting = true;
                }
            }
            else {
                isReverting = false;
                state = GameState::WaitingForInput;
                clickCount = 0;
            }
        }
    }

    if (state == GameState::BonusActivation) {
        if (animationClock.getElapsedTime() >= bonusDelay) {
            Gems* activeBonus = board.getGems(activeBonusRow, activeBonusCol);
            if (activeBonus) {
                activeBonus->activate(activeBonusRow, activeBonusCol, board);
            }
            activeBonusType = BonusType::None;
            state = GameState::DropAnimation;
            animationClock.restart();
        }
    }

    if (state == GameState::DropAnimation) {
        if (animationClock.getElapsedTime() >= frameDelay) {
            bool fieldChanged = board.stepDropAndSpawn();
            animationClock.restart();
            if (!fieldChanged) {
                if (board.findMatches(activeBonusRow, activeBonusCol, activeBonusType, activeBonusColor)) {
                    state = (activeBonusType != BonusType::None) ? GameState::BonusActivation : GameState::DropAnimation;
                }
                else {
                    state = GameState::WaitingForInput;
                }
            }
        }
    }
}
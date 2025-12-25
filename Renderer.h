/**
 * Renderer.hpp
 * Main rendering system for the game
 * Handles drawing cards, hands, game state, and UI
 */

#ifndef RENDERER_H
#define RENDERER_H

#include "Card.h"
#include "Hand.h"
#include "CardSprite.h"
#include "UIElements.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <memory>

 /**
  * Main renderer class
  */
class Renderer {
public:
    /**
     * Constructor
     */
    Renderer(sf::RenderWindow& window);

    /**
     * Initialize renderer (load resources, etc.)
     */
    bool initialize();

    /**
     * Clear the screen
     */
    void clear();

    /**
     * Present/display the rendered frame
     */
    void present();

    /**
     * Draw a single card
     */
    void drawCard(const Card& card, float x, float y, bool highlighted = false, bool faceUp = true);

    /**
     * Draw a hand of cards (horizontally arranged)
     */
    void drawHand(const Hand& hand, float x, float y, bool faceUp = true,
        const std::vector<bool>& highlighted = {});

    /**
     * Draw a hand in an arc (for better visual appeal)
     */
    void drawHandArc(const Hand& hand, float centerX, float centerY, float radius,
        bool faceUp = true, const std::vector<bool>& highlighted = {});

    /**
     * Draw text on screen
     */
    void drawText(const std::string& text, float x, float y,
        unsigned int size = 24, const sf::Color& color = sf::Color::White,
        TextAlign align = TextAlign::Left);

    /**
     * Draw game status/info panel
     */
    void drawStatusPanel(const std::string& status, float x, float y, float width, float height);

    /**
     * Draw player info (name, card count)
     */
    void drawPlayerInfo(const std::string& name, int cardCount, float x, float y, bool active = false);

    /**
     * Draw the center play area (shows last played cards)
     */
    void drawPlayArea(const std::vector<Card>& cards, float x, float y);

    /**
     * Draw a button
     */
    void drawButton(const std::string& label, float x, float y, float width, float height,
        bool hovered = false);

    /**
     * Get window dimensions
     */
    float getWindowWidth() const;
    float getWindowHeight() const;

    /**
     * Layout helpers - calculate common positions
     */
    float getCenterX() const { return getWindowWidth() / 2.0f; }
    float getCenterY() const { return getWindowHeight() / 2.0f; }
    float getBottomY() const { return getWindowHeight() - 20.0f; }
    float getTopY() const { return 20.0f; }

private:
    sf::RenderWindow& window_;

    /**
     * Calculate card spacing for a hand
     */
    float calculateCardSpacing(size_t numCards, float maxWidth) const;

    /**
     * Draw card back texture
     */
    void drawCardBack(float x, float y);
};

#endif // RENDERER_HPP

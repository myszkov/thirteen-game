/**
 * CardSprite.hpp
 * Visual representation of a playing card
 */

#ifndef CARDSPRITE_H
#define CARDSPRITE_H

#include "Card.h"
#include <SFML/Graphics.hpp>
#include <memory>

 /**
  * Visual representation of a card
  * Draws cards without needing external texture files
  */
class CardSprite {
public:
    /**
     * Card dimensions
     */
    static constexpr float CARD_WIDTH = 80.0f;
    static constexpr float CARD_HEIGHT = 120.0f;
    static constexpr float CORNER_RADIUS = 8.0f;

    /**
     * Constructor
     */
    CardSprite(const Card& card, float x = 0.0f, float y = 0.0f);

    /**
     * Draw the card at its position
     */
    void draw(sf::RenderWindow& window) const;

    /**
     * Draw the card at a specific position (overrides stored position)
     */
    void drawAt(sf::RenderWindow& window, float x, float y) const;

    /**
     * Position setters/getters
     */
    void setPosition(float x, float y) { x_ = x; y_ = y; }
    float getX() const { return x_; }
    float getY() const { return y_; }

    /**
     * Get the card this sprite represents
     */
    const Card& getCard() const { return card_; }

    /**
     * Check if a point is inside the card bounds
     */
    bool contains(float x, float y) const;

    /**
     * Highlighting (for selection)
     */
    void setHighlighted(bool highlighted) { highlighted_ = highlighted; }
    bool isHighlighted() const { return highlighted_; }

    /**
     * Face up/down
     */
    void setFaceUp(bool faceUp) { faceUp_ = faceUp; }
    bool isFaceUp() const { return faceUp_; }

    /**
     * Scale factor
     */
    void setScale(float scale) { scale_ = scale; }
    float getScale() const { return scale_; }

    /**
     * Get card bounds
     */
    sf::FloatRect getBounds() const;

private:
    Card card_;
    float x_;
    float y_;
    bool highlighted_;
    bool faceUp_;
    float scale_;

    /**
     * Draw card face (front)
     */
    void drawFace(sf::RenderWindow& window, float x, float y) const;

    /**
     * Draw card back
     */
    void drawBack(sf::RenderWindow& window, float x, float y) const;

    /**
     * Draw the card background (white rounded rectangle)
     */
    void drawCardBackground(sf::RenderWindow& window, float x, float y) const;

    /**
     * Draw rank symbol in corners
     */
    void drawRankInCorner(sf::RenderWindow& window, float x, float y, bool topLeft) const;

    /**
     * Draw large suit symbol in center
     */
    void drawCenterSuit(sf::RenderWindow& window, float x, float y) const;

    /**
     * Get color for the card's suit
     */
    sf::Color getSuitColor() const;
};

#endif // CARDSPRITE_H

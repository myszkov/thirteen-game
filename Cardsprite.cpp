/**
 * CardSprite.cpp
 * Implementation of visual card representation
 */

#include "CardSprite.h"
#include "UIElements.h"
#include <cmath>

 /**
  * Constructor
  */
CardSprite::CardSprite(const Card& card, float x, float y)
    : card_(card), x_(x), y_(y), highlighted_(false), faceUp_(true), scale_(1.0f) {
}

/**
 * Draw the card at its stored position
 */
void CardSprite::draw(sf::RenderWindow& window) const {
    drawAt(window, x_, y_);
}

/**
 * Draw the card at a specific position
 */
void CardSprite::drawAt(sf::RenderWindow& window, float x, float y) const {
    float width = CARD_WIDTH * scale_;
    float height = CARD_HEIGHT * scale_;

    // Draw highlight if selected
    if (highlighted_) {
        UIElements::drawRoundedRect(
            window,
            x - 4, y - 4,
            width + 8, height + 8,
            CORNER_RADIUS + 2,
            ColorScheme::HighlightYellow,
            ColorScheme::HighlightYellow,
            3.0f
        );
    }

    // Draw card face or back
    if (faceUp_) {
        drawFace(window, x, y);
    }
    else {
        drawBack(window, x, y);
    }
}

/**
 * Draw card face (front)
 */
void CardSprite::drawFace(sf::RenderWindow& window, float x, float y) const {
    float width = CARD_WIDTH * scale_;
    float height = CARD_HEIGHT * scale_;

    // Draw card background
    drawCardBackground(window, x, y);

    // Draw rank and suit in top-left corner
    drawRankInCorner(window, x, y, true);

    // Draw rank and suit in bottom-right corner (rotated)
    drawRankInCorner(window, x, y, false);

    // Draw large suit symbol in center
    drawCenterSuit(window, x, y);
}

/**
 * Draw card back
 */
void CardSprite::drawBack(sf::RenderWindow& window, float x, float y) const {
    float width = CARD_WIDTH * scale_;
    float height = CARD_HEIGHT * scale_;

    // Draw card background
    UIElements::drawRoundedRect(
        window,
        x, y, width, height,
        CORNER_RADIUS * scale_,
        sf::Color(50, 50, 150),  // Blue back
        ColorScheme::CardBorder,
        2.0f * scale_
    );

    // Draw pattern on back
    UIElements::drawRoundedRect(
        window,
        x + 8 * scale_, y + 8 * scale_,
        width - 16 * scale_, height - 16 * scale_,
        (CORNER_RADIUS - 2) * scale_,
        sf::Color(80, 80, 180),
        sf::Color(100, 100, 200),
        1.0f * scale_
    );
}

/**
 * Draw the card background
 */
void CardSprite::drawCardBackground(sf::RenderWindow& window, float x, float y) const {
    float width = CARD_WIDTH * scale_;
    float height = CARD_HEIGHT * scale_;

    UIElements::drawRoundedRect(
        window,
        x, y, width, height,
        CORNER_RADIUS * scale_,
        ColorScheme::CardWhite,
        ColorScheme::CardBorder,
        2.0f * scale_
    );
}

/**
 * Draw rank and suit in corner
 */
void CardSprite::drawRankInCorner(sf::RenderWindow& window, float x, float y, bool topLeft) const {
    float cardWidth = CARD_WIDTH * scale_;
    float cardHeight = CARD_HEIGHT * scale_;
    unsigned int fontSize = static_cast<unsigned int>(18 * scale_);

    // Get rank string
    std::string rankStr;
    if (card_.getRank() == Rank::Ten) {
        rankStr = "10";
    }
    else {
        rankStr = card_.getRankChar();
    }

    // Get suit symbol
    std::string suitStr = card_.getSuitSymbol();

    sf::Color color = getSuitColor();

    if (topLeft) {
        // Top-left corner
        float cornerX = x + 8 * scale_;
        float cornerY = y + 8 * scale_;

        UIElements::drawText(window, rankStr, cornerX, cornerY, fontSize, color, TextAlign::Left);
        UIElements::drawText(window, suitStr, cornerX, cornerY + 18 * scale_, fontSize, color, TextAlign::Left);
    }
    else {
        // Bottom-right corner (rotated 180 degrees)
        float cornerX = x + cardWidth - 8 * scale_;
        float cornerY = y + cardHeight - 8 * scale_;

        // For simplicity, we'll draw upside-down text by drawing at bottom-right
        UIElements::drawText(window, rankStr, cornerX, cornerY - 18 * scale_, fontSize, color, TextAlign::Right);
        UIElements::drawText(window, suitStr, cornerX, cornerY - 36 * scale_, fontSize, color, TextAlign::Right);
    }
}

/**
 * Draw large suit symbol in center
 */
void CardSprite::drawCenterSuit(sf::RenderWindow& window, float x, float y) const {
    float cardWidth = CARD_WIDTH * scale_;
    float cardHeight = CARD_HEIGHT * scale_;
    unsigned int fontSize = static_cast<unsigned int>(48 * scale_);

    std::string suitStr = card_.getSuitSymbol();
    sf::Color color = getSuitColor();

    // Draw large suit in center
    float centerX = x + cardWidth / 2.0f;
    float centerY = y + cardHeight / 2.0f - 24 * scale_;

    UIElements::drawText(window, suitStr, centerX, centerY, fontSize, color, TextAlign::Center);

    // Draw rank below suit
    std::string rankStr;
    if (card_.getRank() == Rank::Ten) {
        rankStr = "10";
    }
    else {
        rankStr = card_.getRankChar();
    }

    UIElements::drawText(window, rankStr, centerX, centerY + 48 * scale_,
        static_cast<unsigned int>(36 * scale_), color, TextAlign::Center);
}

/**
 * Get color for the card's suit
 */
sf::Color CardSprite::getSuitColor() const {
    Suit suit = card_.getSuit();
    if (suit == Suit::Hearts || suit == Suit::Diamonds) {
        return ColorScheme::RedSuit;
    }
    else {
        return ColorScheme::BlackSuit;
    }
}

/**
 * Check if a point is inside the card bounds
 */
bool CardSprite::contains(float x, float y) const {
    float width = CARD_WIDTH * scale_;
    float height = CARD_HEIGHT * scale_;
    return x >= x_ && x <= x_ + width && y >= y_ && y <= y_ + height;
}

/**
 * Get card bounds
 */
sf::FloatRect CardSprite::getBounds() const {
    return sf::FloatRect(sf::Vector2f(x_, y_), sf::Vector2f(CARD_WIDTH * scale_, CARD_HEIGHT * scale_));
}
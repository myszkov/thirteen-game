/**
 * Renderer.cpp
 * Implementation of the main rendering system
 */

#include "Renderer.h"
#include <cmath>
#include <algorithm>

 /**
  * Constructor
  */
Renderer::Renderer(sf::RenderWindow& window) : window_(window) {
}

/**
 * Initialize renderer
 */
bool Renderer::initialize() {
    return UIElements::initialize();
}

/**
 * Clear the screen
 */
void Renderer::clear() {
    window_.clear(ColorScheme::TableGreen);
}

/**
 * Present/display the rendered frame
 */
void Renderer::present() {
    window_.display();
}

/**
 * Draw a single card
 */
void Renderer::drawCard(const Card& card, float x, float y, bool highlighted, bool faceUp) {
    CardSprite sprite(card, x, y);
    sprite.setHighlighted(highlighted);
    sprite.setFaceUp(faceUp);
    sprite.draw(window_);
}

/**
 * Draw a hand of cards horizontally
 */
void Renderer::drawHand(const Hand& hand, float x, float y, bool faceUp,
    const std::vector<bool>& highlighted) {
    if (hand.isEmpty()) {
        return;
    }

    const auto& cards = hand.getCards();
    float spacing = calculateCardSpacing(cards.size(), getWindowWidth() - 100);

    for (size_t i = 0; i < cards.size(); ++i) {
        bool isHighlighted = i < highlighted.size() ? highlighted[i] : false;
        float cardX = x + i * spacing;
        drawCard(cards[i], cardX, y, isHighlighted, faceUp);
    }
}

/**
 * Draw a hand in an arc
 */
void Renderer::drawHandArc(const Hand& hand, float centerX, float centerY, float radius,
    bool faceUp, const std::vector<bool>& highlighted) {
    if (hand.isEmpty()) {
        return;
    }

    const auto& cards = hand.getCards();
    size_t numCards = cards.size();

    // Calculate arc angle (max 120 degrees)
    float maxAngle = 120.0f;
    float totalAngle = std::min(maxAngle, static_cast<float>(numCards) * 8.0f);
    float angleStep = numCards > 1 ? totalAngle / (numCards - 1) : 0;
    float startAngle = -totalAngle / 2.0f;

    for (size_t i = 0; i < numCards; ++i) {
        // Calculate angle for this card
        float angle = startAngle + i * angleStep;
        float radians = angle * 3.14159f / 180.0f;

        // Calculate position
        float cardX = centerX + radius * std::sin(radians) - CardSprite::CARD_WIDTH / 2.0f;
        float cardY = centerY - radius * std::cos(radians);

        bool isHighlighted = i < highlighted.size() ? highlighted[i] : false;
        drawCard(cards[i], cardX, cardY, isHighlighted, faceUp);
    }
}

/**
 * Draw text on screen
 */
void Renderer::drawText(const std::string& text, float x, float y,
    unsigned int size, const sf::Color& color, TextAlign align) {
    UIElements::drawText(window_, text, x, y, size, color, align);
}

/**
 * Draw game status panel
 */
void Renderer::drawStatusPanel(const std::string& status, float x, float y,
    float width, float height) {
    // Draw panel background
    UIElements::drawRoundedRect(window_, x, y, width, height, 8.0f,
        sf::Color(40, 40, 40, 200),
        sf::Color(100, 100, 100),
        2.0f);

    // Draw status text
    drawText(status, x + width / 2.0f, y + height / 2.0f - 12, 20,
        ColorScheme::TextWhite, TextAlign::Center);
}

/**
 * Draw player info
 */
void Renderer::drawPlayerInfo(const std::string& name, int cardCount,
    float x, float y, bool active) {
    // Background color depends on if player is active
    sf::Color bgColor = active ? sf::Color(80, 120, 80, 200) : sf::Color(60, 60, 60, 200);

    // Draw background panel
    float width = 200.0f;
    float height = 60.0f;
    UIElements::drawRoundedRect(window_, x, y, width, height, 8.0f,
        bgColor, sf::Color(120, 120, 120), 2.0f);

    // Draw player name
    drawText(name, x + 10, y + 10, 18, ColorScheme::TextWhite);

    // Draw card count
    std::string cardCountStr = "Cards: " + std::to_string(cardCount);
    drawText(cardCountStr, x + 10, y + 35, 16, ColorScheme::TextWhite);
}

/**
 * Draw the center play area
 */
void Renderer::drawPlayArea(const std::vector<Card>& cards, float x, float y) {
    if (cards.empty()) {
        // Draw empty play area
        float width = 400.0f;
        float height = 150.0f;
        UIElements::drawRoundedRect(window_, x - width / 2, y - height / 2,
            width, height, 12.0f,
            sf::Color(30, 90, 30, 150),
            sf::Color(80, 80, 80),
            2.0f);

        drawText("No cards played yet", x, y - 10, 20,
            sf::Color(200, 200, 200), TextAlign::Center);
        return;
    }

    // Draw the played cards
    float spacing = 85.0f;  // Slight overlap
    float startX = x - (cards.size() * spacing) / 2.0f;

    for (size_t i = 0; i < cards.size(); ++i) {
        float cardX = startX + i * spacing;
        float cardY = y - CardSprite::CARD_HEIGHT / 2.0f;
        drawCard(cards[i], cardX, cardY, false, true);
    }
}

/**
 * Draw a button
 */
void Renderer::drawButton(const std::string& label, float x, float y,
    float width, float height, bool hovered) {
    Button button(x, y, width, height, label);
    button.setHovered(hovered);
    button.draw(window_);
}

/**
 * Get window dimensions
 */
float Renderer::getWindowWidth() const {
    return static_cast<float>(window_.getSize().x);
}

float Renderer::getWindowHeight() const {
    return static_cast<float>(window_.getSize().y);
}

/**
 * Calculate card spacing for a hand
 */
float Renderer::calculateCardSpacing(size_t numCards, float maxWidth) const {
    if (numCards == 0) return 0;
    if (numCards == 1) return 0;

    // Calculate spacing needed
    float totalCardWidth = numCards * CardSprite::CARD_WIDTH;
    float availableSpace = maxWidth - CardSprite::CARD_WIDTH;

    // If cards fit with full spacing, use full card width
    if (totalCardWidth <= maxWidth) {
        return CardSprite::CARD_WIDTH + 10.0f;
    }

    // Otherwise, overlap cards
    float spacing = availableSpace / (numCards - 1);
    return std::max(30.0f, spacing);  // Minimum spacing of 30 pixels
}
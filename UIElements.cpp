/**
 * UIElements.cpp
 * Implementation of UI utility functions
 */

#include "UIElements.h"
#include <cmath>

 // Initialize static members
sf::Font UIElements::font_;
bool UIElements::fontLoaded_ = false;

// Color scheme constants
const sf::Color ColorScheme::TableGreen = sf::Color(20, 120, 20);
const sf::Color ColorScheme::CardWhite = sf::Color(250, 250, 250);
const sf::Color ColorScheme::CardBorder = sf::Color(40, 40, 40);
const sf::Color ColorScheme::TextBlack = sf::Color(20, 20, 20);
const sf::Color ColorScheme::TextWhite = sf::Color(250, 250, 250);
const sf::Color ColorScheme::RedSuit = sf::Color(220, 20, 20);
const sf::Color ColorScheme::BlackSuit = sf::Color(20, 20, 20);
const sf::Color ColorScheme::HighlightYellow = sf::Color(255, 220, 0);
const sf::Color ColorScheme::ShadowGray = sf::Color(0, 0, 0, 100);

/**
 * Initialize UI system
 */
bool UIElements::initialize() {
    if (!fontLoaded_) {
        fontLoaded_ = loadFont();
    }
    return fontLoaded_;
}

/**
 * Load default font
 */
bool UIElements::loadFont() {
    // Try to load from common system font locations
    const std::vector<std::string> fontPaths = {
        "assets/fonts/arial.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/System/Library/Fonts/Helvetica.ttc",
        "C:/Windows/Fonts/arial.ttf"
    };

    for (const auto& path : fontPaths) {
        if (font_.openFromFile(path)) {
            return true;
        }
    }

    // If no font file found, SFML will use a built-in fallback
    // This is acceptable for our purposes
    return true;
}

/**
 * Get the default font
 */
const sf::Font& UIElements::getFont() {
    if (!fontLoaded_) {
        initialize();
    }
    return font_;
}

/**
 * Draw text with alignment
 */
void UIElements::drawText(
    sf::RenderWindow& window,
    const std::string& text,
    float x, float y,
    unsigned int size,
    const sf::Color& color,
    TextAlign align
) {
    sf::Text sfText = createText(text, x, y, size, color);

    // Adjust position based on alignment
    sf::FloatRect bounds = sfText.getLocalBounds();

    switch (align) {
    case TextAlign::Center:
        sfText.setOrigin(sf::Vector2f(bounds.size.x / 2.0f, 0));
        break;
    case TextAlign::Right:
        sfText.setOrigin(sf::Vector2f(bounds.size.x, 0));
        break;
    case TextAlign::Left:
    default:
        // No adjustment needed
        break;
    }

    window.draw(sfText);
}

/**
 * Draw a rounded rectangle
 */
void UIElements::drawRoundedRect(
    sf::RenderWindow& window,
    float x, float y, float width, float height,
    float radius,
    const sf::Color& fillColor,
    const sf::Color& outlineColor,
    float outlineThickness
) {
    // Clamp radius to reasonable values
    radius = std::min(radius, std::min(width, height) / 2.0f);

    // Create a shape with rounded corners
    sf::CircleShape corner(radius);
    corner.setFillColor(fillColor);
    corner.setOutlineColor(outlineColor);
    corner.setOutlineThickness(outlineThickness);

    // Draw four corners
    corner.setPosition(sf::Vector2f(x, y));
    window.draw(corner);

    corner.setPosition(sf::Vector2f(x + width - 2 * radius, y));
    window.draw(corner);

    corner.setPosition(sf::Vector2f(x, y + height - 2 * radius));
    window.draw(corner);

    corner.setPosition(sf::Vector2f(x + width - 2 * radius, y + height - 2 * radius));
    window.draw(corner);

    // Draw rectangles to fill the spaces
    drawRect(window, x + radius, y, width - 2 * radius, height, fillColor, outlineColor, outlineThickness);
    drawRect(window, x, y + radius, radius, height - 2 * radius, fillColor, outlineColor, outlineThickness);
    drawRect(window, x + width - radius, y + radius, radius, height - 2 * radius, fillColor, outlineColor, outlineThickness);
}

/**
 * Draw a simple rectangle
 */
void UIElements::drawRect(
    sf::RenderWindow& window,
    float x, float y, float width, float height,
    const sf::Color& fillColor,
    const sf::Color& outlineColor,
    float outlineThickness
) {
    sf::RectangleShape rect(sf::Vector2f(width, height));
    rect.setPosition(sf::Vector2f(x, y));
    rect.setFillColor(fillColor);
    rect.setOutlineColor(outlineColor);
    rect.setOutlineThickness(outlineThickness);
    window.draw(rect);
}

/**
 * Draw a line
 */
void UIElements::drawLine(
    sf::RenderWindow& window,
    float x1, float y1, float x2, float y2,
    const sf::Color& color,
    float thickness
) {
    // For thicker lines, draw a rectangle
    if (thickness > 1.0f) {
        float dx = x2 - x1;
        float dy = y2 - y1;
        float length = std::sqrt(dx * dx + dy * dy);
        float angle = std::atan2(dy, dx) * 180.0f / 3.14159f;

        sf::RectangleShape rect(sf::Vector2f(length, thickness));
        rect.setPosition(sf::Vector2f(x1, y1));
        rect.setRotation(sf::degrees(angle));
        rect.setFillColor(color);
        rect.setOrigin(sf::Vector2f(0, thickness / 2.0f));
        window.draw(rect);
    }
    else {
        // For thin lines, use vertex array
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(x1, y1), color),
            sf::Vertex(sf::Vector2f(x2, y2), color)
        };
        window.draw(line, 2, sf::PrimitiveType::Lines);
    }
}

/**
 * Get text bounds
 */
sf::FloatRect UIElements::getTextBounds(const std::string& text, unsigned int size) {
    sf::Text sfText = createText(text, 0, 0, size);
    return sfText.getLocalBounds();
}

/**
 * Create a text object
 */
sf::Text UIElements::createText(
    const std::string& text,
    float x, float y,
    unsigned int size,
    const sf::Color& color
) {
    sf::Text sfText(getFont());
    sfText.setString(text);
    sfText.setCharacterSize(size);
    sfText.setFillColor(color);
    sfText.setPosition(sf::Vector2f(x, y));
    return sfText;
}

// ===== Button Implementation =====

Button::Button(float x, float y, float width, float height, const std::string& label)
    : x_(x), y_(y), width_(width), height_(height), label_(label), hovered_(false) {
}

void Button::draw(sf::RenderWindow& window) {
    // Draw button background
    sf::Color bgColor = hovered_ ? sf::Color(100, 150, 100) : sf::Color(60, 120, 60);
    UIElements::drawRoundedRect(window, x_, y_, width_, height_, 8.0f,
        bgColor, sf::Color::White, 2.0f);

    // Draw button label
    UIElements::drawText(window, label_, x_ + width_ / 2.0f, y_ + height_ / 2.0f - 12.0f,
        20, sf::Color::White, TextAlign::Center);
}

bool Button::contains(float x, float y) const {
    return x >= x_ && x <= x_ + width_ && y >= y_ && y <= y_ + height_;
}
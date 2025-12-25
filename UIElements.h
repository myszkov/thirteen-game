/**
 * UIElements.hpp
 * Utility functions and classes for drawing UI elements in SFML
 */

#ifndef UIELEMENTS_H
#define UIELEMENTS_H

#include <SFML/Graphics.hpp>
#include <string>
#include <memory>

 /**
  * Text alignment options
  */
enum class TextAlign {
    Left,
    Center,
    Right
};

/**
 * UI color scheme for the game
 */
struct ColorScheme {
    static const sf::Color TableGreen;
    static const sf::Color CardWhite;
    static const sf::Color CardBorder;
    static const sf::Color TextBlack;
    static const sf::Color TextWhite;
    static const sf::Color RedSuit;
    static const sf::Color BlackSuit;
    static const sf::Color HighlightYellow;
    static const sf::Color ShadowGray;
};

/**
 * UI utility functions
 */
class UIElements {
public:
    /**
     * Initialize UI system (load fonts, etc.)
     * @return true if successful
     */
    static bool initialize();

    /**
     * Get the default font
     */
    static const sf::Font& getFont();

    /**
     * Draw text at a position with alignment
     */
    static void drawText(
        sf::RenderWindow& window,
        const std::string& text,
        float x, float y,
        unsigned int size = 24,
        const sf::Color& color = sf::Color::White,
        TextAlign align = TextAlign::Left
    );

    /**
     * Draw a rounded rectangle
     */
    static void drawRoundedRect(
        sf::RenderWindow& window,
        float x, float y, float width, float height,
        float radius,
        const sf::Color& fillColor,
        const sf::Color& outlineColor = sf::Color::Transparent,
        float outlineThickness = 0.0f
    );

    /**
     * Draw a simple rectangle
     */
    static void drawRect(
        sf::RenderWindow& window,
        float x, float y, float width, float height,
        const sf::Color& fillColor,
        const sf::Color& outlineColor = sf::Color::Transparent,
        float outlineThickness = 0.0f
    );

    /**
     * Draw a line
     */
    static void drawLine(
        sf::RenderWindow& window,
        float x1, float y1, float x2, float y2,
        const sf::Color& color,
        float thickness = 1.0f
    );

    /**
     * Get text bounds (useful for centering)
     */
    static sf::FloatRect getTextBounds(
        const std::string& text,
        unsigned int size = 24
    );

    /**
     * Create a text object (for more control)
     */
    static sf::Text createText(
        const std::string& text,
        float x, float y,
        unsigned int size = 24,
        const sf::Color& color = sf::Color::White
    );

private:
    static sf::Font font_;
    static bool fontLoaded_;

    /**
     * Load default font (creates a basic font if file not found)
     */
    static bool loadFont();
};

/**
 * Button class for clickable UI elements
 */
class Button {
public:
    Button(float x, float y, float width, float height, const std::string& label);

    void draw(sf::RenderWindow& window);
    bool contains(float x, float y) const;
    bool isHovered() const { return hovered_; }
    void setHovered(bool hovered) { hovered_ = hovered; }

    const std::string& getLabel() const { return label_; }
    void setLabel(const std::string& label) { label_ = label; }

private:
    float x_, y_, width_, height_;
    std::string label_;
    bool hovered_;
};

#endif // UIELEMENTS_HPP
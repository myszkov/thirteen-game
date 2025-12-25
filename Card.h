/**
 * Card.h
 * Represents a single playing card with rank and suit
 * In Big Two: 2 is the highest rank, Spades is the highest suit
 * In Thirteen: 2 is the highest rank, Spades is the lowest suit
 */

#ifndef CARD_H
#define CARD_H

#include <string>
#include <ostream>

enum class Rank {
    Three = 3,
    Four = 4,
    Five = 5,
    Six = 6,
    Seven = 7,
    Eight = 8,
    Nine = 9,
    Ten = 10,
    Jack = 11,
    Queen = 12,
    King = 13,
    Ace = 14,
    Two = 15  // Highest rank in Thirteen
};

enum class Suit {
    Diamonds = 0,  // Lowest suit (Big two), Highest suit (Thirteen)
    Clubs = 1,
    Hearts = 2,
    Spades = 3     // Highest suit (Big two), Lowest suit (Thirteen)
};

class Card {
public:
    /**
     * Constructors
     */
    Card(Rank rank, Suit suit);
    Card(const std::string& cardStr);  // e.g., "3H", "AS", "10D"

    /**
     * Getters
     */
    Rank getRank() const { return rank_; }
    Suit getSuit() const { return suit_; }

    /**
     * String representations
     */
    std::string toString() const;           // e.g., "3H", "10S"
    std::string toDisplayString() const;    // e.g., "3♥", "10♠"
    char getRankChar() const;               // '3', 'J', 'A', etc.
    char getSuitChar() const;               // 'H', 'S', 'D', 'C'
    std::string getSuitSymbol() const;      // "♥", "♠", "♦", "♣"

    /**
     * Comparison operators
     * In Big Two, cards are compared first by rank, then by suit
     */
    bool operator==(const Card& other) const;
    bool operator!=(const Card& other) const;
    bool operator<(const Card& other) const;   // For sorting
    bool operator>(const Card& other) const;
    bool operator<=(const Card& other) const;
    bool operator>=(const Card& other) const;

    /**
     * Static utility functions
     */
    static Rank charToRank(char c);
    static Suit charToSuit(char c);
    static char rankToChar(Rank rank);
    static char suitToChar(Suit suit);
    static std::string suitToSymbol(Suit suit);

private:
    Rank rank_;
    Suit suit_;
};

/**
 * Output stream operator for easy printing
 */
std::ostream& operator<<(std::ostream& os, const Card& card);

#endif // CARD_HPP
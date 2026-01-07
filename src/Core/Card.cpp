/**
 * Card.cpp
 * Implementation of Card class
 */

#include "Card.h"
#include <stdexcept>
#include <cctype>

 /**
  * Constructor with rank and suit
  */
Card::Card(Rank rank, Suit suit) : rank_(rank), suit_(suit) {}

/**
 * Constructor from string (e.g., "3H", "AS", "10D")
 */
Card::Card(const std::string& cardStr) {
    if (cardStr.length() < 2) {
        throw std::invalid_argument("Invalid card string: " + cardStr);
    }

    // Parse rank (can be 1 or 2 characters for "10")
    std::string rankStr;
    size_t suitPos;

    if (cardStr.length() >= 3 && cardStr.substr(0, 2) == "10") {
        rankStr = "10";
        suitPos = 2;
    }
    else {
        rankStr = cardStr.substr(0, 1);
        suitPos = 1;
    }

    // Parse suit (last character)
    if (suitPos >= cardStr.length()) {
        throw std::invalid_argument("Invalid card string: " + cardStr);
    }

    rank_ = charToRank(rankStr[0]);
    if (rankStr == "10") {
        rank_ = Rank::Ten;
    }

    suit_ = charToSuit(cardStr[suitPos]);
}

/**
 * Convert to string (e.g., "3H", "10S")
 */
std::string Card::toString() const {
    std::string result;
    result += getRankChar();
    result += getSuitChar();
    return result;
}

/**
 * Convert to display string with unicode symbols (e.g., "3♥", "10♠")
 */
std::string Card::toDisplayString() const {
    std::string result;

    // Add rank
    if (rank_ == Rank::Ten) {
        result += "10";
    }
    else {
        result += getRankChar();
    }

    // Add suit symbol
    result += getSuitSymbol();

    return result;
}

/**
 * Get rank as character
 */
char Card::getRankChar() const {
    return rankToChar(rank_);
}

/**
 * Get suit as character (H, S, D, C)
 */
char Card::getSuitChar() const {
    return suitToChar(suit_);
}

/**
 * Get suit as unicode symbol
 */
std::string Card::getSuitSymbol() const {
    return suitToSymbol(suit_);
}

/**
 * Comparison operators
 * In Big Two: compare by rank first, then by suit if ranks are equal
 */
bool Card::operator==(const Card& other) const {
    return rank_ == other.rank_ && suit_ == other.suit_;
}

bool Card::operator!=(const Card& other) const {
    return !(*this == other);
}

bool Card::operator<(const Card& other) const {
    if (rank_ != other.rank_) {
        return rank_ < other.rank_;
    }
    return suit_ < other.suit_;
}

bool Card::operator>(const Card& other) const {
    return other < *this;
}

bool Card::operator<=(const Card& other) const {
    return !(other < *this);
}

bool Card::operator>=(const Card& other) const {
    return !(*this < other);
}

/**
 * Static utility functions
 */
Rank Card::charToRank(char c) {
    c = std::toupper(c);
    switch (c) {
    case '3': return Rank::Three;
    case '4': return Rank::Four;
    case '5': return Rank::Five;
    case '6': return Rank::Six;
    case '7': return Rank::Seven;
    case '8': return Rank::Eight;
    case '9': return Rank::Nine;
    case 'T':
    case '1': return Rank::Ten;  // Allow both 'T' and '1' (from "10")
    case 'J': return Rank::Jack;
    case 'Q': return Rank::Queen;
    case 'K': return Rank::King;
    case 'A': return Rank::Ace;
    case '2': return Rank::Two;
    default:
        throw std::invalid_argument(std::string("Invalid rank character: ") + c);
    }
}

Suit Card::charToSuit(char c) {
    c = std::toupper(c);
    switch (c) {
    case 'D': return Suit::Diamonds;
    case 'C': return Suit::Clubs;
    case 'H': return Suit::Hearts;
    case 'S': return Suit::Spades;
    default:
        throw std::invalid_argument(std::string("Invalid suit character: ") + c);
    }
}

char Card::rankToChar(Rank rank) {
    switch (rank) {
    case Rank::Three: return '3';
    case Rank::Four: return '4';
    case Rank::Five: return '5';
    case Rank::Six: return '6';
    case Rank::Seven: return '7';
    case Rank::Eight: return '8';
    case Rank::Nine: return '9';
    case Rank::Ten: return 'T';  // Use 'T' for single char representation
    case Rank::Jack: return 'J';
    case Rank::Queen: return 'Q';
    case Rank::King: return 'K';
    case Rank::Ace: return 'A';
    case Rank::Two: return '2';
    default:
        throw std::invalid_argument("Invalid rank");
    }
}

char Card::suitToChar(Suit suit) {
    switch (suit) {
    case Suit::Diamonds: return 'D';
    case Suit::Clubs: return 'C';
    case Suit::Hearts: return 'H';
    case Suit::Spades: return 'S';
    default:
        throw std::invalid_argument("Invalid suit");
    }
}

std::string Card::suitToSymbol(Suit suit) {
    // Using ASCII letters for better compatibility
    // These will be colored red/black in the rendering
    switch (suit) {
    case Suit::Diamonds: return "D";
    case Suit::Clubs: return "C";
    case Suit::Hearts: return "H";
    case Suit::Spades: return "S";
    default:
        throw std::invalid_argument("Invalid suit");
    }
}

/**
 * Output stream operator
 */
std::ostream& operator<<(std::ostream& os, const Card& card) {
    os << card.toString();
    return os;
}
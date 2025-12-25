/**
 * Player.cpp
 * Implementation of Player class
 */

#include "Player.h"
#include <sstream>

 /**
  * Constructor
  */
Player::Player(const std::string& name, PlayerType type)
    : name_(name), type_(type), score_(0), hasPassed_(false) {
}

/**
 * Deal cards to this player
 */
void Player::dealCards(const std::vector<Card>& cards) {
    hand_.addCards(cards);
}

/**
 * String representation
 */
std::string Player::toString() const {
    std::ostringstream oss;
    oss << name_ << " (" << (type_ == PlayerType::Human ? "Human" : "AI") << ")";
    oss << " - Cards: " << hand_.size();
    oss << ", Score: " << score_;
    if (hasPassed_) {
        oss << " [PASSED]";
    }
    return oss.str();
}
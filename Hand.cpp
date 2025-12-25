/**
 * Hand.cpp
 * Implementation of Hand class
 */

#include "Hand.h"
#include <algorithm>
#include <sstream>
#include <stdexcept>

 /**
  * Add a card to the hand
  */
void Hand::addCard(const Card& card) {
    cards_.push_back(card);
}

/**
 * Add multiple cards to the hand
 */
void Hand::addCards(const std::vector<Card>& cards) {
    cards_.insert(cards_.end(), cards.begin(), cards.end());
}

/**
 * Remove a card from the hand
 */
bool Hand::removeCard(const Card& card) {
    auto it = std::find(cards_.begin(), cards_.end(), card);
    if (it != cards_.end()) {
        cards_.erase(it);
        return true;
    }
    return false;
}

/**
 * Remove multiple cards from the hand
 */
bool Hand::removeCards(const std::vector<Card>& cards) {
    // First check if all cards exist
    for (const auto& card : cards) {
        if (!hasCard(card)) {
            return false;
        }
    }

    // If all exist, remove them
    for (const auto& card : cards) {
        removeCard(card);
    }

    return true;
}

/**
 * Check if hand contains a specific card
 */
bool Hand::hasCard(const Card& card) const {
    return std::find(cards_.begin(), cards_.end(), card) != cards_.end();
}

/**
 * Check if hand contains all specified cards
 */
bool Hand::hasCards(const std::vector<Card>& cards) const {
    for (const auto& card : cards) {
        if (!hasCard(card)) {
            return false;
        }
    }
    return true;
}

/**
 * Sort the hand
 */
void Hand::sort(SortOrder order) {
    if (order == SortOrder::ByRank) {
        std::sort(cards_.begin(), cards_.end(), compareByRank);
    }
    else {
        std::sort(cards_.begin(), cards_.end(), compareBySuit);
    }
}

/**
 * Find cards by string representation
 */
std::vector<Card> Hand::findCards(const std::vector<std::string>& cardStrings) const {
    std::vector<Card> found;
    found.reserve(cardStrings.size());

    for (const auto& cardStr : cardStrings) {
        try {
            Card searchCard(cardStr);
            if (hasCard(searchCard)) {
                found.push_back(searchCard);
            }
        }
        catch (const std::exception&) {
            // Invalid card string, skip it
            continue;
        }
    }

    return found;
}

/**
 * Get card at specific index
 */
const Card& Hand::at(size_t index) const {
    if (index >= cards_.size()) {
        throw std::out_of_range("Hand index out of range");
    }
    return cards_[index];
}

/**
 * Clear all cards from hand
 */
void Hand::clear() {
    cards_.clear();
}

/**
 * Check if this hand contains the 3 of Diamonds
 */
bool Hand::hasThreeOfDiamonds() const {
    Card threeOfDiamonds(Rank::Three, Suit::Diamonds);
    return hasCard(threeOfDiamonds);
}

/**
 * String representation of the hand
 */
std::string Hand::toString() const {
    if (isEmpty()) {
        return "Empty hand";
    }

    std::ostringstream oss;
    for (size_t i = 0; i < cards_.size(); ++i) {
        if (i > 0) {
            oss << " ";
        }
        oss << cards_[i].toString();
    }
    return oss.str();
}

/**
 * Display string with unicode symbols
 */
std::string Hand::toDisplayString() const {
    if (isEmpty()) {
        return "Empty hand";
    }

    std::ostringstream oss;
    for (size_t i = 0; i < cards_.size(); ++i) {
        if (i > 0) {
            oss << " ";
        }
        oss << cards_[i].toDisplayString();
    }
    return oss.str();
}

/**
 * Comparator for sorting by rank (then by suit)
 */
bool Hand::compareByRank(const Card& a, const Card& b) {
    if (a.getRank() != b.getRank()) {
        return a.getRank() < b.getRank();
    }
    return a.getSuit() < b.getSuit();
}

/**
 * Comparator for sorting by suit (then by rank)
 */
bool Hand::compareBySuit(const Card& a, const Card& b) {
    if (a.getSuit() != b.getSuit()) {
        return a.getSuit() < b.getSuit();
    }
    return a.getRank() < b.getRank();
}
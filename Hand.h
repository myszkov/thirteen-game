/**
 * Hand.hpp
 * Manages a player's hand of cards with sorting and selection capabilities
 */

#ifndef HAND_H
#define HAND_H

#include "Card.h"
#include <vector>
#include <string>
#include <optional>

enum class SortOrder {
    ByRank,     // Sort by rank first, then suit
    BySuit      // Sort by suit first, then rank
};

class Hand {
public:
    /**
     * Constructor
     */
    Hand() = default;

    /**
     * Add a card to the hand
     */
    void addCard(const Card& card);

    /**
     * Add multiple cards to the hand
     */
    void addCards(const std::vector<Card>& cards);

    /**
     * Remove a card from the hand
     * @return true if card was found and removed, false otherwise
     */
    bool removeCard(const Card& card);

    /**
     * Remove multiple cards from the hand
     * @return true if all cards were found and removed, false otherwise
     */
    bool removeCards(const std::vector<Card>& cards);

    /**
     * Check if hand contains a specific card
     */
    bool hasCard(const Card& card) const;

    /**
     * Check if hand contains all specified cards
     */
    bool hasCards(const std::vector<Card>& cards) const;

    /**
     * Get all cards in the hand
     */
    const std::vector<Card>& getCards() const { return cards_; }

    /**
     * Get number of cards in hand
     */
    size_t size() const { return cards_.size(); }

    /**
     * Check if hand is empty
     */
    bool isEmpty() const { return cards_.empty(); }

    /**
     * Sort the hand
     */
    void sort(SortOrder order = SortOrder::ByRank);

    /**
     * Find cards by string representation (e.g., "3H", "AS")
     * @return Vector of found cards, empty if not found
     */
    std::vector<Card> findCards(const std::vector<std::string>& cardStrings) const;

    /**
     * Get card at specific index
     * @throws std::out_of_range if index is invalid
     */
    const Card& at(size_t index) const;

    /**
     * Clear all cards from hand
     */
    void clear();

    /**
     * Check if this hand contains the 3 of Diamonds
     * (Used to determine starting player in Big Two)
     */
    bool hasThreeOfDiamonds() const;

    /**
     * String representation of the hand
     */
    std::string toString() const;
    std::string toDisplayString() const;  // With unicode suit symbols

private:
    std::vector<Card> cards_;

    /**
     * Comparator for sorting by rank
     */
    static bool compareByRank(const Card& a, const Card& b);

    /**
     * Comparator for sorting by suit
     */
    static bool compareBySuit(const Card& a, const Card& b);
};

#endif // HAND_HPP
/**
 * Deck.hpp
 * Manages a standard 52-card deck with shuffling and dealing capabilities
 */

#ifndef DECK_H
#define DECK_H

#include "Card.h"
#include <vector>
#include <random>

class Deck {
public:
    /**
     * Constructor - creates a standard 52-card deck
     */
    Deck();

    /**
     * Shuffle the deck using random number generator
     */
    void shuffle();

    /**
     * Deal a single card from the top of the deck
     * @return The dealt card
     * @throws std::runtime_error if deck is empty
     */
    Card deal();

    /**
     * Deal multiple cards from the deck
     * @param count Number of cards to deal
     * @return Vector of dealt cards
     * @throws std::runtime_error if not enough cards
     */
    std::vector<Card> dealMultiple(size_t count);

    /**
     * Reset the deck to full 52 cards (unshuffled)
     */
    void reset();

    /**
     * Get number of cards remaining in deck
     */
    size_t size() const { return cards_.size(); }

    /**
     * Check if deck is empty
     */
    bool isEmpty() const { return cards_.empty(); }

    /**
     * Get all remaining cards (for debugging/testing)
     */
    const std::vector<Card>& getCards() const { return cards_; }

private:
    std::vector<Card> cards_;
    std::mt19937 rng_;  // Random number generator

    /**
     * Initialize deck with all 52 cards
     */
    void initializeDeck();
};

#endif // DECK_HPP

/**
 * Deck.cpp
 * Implementation of Deck class
 */

#include "Deck.h"
#include <algorithm>
#include <stdexcept>
#include <chrono>

 /**
  * Constructor - creates and initializes a standard 52-card deck
  */
Deck::Deck() {
    // Seed the random number generator with current time
    auto seed = std::chrono::steady_clock::now().time_since_epoch().count();
    rng_.seed(static_cast<std::mt19937::result_type>(seed));

    initializeDeck();
}

/**
 * Initialize deck with all 52 cards (13 ranks × 4 suits)
 */
void Deck::initializeDeck() {
    cards_.clear();
    cards_.reserve(52);

    // Create all combinations of ranks and suits
    const std::vector<Rank> ranks = {
        Rank::Three, Rank::Four, Rank::Five, Rank::Six,
        Rank::Seven, Rank::Eight, Rank::Nine, Rank::Ten,
        Rank::Jack, Rank::Queen, Rank::King, Rank::Ace, Rank::Two
    };

    const std::vector<Suit> suits = {
        Suit::Diamonds, Suit::Clubs, Suit::Hearts, Suit::Spades
    };

    for (Rank rank : ranks) {
        for (Suit suit : suits) {
            cards_.emplace_back(rank, suit);
        }
    }
}

/**
 * Shuffle the deck using Fisher-Yates algorithm
 */
void Deck::shuffle() {
    std::shuffle(cards_.begin(), cards_.end(), rng_);
}

/**
 * Deal a single card from the top of the deck
 */
Card Deck::deal() {
    if (isEmpty()) {
        throw std::runtime_error("Cannot deal from empty deck");
    }

    Card card = cards_.back();
    cards_.pop_back();
    return card;
}

/**
 * Deal multiple cards from the deck
 */
std::vector<Card> Deck::dealMultiple(size_t count) {
    if (count > size()) {
        throw std::runtime_error(
            "Not enough cards in deck. Requested: " + std::to_string(count) +
            ", Available: " + std::to_string(size())
        );
    }

    std::vector<Card> dealt;
    dealt.reserve(count);

    for (size_t i = 0; i < count; ++i) {
        dealt.push_back(deal());
    }

    return dealt;
}

/**
 * Reset the deck to full 52 cards (unshuffled)
 */
void Deck::reset() {
    initializeDeck();
}
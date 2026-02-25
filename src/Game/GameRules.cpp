/**
 * GameRules.cpp
 * Implementation of Big Two game rules
 */

#include "Game/GameRules.h"
#include <algorithm>
#include <set>

 /**
  * Validate if a play is legal (Thirteen version)
  */
PlayValidation GameRules::validatePlay(
    const std::vector<Card>& cards,
    const std::vector<Card>& lastPlay,
    bool isFirstPlay,
    bool mustIncludeThreeOfSpades
) {
    PlayValidation result;

    // Empty play is invalid
    if (cards.empty()) {
        result.errorMessage = "No cards selected";
        return result;
    }

    // Check if 3 of Spades is required and present (Thirteen rule)
    if (mustIncludeThreeOfSpades && !containsThreeOfSpades(cards)) {
        result.errorMessage = "First play must include 3 of Spades";
        return result;
    }

    // Determine play type
    result.playType = determinePlayType(cards);

    if (result.playType == PlayType::Invalid) {
        result.errorMessage = "Invalid card combination";
        return result;
    }

    // If this is the first play or last play was cleared, any valid play is allowed
    if (isFirstPlay || lastPlay.empty()) {
        result.isValid = true;
        return result;
    }

    // Must match the number of cards in last play (unless playing four of a kind)
    if (result.playType != PlayType::FourOfAKind && cards.size() != lastPlay.size()) {
        result.errorMessage = "Must play same number of cards as last play";
        return result;
    }

    // Check if this play beats the last play
    if (!doesPlayBeat(cards, lastPlay)) {
        result.errorMessage = "Play does not beat the previous play";
        return result;
    }

    result.isValid = true;
    return result;
}

/**
 * Check if a play beats the previous play (Thirteen version)
 */
bool GameRules::doesPlayBeat(
    const std::vector<Card>& newPlay,
    const std::vector<Card>& lastPlay
) {
    if (lastPlay.empty()) {
        return true;  // Any play beats an empty play
    }

    PlayType newType = determinePlayType(newPlay);
    PlayType lastType = determinePlayType(lastPlay);

    // Four of a Kind is special in Thirteen - it can beat anything!
    if (newType == PlayType::FourOfAKind) {
        if (lastType == PlayType::FourOfAKind) {
            // Four of a kind vs four of a kind - compare ranks
            return getHighestCard(newPlay) > getHighestCard(lastPlay);
        }
        return true;  // Four of a kind beats everything else
    }

    // Can't beat four of a kind with anything else
    if (lastType == PlayType::FourOfAKind && newType != PlayType::FourOfAKind) {
        return false;
    }

    // Must match play type and number of cards
    if (newType != lastType || newPlay.size() != lastPlay.size()) {
        return false;
    }

    switch (newType) {
    case PlayType::Single:
        return singleBeats(newPlay[0], lastPlay[0]);

    case PlayType::Pair:
        return pairBeats(newPlay, lastPlay);

    case PlayType::Triple:
        return tripleBeats(newPlay, lastPlay);
    
    case PlayType::FourOfAKind:
        return fourOfAKindBeats(newPlay, lastPlay);

    case PlayType::Sequence:
    case PlayType::DoubleSequence:
    case PlayType::TripleSequence:
        // Sequences: compare highest card
        return getHighestCard(newPlay) > getHighestCard(lastPlay);

    default:
        return false;
    }
}

/**
 * Determine play type (Thirteen version)
 */
PlayType GameRules::determinePlayType(const std::vector<Card>& cards) {
    size_t size = cards.size();

    if (size == 0) {
        return PlayType::Invalid;
    }

    if (size == 1) {
        return PlayType::Single;
    }

    if (size == 2 && isPair(cards)) {
        return PlayType::Pair;
    }

    if (size == 3 && isTriple(cards)) {
        return PlayType::Triple;
    }

    if (size == 4 && isFourOfAKind(cards)) {
        return PlayType::FourOfAKind;  // Standalone in Thirteen!
    }

    // Check for sequences (3+ cards)
    if (size >= 3 && isSequence(cards)) {
        return PlayType::Sequence;
    }

    // Check for double sequence (3+ pairs)
    if (size >= 6 && size % 2 == 0 && isDoubleSequence(cards)) {
        return PlayType::DoubleSequence;
    }

    // Check for triple sequence (3+ triples)
    if (size >= 9 && size % 3 == 0 && isTripleSequence(cards)) {
        return PlayType::TripleSequence;
    }


    return PlayType::Invalid;
}

/**
 * Check if cards form a valid single
 */
bool GameRules::isSingle(const std::vector<Card>& cards) {
    return cards.size() == 1;
}

/**
 * Check if cards form a valid straight (Thirteen: 3+ consecutive cards)
 */
bool GameRules::isStraight(const std::vector<Card>& cards) {
    if (cards.size() < 3) return false;  // Minimum 3 cards

    auto sorted = sortByRank(cards);

    // Check consecutive ranks
    for (size_t i = 1; i < sorted.size(); ++i) {
        int prevRank = static_cast<int>(sorted[i - 1].getRank());
        int currRank = static_cast<int>(sorted[i].getRank());

        // Each card must be exactly 1 rank higher than previous
        if (currRank != prevRank + 1) {
            return false;  // Not consecutive!
        }
    }

    return true;
}

/**
 * Check if cards form a sequence (alias for isStraight in Thirteen)
 */
bool GameRules::isSequence(const std::vector<Card>& cards) {
    return isStraight(cards);
}

/**
 * Check if cards form a valid pair
 */
bool GameRules::isPair(const std::vector<Card>& cards) {
    return cards.size() == 2 && cards[0].getRank() == cards[1].getRank();
}

/**
 * Check if cards form a valid triple
 */
bool GameRules::isTriple(const std::vector<Card>& cards) {
    return cards.size() == 3 &&
        cards[0].getRank() == cards[1].getRank() &&
        cards[1].getRank() == cards[2].getRank();
}

/**
 * Check if cards form four of a kind (Thirteen: exactly 4 cards of same rank)
 */
bool GameRules::isFourOfAKind(const std::vector<Card>& cards) {
    if (cards.size() != 4) return false;  // Thirteen: exactly 4 cards

    // All 4 cards must have the same rank
    Rank rank = cards[0].getRank();
    for (size_t i = 1; i < cards.size(); ++i) {
        if (cards[i].getRank() != rank) {
            return false;
        }
    }

    return true;
}

/**
 * Compare two singles
 */
bool GameRules::singleBeats(const Card& newCard, const Card& lastCard) {
    return newCard > lastCard;
}

/**
 * Compare two pairs
 */
bool GameRules::pairBeats(const std::vector<Card>& newPair, const std::vector<Card>& lastPair) {
    if (newPair.size() != 2 || lastPair.size() != 2) return false;

    Card newHigh = getHighestCard(newPair);
    Card lastHigh = getHighestCard(lastPair);

    return newHigh > lastHigh;
}

/**
 * Compare two triples
 */
bool GameRules::tripleBeats(const std::vector<Card>& newTriple, const std::vector<Card>& lastTriple) {
    if (newTriple.size() != 3 || lastTriple.size() != 3) return false;

    Card newHigh = getHighestCard(newTriple);
    Card lastHigh = getHighestCard(lastTriple);

    return newHigh > lastHigh;
}

bool GameRules::fourOfAKindBeats(const std::vector<Card>& newFourOfAKind, const std::vector<Card>& lastFourOfAKind) {
    if (newFourOfAKind.size() != 4 || lastFourOfAKind.size() != 4) return false;

    Card newHigh = getHighestCard(newFourOfAKind);
    Card lastHigh = getHighestCard(lastFourOfAKind);

    return newHigh > lastHigh;
}

/**
 * Get the highest card in a set
 */
Card GameRules::getHighestCard(const std::vector<Card>& cards) {
    if (cards.empty()) {
        throw std::runtime_error("Cannot get highest card from empty set");
    }

    Card highest = cards[0];
    for (size_t i = 1; i < cards.size(); ++i) {
        if (cards[i] > highest) {
            highest = cards[i];
        }
    }

    return highest;
}

/**
 * Check if play contains 3 of Spades (for Thirteen)
 */
bool GameRules::containsThreeOfSpades(const std::vector<Card>& cards) {
    Card threeOfSpades(Rank::Three, Suit::Spades); 
    return std::find(cards.begin(), cards.end(), threeOfSpades) != cards.end();
}

/**
 * Check if cards form a double sequence (3+ pairs in a row)
 * Example: 3-3-4-4-5-5 (three pairs in sequence)
 */
bool GameRules::isDoubleSequence(const std::vector<Card>& cards) {
    if (cards.size() < 6 || cards.size() % 2 != 0) return false;

    auto sorted = sortByRank(cards);
    size_t numPairs = cards.size() / 2;

    // Check each pair
    for (size_t i = 0; i < cards.size(); i += 2) {
        // Check if this is a pair
        if (sorted[i].getRank() != sorted[i + 1].getRank()) {
            return false;
        }

        // Check if pairs are consecutive (except for last pair)
        if (i + 2 < cards.size()) {
            int currRank = static_cast<int>(sorted[i].getRank());
            int nextRank = static_cast<int>(sorted[i + 2].getRank());
            if (nextRank != currRank + 1) {
                return false;
            }
        }
    }

    return numPairs >= 3;  // At least 3 pairs
}

/**
 * Check if cards form a triple sequence (3+ triples in a row)
 * Example: 3-3-3-4-4-4-5-5-5 (three triples in sequence)
 */
bool GameRules::isTripleSequence(const std::vector<Card>& cards) {
    if (cards.size() < 9 || cards.size() % 3 != 0) return false;

    auto sorted = sortByRank(cards);
    size_t numTriples = cards.size() / 3;

    // Check each triple
    for (size_t i = 0; i < cards.size(); i += 3) {
        // Check if this is a triple
        if (sorted[i].getRank() != sorted[i + 1].getRank() ||
            sorted[i].getRank() != sorted[i + 2].getRank()) {
            return false;
        }

        // Check if triples are consecutive (except for last triple)
        if (i + 3 < cards.size()) {
            int currRank = static_cast<int>(sorted[i].getRank());
            int nextRank = static_cast<int>(sorted[i + 3].getRank());
            if (nextRank != currRank + 1) {
                return false;
            }
        }
    }

    return numTriples >= 3;  // At least 3 triples
}

/**
 * Sort cards by rank
 */
std::vector<Card> GameRules::sortByRank(std::vector<Card> cards) {
    std::sort(cards.begin(), cards.end(), [](const Card& a, const Card& b) {
        return a.getRank() < b.getRank();
        });
    return cards;
}

/**
 * Get play type name (Thirteen version)
 */
std::string GameRules::getPlayTypeName(PlayType type) {
    switch (type) {
    case PlayType::Single:
        return "Single";
    case PlayType::Pair:
        return "Pair";
    case PlayType::Triple:
        return "Triple";
    case PlayType::FourOfAKind:
        return "Four of a Kind";  // Powerful standalone play in Thirteen!
    case PlayType::Sequence:
        return "Sequence";  // 3+ card straight
    case PlayType::DoubleSequence:
        return "Double Sequence";  // Pairs in a row
    case PlayType::TripleSequence:
        return "Triple Sequence";  // Triples in a row
        // Legacy Big Two five-card (kept for reference)
        // case PlayType::FiveCard:
        //     switch (fiveCardType) {
        //         case FiveCardType::Straight:      return "Straight";
        //         case FiveCardType::Flush:         return "Flush";
        //         case FiveCardType::FullHouse:     return "Full House";
        //         case FiveCardType::FourOfAKind:   return "Four of a Kind";
        //         case FiveCardType::StraightFlush: return "Straight Flush";
        //         default:                          return "Five Card";
        //     }
    default:
        return "Invalid";
    }
}
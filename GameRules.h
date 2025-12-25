/**
 * GameRules.hpp
 * Implements Big Two game rules and logic
 */

#ifndef GAMERULES_H
#define GAMERULES_H

#include "Card.h"
#include "GameState.h"
#include <vector>
#include <string>

 /**
  * Play type enumeration
  */
enum class PlayType {
    Invalid,
    Single,         // One card
    Pair,           // Two cards of same rank
    Triple,         // Three cards of same rank
    FiveCard        // Five-card combinations (straight, flush, full house, etc.)
};

/**
 * Five-card combination types
 */
enum class FiveCardType {
    None,
    Straight,       // 5 consecutive ranks
    Flush,          // 5 cards of same suit
    FullHouse,      // Triple + Pair
    FourOfAKind,    // Four cards of same rank + 1
    StraightFlush   // Straight + Flush
};

/**
 * Result of play validation
 */
struct PlayValidation {
    bool isValid;
    PlayType playType;
    FiveCardType fiveCardType;
    std::string errorMessage;

    PlayValidation()
        : isValid(false),
        playType(PlayType::Invalid),
        fiveCardType(FiveCardType::None),
        errorMessage("") {
    }
};

/**
 * GameRules class
 */
class GameRules {
public:
    /**
     * Validate if a play is legal
     */
    static PlayValidation validatePlay(
        const std::vector<Card>& cards,
        const std::vector<Card>& lastPlay,
        bool isFirstPlay,
        bool mustIncludeThreeOfDiamonds
    );

    /**
     * Check if a play beats the previous play
     */
    static bool doesPlayBeat(
        const std::vector<Card>& newPlay,
        const std::vector<Card>& lastPlay
    );

    /**
     * Determine play type
     */
    static PlayType determinePlayType(const std::vector<Card>& cards);

    /**
     * Determine five-card combination type
     */
    static FiveCardType determineFiveCardType(const std::vector<Card>& cards);

    /**
     * Check if cards form a valid single
     */
    static bool isSingle(const std::vector<Card>& cards);

    /**
     * Check if cards form a valid pair
     */
    static bool isPair(const std::vector<Card>& cards);

    /**
     * Check if cards form a valid triple
     */
    static bool isTriple(const std::vector<Card>& cards);

    /**
     * Check if cards form a valid straight
     */
    static bool isStraight(const std::vector<Card>& cards);

    /**
     * Check if cards form a valid flush
     */
    static bool isFlush(const std::vector<Card>& cards);

    /**
     * Check if cards form a valid full house
     */
    static bool isFullHouse(const std::vector<Card>& cards);

    /**
     * Check if cards form four of a kind
     */
    static bool isFourOfAKind(const std::vector<Card>& cards);

    /**
     * Check if cards form a straight flush
     */
    static bool isStraightFlush(const std::vector<Card>& cards);

    /**
     * Compare two singles
     */
    static bool singleBeats(const Card& newCard, const Card& lastCard);

    /**
     * Compare two pairs
     */
    static bool pairBeats(const std::vector<Card>& newPair, const std::vector<Card>& lastPair);

    /**
     * Compare two triples
     */
    static bool tripleBeats(const std::vector<Card>& newTriple, const std::vector<Card>& lastTriple);

    /**
     * Compare two five-card combinations
     */
    static bool fiveCardBeats(const std::vector<Card>& newCards, const std::vector<Card>& lastCards);

    /**
     * Get the highest card in a set
     */
    static Card getHighestCard(const std::vector<Card>& cards);

    /**
     * Get rank value for comparison (in a five-card combo)
     */
    static int getFiveCardRank(FiveCardType type);

    /**
     * Check if play must include 3 of Diamonds (first play of game)
     */
    static bool containsThreeOfDiamonds(const std::vector<Card>& cards);

    /**
     * Sort cards by rank
     */
    static std::vector<Card> sortByRank(std::vector<Card> cards);

    /**
     * Get play type name
     */
    static std::string getPlayTypeName(PlayType type, FiveCardType fiveCardType = FiveCardType::None);
};

#endif // GAMERULES_HPP

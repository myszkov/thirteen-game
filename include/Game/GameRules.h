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
  * Play type enumeration (Thirteen version)
  */
enum class PlayType {
    Invalid,
    Single,            // One card
    Pair,              // Two cards of same rank
    Triple,            // Three cards of same rank
    FourOfAKind,       // Four cards of same rank (powerful in Thirteen!)
    Sequence,          // 3+ consecutive cards (straights)
    DoubleSequence,    // 3+ pairs in sequence
    TripleSequence     // 3+ triples in sequence
};

/**
 * Result of play validation
 */
struct PlayValidation {
    bool isValid;
    PlayType playType;
    std::string errorMessage;

    PlayValidation()
        : isValid(false),
        playType(PlayType::Invalid),
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
        bool mustIncludeThreeOfSpades
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
     * Check if cards form four of a kind
     */
    static bool isFourOfAKind(const std::vector<Card>& cards);

    /**
     * Check if cards form a valid straight (Thirteen: 3+ cards)
     */
    static bool isStraight(const std::vector<Card>& cards);

    /**
     * Check if cards form a valid sequence (3+ consecutive cards)
     */
    static bool isSequence(const std::vector<Card>& cards);

    /**
     * Check if cards form a valid double sequence (3+ pairs in a row)
     */
    static bool isDoubleSequence(const std::vector<Card>& cards);

    /**
     * Check if cards form a valid triple sequence (3+ triples in a row)
     */
    static bool isTripleSequence(const std::vector<Card>& cards);

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
     * Compare two four Of a Kind
     */
    static bool fourOfAKindBeats(const std::vector<Card>& newTriple, const std::vector<Card>& lastFourOfAKind);

    /**
     * Get the highest card in a set
     */
    static Card getHighestCard(const std::vector<Card>& cards);

    /**
     * Check if play must include 3 of Diamonds (first play of game)
     */
    static bool containsThreeOfSpades(const std::vector<Card>& cards);

    /**
     * Sort cards by rank
     */
    static std::vector<Card> sortByRank(std::vector<Card> cards);

    /**
     * Get play type name
     */
    static std::string getPlayTypeName(PlayType type);
};

#endif // GAMERULES_H

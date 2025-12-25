/**
 * GameRules.cpp
 * Implementation of Big Two game rules
 */

#include "GameRules.h"
#include <algorithm>
#include <set>

 /**
  * Validate if a play is legal
  */
PlayValidation GameRules::validatePlay(
    const std::vector<Card>& cards,
    const std::vector<Card>& lastPlay,
    bool isFirstPlay,
    bool mustIncludeThreeOfDiamonds
) {
    PlayValidation result;

    // Empty play is invalid
    if (cards.empty()) {
        result.errorMessage = "No cards selected";
        return result;
    }

    // Check if 3 of Diamonds is required and present
    if (mustIncludeThreeOfDiamonds && !containsThreeOfDiamonds(cards)) {
        result.errorMessage = "First play must include 3 of Diamonds";
        return result;
    }

    // Determine play type
    result.playType = determinePlayType(cards);

    if (result.playType == PlayType::Invalid) {
        result.errorMessage = "Invalid card combination";
        return result;
    }

    // If it's a five-card play, determine the specific type
    if (result.playType == PlayType::FiveCard) {
        result.fiveCardType = determineFiveCardType(cards);
        if (result.fiveCardType == FiveCardType::None) {
            result.errorMessage = "Invalid five-card combination";
            return result;
        }
    }

    // If this is the first play or last play was cleared, any valid play is allowed
    if (isFirstPlay || lastPlay.empty()) {
        result.isValid = true;
        return result;
    }

    // Must match the number of cards in last play
    if (cards.size() != lastPlay.size()) {
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
 * Check if a play beats the previous play
 */
bool GameRules::doesPlayBeat(
    const std::vector<Card>& newPlay,
    const std::vector<Card>& lastPlay
) {
    if (lastPlay.empty()) {
        return true;  // Any play beats an empty play
    }

    if (newPlay.size() != lastPlay.size()) {
        return false;  // Must be same size
    }

    PlayType playType = determinePlayType(newPlay);

    switch (playType) {
    case PlayType::Single:
        return singleBeats(newPlay[0], lastPlay[0]);

    case PlayType::Pair:
        return pairBeats(newPlay, lastPlay);

    case PlayType::Triple:
        return tripleBeats(newPlay, lastPlay);

    case PlayType::FiveCard:
        return fiveCardBeats(newPlay, lastPlay);

    default:
        return false;
    }
}

/**
 * Determine play type
 */
PlayType GameRules::determinePlayType(const std::vector<Card>& cards) {
    size_t size = cards.size();

    if (size == 1) {
        return PlayType::Single;
    }
    else if (size == 2 && isPair(cards)) {
        return PlayType::Pair;
    }
    else if (size == 3 && isTriple(cards)) {
        return PlayType::Triple;
    }
    else if (size == 5) {
        FiveCardType fiveType = determineFiveCardType(cards);
        if (fiveType != FiveCardType::None) {
            return PlayType::FiveCard;
        }
    }

    return PlayType::Invalid;
}

/**
 * Determine five-card combination type
 */
FiveCardType GameRules::determineFiveCardType(const std::vector<Card>& cards) {
    if (cards.size() != 5) {
        return FiveCardType::None;
    }

    // Check in order of strength (highest first)
    if (isStraightFlush(cards)) return FiveCardType::StraightFlush;
    if (isFourOfAKind(cards)) return FiveCardType::FourOfAKind;
    if (isFullHouse(cards)) return FiveCardType::FullHouse;
    if (isFlush(cards)) return FiveCardType::Flush;
    if (isStraight(cards)) return FiveCardType::Straight;

    return FiveCardType::None;
}

/**
 * Check if cards form a valid single
 */
bool GameRules::isSingle(const std::vector<Card>& cards) {
    return cards.size() == 1;
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
 * Check if cards form a valid straight
 */
bool GameRules::isStraight(const std::vector<Card>& cards) {
    if (cards.size() != 5) return false;

    auto sorted = sortByRank(cards);

    // Check consecutive ranks
    for (size_t i = 1; i < sorted.size(); ++i) {
        int prevRank = static_cast<int>(sorted[i - 1].getRank());
        int currRank = static_cast<int>(sorted[i].getRank());

        if (currRank != prevRank + 1) {
            return false;
        }
    }

    return true;
}

/**
 * Check if cards form a valid flush
 */
bool GameRules::isFlush(const std::vector<Card>& cards) {
    if (cards.size() != 5) return false;

    Suit suit = cards[0].getSuit();
    for (size_t i = 1; i < cards.size(); ++i) {
        if (cards[i].getSuit() != suit) {
            return false;
        }
    }

    return true;
}

/**
 * Check if cards form a valid full house
 */
bool GameRules::isFullHouse(const std::vector<Card>& cards) {
    if (cards.size() != 5) return false;

    auto sorted = sortByRank(cards);

    // Pattern: AAA BB or AA BBB
    bool pattern1 = (sorted[0].getRank() == sorted[1].getRank() &&
        sorted[1].getRank() == sorted[2].getRank() &&
        sorted[3].getRank() == sorted[4].getRank());

    bool pattern2 = (sorted[0].getRank() == sorted[1].getRank() &&
        sorted[2].getRank() == sorted[3].getRank() &&
        sorted[3].getRank() == sorted[4].getRank());

    return pattern1 || pattern2;
}

/**
 * Check if cards form four of a kind
 */
bool GameRules::isFourOfAKind(const std::vector<Card>& cards) {
    if (cards.size() != 5) return false;

    auto sorted = sortByRank(cards);

    // Pattern: AAAA B or A BBBB
    bool pattern1 = (sorted[0].getRank() == sorted[1].getRank() &&
        sorted[1].getRank() == sorted[2].getRank() &&
        sorted[2].getRank() == sorted[3].getRank());

    bool pattern2 = (sorted[1].getRank() == sorted[2].getRank() &&
        sorted[2].getRank() == sorted[3].getRank() &&
        sorted[3].getRank() == sorted[4].getRank());

    return pattern1 || pattern2;
}

/**
 * Check if cards form a straight flush
 */
bool GameRules::isStraightFlush(const std::vector<Card>& cards) {
    return isStraight(cards) && isFlush(cards);
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

/**
 * Compare two five-card combinations
 */
bool GameRules::fiveCardBeats(const std::vector<Card>& newCards, const std::vector<Card>& lastCards) {
    FiveCardType newType = determineFiveCardType(newCards);
    FiveCardType lastType = determineFiveCardType(lastCards);

    // Compare by combination rank first
    int newRank = getFiveCardRank(newType);
    int lastRank = getFiveCardRank(lastType);

    if (newRank > lastRank) {
        return true;
    }
    else if (newRank < lastRank) {
        return false;
    }

    // Same type - compare by highest card
    Card newHigh = getHighestCard(newCards);
    Card lastHigh = getHighestCard(lastCards);

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
 * Get rank value for five-card combination type
 */
int GameRules::getFiveCardRank(FiveCardType type) {
    switch (type) {
    case FiveCardType::Straight:      return 1;
    case FiveCardType::Flush:         return 2;
    case FiveCardType::FullHouse:     return 3;
    case FiveCardType::FourOfAKind:   return 4;
    case FiveCardType::StraightFlush: return 5;
    default:                          return 0;
    }
}

/**
 * Check if play contains 3 of Diamonds
 */
bool GameRules::containsThreeOfDiamonds(const std::vector<Card>& cards) {
    Card threeOfDiamonds(Rank::Three, Suit::Diamonds);
    return std::find(cards.begin(), cards.end(), threeOfDiamonds) != cards.end();
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
 * Get play type name
 */
std::string GameRules::getPlayTypeName(PlayType type, FiveCardType fiveCardType) {
    switch (type) {
    case PlayType::Single:
        return "Single";
    case PlayType::Pair:
        return "Pair";
    case PlayType::Triple:
        return "Triple";
    case PlayType::FiveCard:
        switch (fiveCardType) {
        case FiveCardType::Straight:      return "Straight";
        case FiveCardType::Flush:         return "Flush";
        case FiveCardType::FullHouse:     return "Full House";
        case FiveCardType::FourOfAKind:   return "Four of a Kind";
        case FiveCardType::StraightFlush: return "Straight Flush";
        default:                          return "Five Card";
        }
    default:
        return "Invalid";
    }
}
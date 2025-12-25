/**
 * GameState.cpp
 * Implementation of GameState class
 */

#include "GameState.h"
#include <sstream>
#include <algorithm>

 /**
  * Constructor
  */
GameState::GameState()
    : currentPlayerIndex_(0),
    lastPlayingPlayerIndex_(0),
    phase_(GamePhase::NotStarted),
    consecutivePasses_(0),
    firstPlayOfGame_(true) {
}

/**
 * Initialize game with players
 */
void GameState::initializePlayers(int numPlayers, int numHumans) {
    players_.clear();

    // Clamp values to reasonable range (2-4 players typical for Big Two)
    numPlayers = std::max(2, std::min(4, numPlayers));
    numHumans = std::max(0, std::min(numHumans, numPlayers));

    // Create human players first
    for (int i = 0; i < numHumans; ++i) {
        players_.emplace_back(generatePlayerName(i, PlayerType::Human), PlayerType::Human);
    }

    // Create AI players
    for (int i = numHumans; i < numPlayers; ++i) {
        players_.emplace_back(generatePlayerName(i, PlayerType::AI), PlayerType::AI);
    }
}

/**
 * Start a new game
 */
void GameState::startNewGame() {
    // Reset deck
    deck_.reset();
    deck_.shuffle();

    // Clear all player hands
    for (auto& player : players_) {
        player.clearHand();
        player.resetPass();
    }

    // Deal cards
    dealCards();

    // Find starting player (player with 3 of Diamonds)
    currentPlayerIndex_ = findStartingPlayer();

    // Reset game state
    lastPlay_.clear();
    lastPlayingPlayerIndex_ = currentPlayerIndex_;
    consecutivePasses_ = 0;
    firstPlayOfGame_ = true;  // Reset the first play flag
    phase_ = GamePhase::InProgress;
}

/**
 * Deal cards to all players
 */
void GameState::dealCards() {
    if (players_.empty()) {
        return;
    }

    size_t numPlayers = players_.size();
    size_t cardsPerPlayer = 52 / numPlayers;

    // Deal cards in round-robin fashion
    for (size_t i = 0; i < cardsPerPlayer * numPlayers; ++i) {
        size_t playerIndex = i % numPlayers;
        Card card = deck_.deal();
        players_[playerIndex].getHand().addCard(card);
    }

    // Sort each player's hand
    for (auto& player : players_) {
        player.getHand().sort(SortOrder::ByRank);
    }
}

/**
 * Get current player
 */
Player* GameState::getCurrentPlayer() {
    if (currentPlayerIndex_ >= players_.size()) {
        return nullptr;
    }
    return &players_[currentPlayerIndex_];
}

const Player* GameState::getCurrentPlayer() const {
    if (currentPlayerIndex_ >= players_.size()) {
        return nullptr;
    }
    return &players_[currentPlayerIndex_];
}

/**
 * Get player by index
 */
Player* GameState::getPlayer(size_t index) {
    if (index >= players_.size()) {
        return nullptr;
    }
    return &players_[index];
}

const Player* GameState::getPlayer(size_t index) const {
    if (index >= players_.size()) {
        return nullptr;
    }
    return &players_[index];
}

/**
 * Get last playing player
 */
const Player* GameState::getLastPlayingPlayer() const {
    if (lastPlayingPlayerIndex_ >= players_.size()) {
        return nullptr;
    }
    return &players_[lastPlayingPlayerIndex_];
}

/**
 * Set last played cards
 */
void GameState::setLastPlay(const std::vector<Card>& cards, size_t playerIndex) {
    lastPlay_ = cards;
    lastPlayingPlayerIndex_ = playerIndex;
    resetPasses();
}

/**
 * Clear last play
 */
void GameState::clearLastPlay() {
    lastPlay_.clear();
    resetAllPasses();
    resetPasses();
}

/**
 * Move to next player's turn
 */
void GameState::nextTurn() {
    currentPlayerIndex_ = (currentPlayerIndex_ + 1) % players_.size();
}

/**
 * Check if all other players have passed
 */
bool GameState::allOthersHavePassed() const {
    if (players_.size() <= 1) {
        return true;
    }

    // Check if all players except the last playing player have passed
    int passCount = 0;
    for (size_t i = 0; i < players_.size(); ++i) {
        if (i != lastPlayingPlayerIndex_ && players_[i].hasPassed()) {
            passCount++;
        }
    }

    return passCount >= static_cast<int>(players_.size()) - 1;
}

/**
 * Reset all player pass flags
 */
void GameState::resetAllPasses() {
    for (auto& player : players_) {
        player.resetPass();
    }
}

/**
 * Find winner
 */
Player* GameState::findWinner() {
    for (auto& player : players_) {
        if (player.hasWon()) {
            return &player;
        }
    }
    return nullptr;
}

/**
 * Find starting player (player with 3 of Diamonds)
 */
size_t GameState::findStartingPlayer() const {
    for (size_t i = 0; i < players_.size(); ++i) {
        if (players_[i].getHand().hasThreeOfDiamonds()) {
            return i;
        }
    }

    // Fallback to first player if 3 of Diamonds not found
    return 0;
}

/**
 * Generate player name
 */
std::string GameState::generatePlayerName(size_t index, PlayerType type) const {
    if (type == PlayerType::Human) {
        if (index == 0) {
            return "You";
        }
        return "Player " + std::to_string(index + 1);
    }
    else {
        const std::vector<std::string> aiNames = {
            "CPU Alice", "CPU Bob", "CPU Charlie", "CPU Dana"
        };
        if (index < aiNames.size()) {
            return aiNames[index];
        }
        return "CPU " + std::to_string(index + 1);
    }
}

/**
 * Get status message
 */
std::string GameState::getStatusMessage() const {
    std::ostringstream oss;

    switch (phase_) {
    case GamePhase::NotStarted:
        oss << "Game not started. Waiting to begin...";
        break;

    case GamePhase::InProgress:
        if (const Player* current = getCurrentPlayer()) {
            oss << current->getName() << "'s turn";

            if (!lastPlay_.empty()) {
                oss << " - Last play: ";
                for (size_t i = 0; i < lastPlay_.size(); ++i) {
                    if (i > 0) oss << " ";
                    oss << lastPlay_[i].toString();
                }

                if (const Player* lastPlayer = getLastPlayingPlayer()) {
                    oss << " by " << lastPlayer->getName();
                }
            }
            else {
                oss << " - No cards played yet";
            }
        }
        break;

    case GamePhase::Finished:
        if (Player* winner = const_cast<GameState*>(this)->findWinner()) {
            oss << winner->getName() << " wins!";
        }
        else {
            oss << "Game finished";
        }
        break;
    }

    return oss.str();
}
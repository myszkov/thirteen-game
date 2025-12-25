/**
 * GameState.hpp
 * Manages the overall game state including players, turns, and game flow
 */

#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "Player.h"
#include "Deck.h"
#include "Card.h"
#include <vector>
#include <memory>
#include <optional>

 /**
  * Game phase
  */
enum class GamePhase {
    NotStarted,     // Game hasn't started yet
    InProgress,     // Game is being played
    Finished        // Game has ended
};

/**
 * GameState class
 */
class GameState {
public:
    /**
     * Constructor
     */
    GameState();

    /**
     * Initialize game with players
     */
    void initializePlayers(int numPlayers, int numHumans = 1);

    /**
     * Start a new game
     */
    void startNewGame();

    /**
     * Deal cards to all players
     */
    void dealCards();

    /**
     * Get current player
     */
    Player* getCurrentPlayer();
    const Player* getCurrentPlayer() const;

    /**
     * Get player by index
     */
    Player* getPlayer(size_t index);
    const Player* getPlayer(size_t index) const;

    /**
     * Get all players
     */
    std::vector<Player>& getPlayers() { return players_; }
    const std::vector<Player>& getPlayers() const { return players_; }

    /**
     * Get number of players
     */
    size_t getNumPlayers() const { return players_.size(); }

    /**
     * Get current player index
     */
    size_t getCurrentPlayerIndex() const { return currentPlayerIndex_; }

    /**
     * Get last played cards
     */
    const std::vector<Card>& getLastPlay() const { return lastPlay_; }

    /**
     * Get last player who played (not passed)
     */
    const Player* getLastPlayingPlayer() const;

    /**
     * Set last played cards
     */
    void setLastPlay(const std::vector<Card>& cards, size_t playerIndex);

    /**
     * Clear last play (when all players pass)
     */
    void clearLastPlay();

    /**
     * Move to next player's turn
     */
    void nextTurn();

    /**
     * Get game phase
     */
    GamePhase getPhase() const { return phase_; }

    /**
     * Set game phase
     */
    void setPhase(GamePhase phase) { phase_ = phase; }

    /**
     * Get number of consecutive passes
     */
    int getConsecutivePasses() const { return consecutivePasses_; }

    /**
     * Increment consecutive passes
     */
    void incrementPasses() { consecutivePasses_++; }

    /**
     * Reset consecutive passes
     */
    void resetPasses() { consecutivePasses_ = 0; }

    /**
     * Check if all other players have passed
     */
    bool allOthersHavePassed() const;

    /**
     * Reset all player pass flags
     */
    void resetAllPasses();

    /**
     * Find winner (player with no cards)
     */
    Player* findWinner();

    /**
     * Find starting player (player with 3 of Diamonds)
     */
    size_t findStartingPlayer() const;

    /**
     * Get deck (for debugging/testing)
     */
    Deck& getDeck() { return deck_; }

    /**
     * Game status message
     */
    std::string getStatusMessage() const;

    /**
     * Check if this is the very first play of the game
     */
    bool isFirstPlayOfGame() const { return firstPlayOfGame_; }

    /**
     * Mark that the first play has been made
     */
    void setFirstPlayMade() { firstPlayOfGame_ = false; }

private:
    std::vector<Player> players_;
    Deck deck_;
    size_t currentPlayerIndex_;
    std::vector<Card> lastPlay_;
    size_t lastPlayingPlayerIndex_;
    GamePhase phase_;
    int consecutivePasses_;

    /**
     * Create default player names
     */
    std::string generatePlayerName(size_t index, PlayerType type) const;

    bool firstPlayOfGame_;  // Track if the very first play has been made
};

#endif // GAMESTATE_HPP

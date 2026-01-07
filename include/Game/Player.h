/**
 * Player.hpp
 * Represents a player in the game (human or AI)
 */

#ifndef PLAYER_H
#define PLAYER_H

#include "Hand.h"
#include <string>
#include <memory>

 /**
  * Player type
  */
enum class PlayerType {
    Human,
    AI
};

/**
 * Player class
 */
class Player {
public:
    /**
     * Constructor
     */
    Player(const std::string& name, PlayerType type = PlayerType::Human);

    /**
     * Getters
     */
    const std::string& getName() const { return name_; }
    PlayerType getType() const { return type_; }
    Hand& getHand() { return hand_; }
    const Hand& getHand() const { return hand_; }
    int getScore() const { return score_; }
    bool hasPassed() const { return hasPassed_; }

    /**
     * Setters
     */
    void setName(const std::string& name) { name_ = name; }
    void setScore(int score) { score_ = score; }
    void setHasPassed(bool passed) { hasPassed_ = passed; }

    /**
     * Game actions
     */
    void addScore(int points) { score_ += points; }
    void resetPass() { hasPassed_ = false; }

    /**
     * Check if player has won (no cards left)
     */
    bool hasWon() const { return hand_.isEmpty(); }

    /**
     * Deal cards to this player
     */
    void dealCards(const std::vector<Card>& cards);

    /**
     * Clear the player's hand
     */
    void clearHand() { hand_.clear(); }

    /**
     * String representation
     */
    std::string toString() const;

private:
    std::string name_;
    PlayerType type_;
    Hand hand_;
    int score_;
    bool hasPassed_;
};

#endif // PLAYER_HPP

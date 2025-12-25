/**
 * Thirteen (Big Two) Card Game
 * Main entry point - manages game loop with CLI input and SFML rendering
 */

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>

 // Include our game components
#include "Card.h"
#include "Deck.h"
#include "Hand.h"
#include "GameState.h"
#include "GameRules.h"
#include "Renderer.h"

class Game {
public:
    Game() :
        window(sf::VideoMode({ 1280, 720 }), "Thirteen - Big Two"),
        renderer(window),
        running(true),
        needsRedraw(true) {

        window.setFramerateLimit(60);

        // Initialize renderer
        if (!renderer.initialize()) {
            std::cerr << "Warning: Could not load fonts. Using fallback." << std::endl;
        }

        // Initialize test game state
        initializeTestGame();

        std::cout << "=== Thirteen (Big Two) ===" << std::endl;
        std::cout << "Commands: play <cards>, pass, sort, quit" << std::endl;
        std::cout << "Example: play 3H 3D (play pair of threes)" << std::endl;
        std::cout << "> " << std::flush;
    }

    void run() {
        // Start input thread
        std::thread inputThread(&Game::inputLoop, this);

        // Main game loop (SFML rendering)
        while (window.isOpen() && running) {
            handleEvents();
            processCommands();

            if (needsRedraw) {
                render();
                needsRedraw = false;
            }

            // Small sleep to prevent busy waiting
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }

        // Cleanup
        running = false;
        if (inputThread.joinable()) {
            inputThread.join();
        }
    }

private:
    sf::RenderWindow window;
    Renderer renderer;
    std::atomic<bool> running;
    std::atomic<bool> needsRedraw;

    // Thread-safe command queue
    std::queue<std::string> commandQueue;
    std::mutex queueMutex;

    // Game state
    GameState gameState;
    std::string gameStatus = "Welcome! Starting a new game...";

    /**
     * Initialize game with proper game state
     */
    void initializeTestGame() {
        // Initialize with 4 players (1 human, 3 AI)
        gameState.initializePlayers(4, 1);

        // Start the game
        gameState.startNewGame();

        gameStatus = "Game started! " + gameState.getStatusMessage();

        // Print game info to console
        std::cout << "\n=== Game Started ===" << std::endl;
        std::cout << "Players:" << std::endl;
        for (size_t i = 0; i < gameState.getNumPlayers(); ++i) {
            const Player* player = gameState.getPlayer(i);
            if (player) {
                std::cout << "  " << player->toString() << std::endl;
            }
        }
        std::cout << "\n" << gameStatus << std::endl;
        std::cout << "=====================\n" << std::endl;
    }

    /**
     * Input loop - runs in separate thread to read from stdin
     * This allows SFML window to remain responsive while waiting for input
     */
    void inputLoop() {
        std::string line;

        while (running) {
            if (std::getline(std::cin, line)) {
                if (!line.empty()) {
                    // Add command to queue
                    {
                        std::lock_guard<std::mutex> lock(queueMutex);
                        commandQueue.push(line);
                    }
                    needsRedraw = true;
                }

                // Show prompt again
                if (running) {
                    std::cout << "> " << std::flush;
                }
            }
            else {
                // EOF or error
                running = false;
                break;
            }
        }
    }

    /**
     * Handle SFML window events
     */
    void handleEvents() {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                running = false;
            }

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Escape) {
                    window.close();
                    running = false;
                }
            }
        }
    }

    /**
     * Process queued commands from input thread
     */
    void processCommands() {
        std::lock_guard<std::mutex> lock(queueMutex);

        while (!commandQueue.empty()) {
            std::string command = commandQueue.front();
            commandQueue.pop();

            handleCommand(command);
            needsRedraw = true;
        }
    }

    /**
     * Handle a single command
     * This is where we'll integrate CommandParser later
     */
    void handleCommand(const std::string& command) {
        std::cout << "Processing: " << command << std::endl;

        // Parse command (simplified for now)
        if (command == "quit" || command == "exit") {
            std::cout << "Thanks for playing!" << std::endl;
            window.close();
            running = false;
        }
        else if (command == "pass") {
            Player* currentPlayer = gameState.getCurrentPlayer();
            if (currentPlayer && currentPlayer->getType() == PlayerType::Human) {
                currentPlayer->setHasPassed(true);
                gameState.incrementPasses();
                gameStatus = "You passed.";
                std::cout << gameStatus << std::endl;

                // Move to next turn
                gameState.nextTurn();

                // Check if all others have passed AFTER moving to next player
                if (gameState.allOthersHavePassed()) {
                    const Player* roundWinner = gameState.getLastPlayingPlayer();
                    if (roundWinner) {
                        std::cout << "All others passed. " << roundWinner->getName() << " wins the round!" << std::endl;
                    }
                    gameState.clearLastPlay();
                }

                playAITurns();

                gameStatus = gameState.getStatusMessage();
            }
            else {
                std::cout << "It's not your turn!" << std::endl;
            }
        }
        else if (command.starts_with("play ")) {
            std::string cardsStr = command.substr(5);
            handlePlayCommand(cardsStr);
        }
        else if (command == "sort" || command == "sort rank") {
            Player* currentPlayer = gameState.getPlayer(0);  // Human is always player 0
            if (currentPlayer) {
                currentPlayer->getHand().sort(SortOrder::ByRank);
                gameStatus = "Hand sorted by rank.";
                std::cout << gameStatus << std::endl;
            }
        }
        else if (command == "sort suit") {
            Player* currentPlayer = gameState.getPlayer(0);  // Human is always player 0
            if (currentPlayer) {
                currentPlayer->getHand().sort(SortOrder::BySuit);
                gameStatus = "Hand sorted by suit.";
                std::cout << gameStatus << std::endl;
            }
        }
        else if (command == "help") {
            printHelp();
        }
        else {
            std::cout << "Unknown command. Type 'help' for commands." << std::endl;
        }
    }

    /**
     * Handle play command
     */
    void handlePlayCommand(const std::string& cardsStr) {
        Player* currentPlayer = gameState.getCurrentPlayer();
        if (!currentPlayer) {
            std::cout << "Error: No current player!" << std::endl;
            return;
        }

        // Only allow human player to play
        if (currentPlayer->getType() != PlayerType::Human) {
            std::cout << "It's not your turn! Current player: " << currentPlayer->getName() << std::endl;
            gameStatus = "Not your turn!";
            return;
        }

        // Split card strings by spaces
        std::vector<std::string> cardStrs;
        std::istringstream iss(cardsStr);
        std::string cardStr;
        while (iss >> cardStr) {
            cardStrs.push_back(cardStr);
        }

        // Find cards in hand
        auto cards = currentPlayer->getHand().findCards(cardStrs);

        if (cards.empty()) {
            std::cout << "No valid cards found in hand." << std::endl;
            gameStatus = "Invalid cards specified.";
            return;
        }

        if (cards.size() != cardStrs.size()) {
            std::cout << "Some cards not found in hand." << std::endl;
            gameStatus = "Some cards not in your hand.";
            return;
        }

        // Validate the play using GameRules
        bool isFirstPlay = gameState.getLastPlay().empty();
        bool mustInclude3D = gameState.isFirstPlayOfGame();

        PlayValidation validation = GameRules::validatePlay(
            cards,
            gameState.getLastPlay(),
            isFirstPlay,
            mustInclude3D
        );

        if (!validation.isValid) {
            std::cout << "Invalid play: " << validation.errorMessage << std::endl;
            gameStatus = "Invalid: " + validation.errorMessage;
            return;
        }

        // Remove cards from hand and update game state
        if (currentPlayer->getHand().removeCards(cards)) {
            gameState.setLastPlay(cards, gameState.getCurrentPlayerIndex());

            // Mark that the first play of the game has been made
            if (gameState.isFirstPlayOfGame()) {
                gameState.setFirstPlayMade();
            }

            std::string playName = GameRules::getPlayTypeName(validation.playType, validation.fiveCardType);
            gameStatus = currentPlayer->getName() + " played " + playName + ": " + cardsStr;
            std::cout << gameStatus << std::endl;

            // Check for winner
            if (currentPlayer->hasWon()) {
                gameStatus = currentPlayer->getName() + " wins!";
                gameState.setPhase(GamePhase::Finished);
                std::cout << "\n🎉 " << gameStatus << " 🎉\n" << std::endl;
                return;
            }

            // Move to next turn
            gameState.nextTurn();

            // Auto-play AI turns
            playAITurns();

            gameStatus = gameState.getStatusMessage();
            std::cout << "Cards remaining: " << currentPlayer->getHand().size() << std::endl;
        }
        else {
            std::cout << "Error removing cards from hand." << std::endl;
            gameStatus = "Error playing cards.";
        }
    }

    /**
     * Handle AI turns automatically
     */
    void playAITurns() {
        while (gameState.getPhase() == GamePhase::InProgress) {
            Player* currentPlayer = gameState.getCurrentPlayer();
            if (!currentPlayer || currentPlayer->getType() == PlayerType::Human) {
                break;  // Stop when it's human's turn
            }

            // Simple AI: just pass for now
            std::cout << currentPlayer->getName() << " passes." << std::endl;
            currentPlayer->setHasPassed(true);
            gameState.incrementPasses();

            // Move to next turn BEFORE checking round win
            gameState.nextTurn();

            // Check if all others have passed (check AFTER moving to next player)
            if (gameState.allOthersHavePassed()) {
                const Player* roundWinner = gameState.getLastPlayingPlayer();
                if (roundWinner) {
                    std::cout << "All others passed. " << roundWinner->getName() << " wins the round!" << std::endl;
                }
                gameState.clearLastPlay();
            }

            // Small delay for readability
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }

    /**
     * Render game state to SFML window
     */
    void render() {
        renderer.clear();

        // Draw status panel at top
        float statusX = 50;
        float statusY = 20;
        float statusWidth = renderer.getWindowWidth() - 100;
        float statusHeight = 50;
        renderer.drawStatusPanel(gameStatus, statusX, statusY, statusWidth, statusHeight);

        // Draw player info for all players
        float playerInfoY = 100;
        for (size_t i = 0; i < gameState.getNumPlayers(); ++i) {
            const Player* player = gameState.getPlayer(i);
            if (player) {
                bool isActive = (i == gameState.getCurrentPlayerIndex());
                float x = 50 + (i % 2) * 600;  // Two columns
                float y = playerInfoY + (i / 2) * 70;
                renderer.drawPlayerInfo(player->getName(), player->getHand().size(), x, y, isActive);
            }
        }

        // Draw center play area (last played cards)
        renderer.drawPlayArea(gameState.getLastPlay(), renderer.getCenterX(), renderer.getCenterY());

        // Draw human player's hand at bottom (player 0 is always human)
        const Player* humanPlayer = gameState.getPlayer(0);
        if (humanPlayer) {
            float handY = renderer.getWindowHeight() - CardSprite::CARD_HEIGHT - 20;
            renderer.drawHand(humanPlayer->getHand(), 50, handY, true);

            // Draw label
            renderer.drawText("Your Hand:", 50, handY - 25, 18, sf::Color::White);
        }

        // Draw helpful text at bottom
        renderer.drawText("Type commands in the terminal",
            renderer.getCenterX(), renderer.getBottomY() - 10,
            16, sf::Color(200, 200, 200), TextAlign::Center);

        renderer.present();
    }

    /**
     * Print help text
     */
    void printHelp() {
        std::cout << "\n=== Commands ===" << std::endl;
        std::cout << "  play <cards>  - Play cards (e.g., 'play 3H 4H' for pair)" << std::endl;
        std::cout << "  pass          - Pass your turn" << std::endl;
        std::cout << "  sort [rank|suit] - Sort your hand" << std::endl;
        std::cout << "  help          - Show this help" << std::endl;
        std::cout << "  quit          - Exit game" << std::endl;
        std::cout << std::endl;
    }
};

int main() {
    try {
        Game game;
        game.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
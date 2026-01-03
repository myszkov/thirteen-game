/**
 * Thirteen (Tiến Lên) Card Game
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

        std::cout << "=== Thirteen (Tiến Lên) ===" << std::endl;
        std::cout << "Commands: play <cards>, pass, sort, quit" << std::endl;
        std::cout << "Example: play 3S (start with 3♠), play 3H 4D 5S (sequence)" << std::endl;
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
    bool showingTransition = false;  // Flag for player transition screen

    /**
     * Initialize game with 4 human players (Pass & Play mode)
     */
    void initializeTestGame() {
        // Initialize with 4 players (all human for Pass & Play!)
        gameState.initializePlayers(4, 4);  // 4 players, 4 humans

        // Start the game
        gameState.startNewGame();

        gameStatus = "Game started! " + gameState.getStatusMessage();

        // Print game info to console
        std::cout << "\n=== Game Started (Pass & Play Mode) ===" << std::endl;
        std::cout << "Players:" << std::endl;
        for (size_t i = 0; i < gameState.getNumPlayers(); ++i) {
            const Player* player = gameState.getPlayer(i);
            if (player) {
                std::cout << "  " << player->toString() << std::endl;
            }
        }
        std::cout << "\n" << gameStatus << std::endl;
        std::cout << "=====================\n" << std::endl;

        // Show transition for first player
        showingTransition = true;
        needsRedraw = true;
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

                // SPACE key to continue from transition screen
                if (keyPressed->code == sf::Keyboard::Key::Space && showingTransition) {
                    handleTransitionContinue();
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
            if (currentPlayer) {
                currentPlayer->setHasPassed(true);
                gameState.incrementPasses();
                gameStatus = currentPlayer->getName() + " passed.";
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

                // Show transition screen for next player
                showingTransition = true;

                gameStatus = gameState.getStatusMessage();
                std::cout << "\n=== Press SPACE to continue to next player ===" << std::endl;
            }
        }
        else if (command.starts_with("play ")) {
            std::string cardsStr = command.substr(5);
            handlePlayCommand(cardsStr);
        }
        else if (command == "sort" || command == "sort rank") {
            Player* currentPlayer = gameState.getCurrentPlayer();
            if (currentPlayer) {
                currentPlayer->getHand().sort(SortOrder::ByRank);
                gameStatus = "Hand sorted by rank.";
                std::cout << gameStatus << std::endl;
            }
        }
        else if (command == "sort suit") {
            Player* currentPlayer = gameState.getCurrentPlayer();
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

        // Check if it's the correct player's turn (all players are human in Pass & Play)
        // No need to check player type - just verify cards are in current player's hand

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

            std::string playName = GameRules::getPlayTypeName(validation.playType);
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

            // Show transition screen for next player
            showingTransition = true;

            gameStatus = gameState.getStatusMessage();
            std::cout << "Cards remaining: " << currentPlayer->getHand().size() << std::endl;
            std::cout << "\n=== Press SPACE to continue to next player ===" << std::endl;
        }
        else {
            std::cout << "Error removing cards from hand." << std::endl;
            gameStatus = "Error playing cards.";
        }
    }

    /**
     * Handle SPACE key press to show next player's cards
     */
    void handleTransitionContinue() {
        showingTransition = false;
        needsRedraw = true;

        Player* currentPlayer = gameState.getCurrentPlayer();
        if (currentPlayer) {
            std::cout << "\n=== " << currentPlayer->getName() << "'s Turn ===" << std::endl;
            std::cout << "Type 'help' for commands" << std::endl;
            std::cout << "> " << std::flush;
        }
    }

    /**
     * Render game state to SFML window
     */
    void render() {
        renderer.clear();

        // If showing transition screen, hide cards
        if (showingTransition) {
            drawTransitionScreen();
        }
        else {
            drawGameScreen();
        }

        renderer.present();
    }

    /**
     * Draw transition screen (hides cards between players)
     */
    void drawTransitionScreen() {
        Player* nextPlayer = gameState.getCurrentPlayer();
        if (!nextPlayer) return;

        // Draw large centered message
        std::string message = nextPlayer->getName() + "'s Turn";
        renderer.drawText(message, renderer.getCenterX(), renderer.getCenterY() - 100,
            48, sf::Color::White, TextAlign::Center);

        // Draw instruction
        renderer.drawText("Press SPACE to show your cards",
            renderer.getCenterX(), renderer.getCenterY() + 50,
            24, sf::Color(200, 200, 200), TextAlign::Center);

        // Draw card count
        std::string cardInfo = "Cards in hand: " + std::to_string(nextPlayer->getHand().size());
        renderer.drawText(cardInfo,
            renderer.getCenterX(), renderer.getCenterY(),
            20, sf::Color::White, TextAlign::Center);
    }

    /**
     * Draw main game screen with cards
     */
    void drawGameScreen() {
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

        // Draw CURRENT player's hand at bottom
        const Player* currentPlayer = gameState.getCurrentPlayer();
        if (currentPlayer) {
            float handY = renderer.getWindowHeight() - CardSprite::CARD_HEIGHT - 20;
            renderer.drawHand(currentPlayer->getHand(), 50, handY, true);

            // Draw label showing whose hand this is
            std::string handLabel = currentPlayer->getName() + "'s Hand:";
            renderer.drawText(handLabel, 50, handY - 25, 18, sf::Color::Yellow);
        }

        // Draw helpful text at bottom
        renderer.drawText("Type commands in the terminal",
            renderer.getCenterX(), renderer.getBottomY() - 10,
            16, sf::Color(200, 200, 200), TextAlign::Center);
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
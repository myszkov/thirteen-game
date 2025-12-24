/**
 * Thirteen (Big Two) Card Game
 * Main entry point - manages game loop with CLI input and SFML rendering
 */

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>

 // Forward declarations - we'll implement these headers later
 // #include "Game/GameState.hpp"
 // #include "Input/CommandParser.hpp"
 // #include "Rendering/Renderer.hpp"

class Game {
public:
    Game() :
        window(sf::VideoMode({ 1280, 720 }), "Thirteen - Big Two"),
        running(true),
        needsRedraw(true) {

        window.setFramerateLimit(60);

        // Initialize game state here
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
    std::atomic<bool> running;
    std::atomic<bool> needsRedraw;

    // Thread-safe command queue
    std::queue<std::string> commandQueue;
    std::mutex queueMutex;

    // Game state (placeholder)
    std::string gameStatus = "Game started. Waiting for first play...";

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
            gameStatus = "Player passed.";
            std::cout << gameStatus << std::endl;
        }
        else if (command.starts_with("play ")) {
            std::string cards = command.substr(5);
            gameStatus = "Played: " + cards;
            std::cout << gameStatus << std::endl;
        }
        else if (command == "sort" || command.starts_with("sort ")) {
            gameStatus = "Hand sorted.";
            std::cout << gameStatus << std::endl;
        }
        else if (command == "help") {
            printHelp();
        }
        else {
            std::cout << "Unknown command. Type 'help' for commands." << std::endl;
        }
    }

    /**
     * Render game state to SFML window
     */
    void render() {
        window.clear(sf::Color(20, 120, 20)); // Green felt table

        // For now, just draw placeholder text
        // Later we'll integrate the Renderer class

        // TODO: Draw game state, cards, players, etc.

        window.display();
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



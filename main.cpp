/**
 * Thirteen (Tiến Lên) Card Game
 * Main entry point - manages game loop with SFML rendering and mouse input
 */

#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <vector>

#include "Core/Card.h"
#include "Core/Deck.h"
#include "Core/Hand.h"
#include "Game/GameState.h"
#include "Game/GameRules.h"
#include "Rendering/Renderer.h"

class Game {
public:
    // Button layout — positioned above the hand (hand base = 580, selected cards = 560)
    static constexpr float BTN_X      = 1040.0f;
    static constexpr float BTN_WIDTH  = 200.0f;
    static constexpr float BTN_HEIGHT = 45.0f;
    static constexpr float PLAY_BTN_Y = 450.0f;
    static constexpr float PASS_BTN_Y = 505.0f;

    static constexpr float HELP_BTN_X = 1210.0f;
    static constexpr float HELP_BTN_Y = 15.0f;
    static constexpr float HELP_BTN_W = 50.0f;
    static constexpr float HELP_BTN_H = 45.0f;

    // How many pixels selected cards rise
    static constexpr float CARD_SELECT_OFFSET = 20.0f;

    Game() :
        window(sf::VideoMode({ 1280, 720 }), "Thirteen - Big Two"),
        renderer(window),
        running(true),
        needsRedraw(true),
        playButtonHovered_(false),
        passButtonHovered_(false) {

        window.setFramerateLimit(60);

        if (!renderer.initialize()) {
            std::cerr << "Warning: Could not load fonts. Using fallback." << std::endl;
        }

        initializeTestGame();

        std::cout << "=== Thirteen (Tiến Lên) ===" << std::endl;
        std::cout << "Click cards to select/deselect, then click Play or Pass." << std::endl;
        std::cout << "Keyboard commands still work: play <cards>, pass, sort, quit" << std::endl;
        std::cout << "> " << std::flush;
    }

    void run() {
        std::thread inputThread(&Game::inputLoop, this);

        while (window.isOpen() && running) {
            handleEvents();
            processCommands();

            if (needsRedraw) {
                render();
                needsRedraw = false;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }

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

    std::queue<std::string> commandQueue;
    std::mutex queueMutex;

    GameState gameState;
    std::string gameStatus = "Welcome! Starting a new game...";
    bool showingTransition = false;

    // Per-card selection flags (parallel to current player's hand)
    std::vector<bool> selectedCards_;
    bool playButtonHovered_;
    bool passButtonHovered_;
    bool helpButtonHovered_ = false;
    bool helpVisible_ = false;
    bool isFullscreen_ = false;

    // -------------------------------------------------------------------------
    // Initialization
    // -------------------------------------------------------------------------

    void initializeTestGame() {
        gameState.initializePlayers(4, 4);
        gameState.startNewGame();
        gameStatus = "Game started! " + gameState.getStatusMessage();

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

        showingTransition = true;
        needsRedraw = true;
    }

    // -------------------------------------------------------------------------
    // Input thread (CLI fallback)
    // -------------------------------------------------------------------------

    void inputLoop() {
        std::string line;
        while (running) {
            if (std::getline(std::cin, line)) {
                if (!line.empty()) {
                    {
                        std::lock_guard<std::mutex> lock(queueMutex);
                        commandQueue.push(line);
                    }
                    needsRedraw = true;
                }
                if (running) {
                    std::cout << "> " << std::flush;
                }
            } else {
                running = false;
                break;
            }
        }
    }

    // -------------------------------------------------------------------------
    // View helpers
    // -------------------------------------------------------------------------

    /**
     * Returns a view that centres the 1280x720 game area in the current window.
     * window.clear() (called with the default view) fills the full screen with
     * green first; this view then confines game drawing to the centred sub-area.
     */
    sf::View computeGameView() const {
        const float gw = Renderer::GAME_WIDTH, gh = Renderer::GAME_HEIGHT;
        float ww = static_cast<float>(window.getSize().x);
        float wh = static_cast<float>(window.getSize().y);

        float winAspect  = ww / wh;
        float gameAspect = gw / gh;

        float vpW, vpH, vpX, vpY;
        // Use a small epsilon so that same-aspect displays (e.g. 1920x1080 vs
        // 1280x720) snap to a full 1.0 viewport and don't leave pixel-thin gaps.
        constexpr float eps = 0.002f;
        if (winAspect > gameAspect + eps) {
            // Window is wider → green bars on the sides
            vpH = 1.0f;
            vpW = gameAspect / winAspect;
            vpX = (1.0f - vpW) / 2.0f;
            vpY = 0.0f;
        } else if (winAspect < gameAspect - eps) {
            // Window is taller → green bars on top/bottom
            vpW = 1.0f;
            vpH = winAspect / gameAspect;
            vpX = 0.0f;
            vpY = (1.0f - vpH) / 2.0f;
        } else {
            // Same aspect ratio — fill the whole viewport exactly
            vpW = 1.0f; vpH = 1.0f; vpX = 0.0f; vpY = 0.0f;
        }

        sf::View view(sf::FloatRect({0.f, 0.f}, {gw, gh}));
        view.setViewport(sf::FloatRect({vpX, vpY}, {vpW, vpH}));
        return view;
    }

    /** Convert a raw window pixel position to game (1280x720) coordinates. */
    sf::Vector2f pixelToGame(int px, int py) const {
        return window.mapPixelToCoords({px, py}, computeGameView());
    }

    void toggleFullscreen() {
        isFullscreen_ = !isFullscreen_;
        if (isFullscreen_) {
            window.create(sf::VideoMode::getDesktopMode(), "Thirteen - Big Two",
                          sf::State::Fullscreen);
        } else {
            window.create(sf::VideoMode({1280u, 720u}), "Thirteen - Big Two");
        }
        window.setFramerateLimit(60);
    }

    // -------------------------------------------------------------------------
    // Event handling
    // -------------------------------------------------------------------------

    void handleEvents() {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                running = false;
            }

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Escape) {
                    if (isFullscreen_) {
                        toggleFullscreen();  // Escape exits fullscreen first
                    } else {
                        window.close();
                        running = false;
                    }
                }
                if (keyPressed->code == sf::Keyboard::Key::F11) {
                    toggleFullscreen();
                }
                if (keyPressed->code == sf::Keyboard::Key::Space && showingTransition) {
                    handleTransitionContinue();
                }
            }

            // Snapshot showingTransition so that actions taken during this event
            // cannot cause the transition handler below to fire in the same iteration.
            const bool wasTransition = showingTransition;

            // Mouse only active when showing the game board
            if (!wasTransition && gameState.getPhase() != GamePhase::Finished) {
                if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
                    auto gp = pixelToGame(mouseMoved->position.x, mouseMoved->position.y);
                    handleMouseMoved(gp.x, gp.y);
                }
                if (const auto* mouseClick = event->getIf<sf::Event::MouseButtonPressed>()) {
                    if (mouseClick->button == sf::Mouse::Button::Left) {
                        auto gp = pixelToGame(mouseClick->position.x, mouseClick->position.y);
                        handleMouseClick(gp.x, gp.y);
                    }
                }
            }
            if (const auto* mouseClick = event->getIf<sf::Event::MouseButtonPressed>()) {
                if ((mouseClick->button == sf::Mouse::Button::Right || mouseClick->button == sf::Mouse::Button::Left) && wasTransition) {
                    handleTransitionContinue();
                }
            }
        }
    }

    void handleMouseMoved(float x, float y) {
        bool newPlay = isPointInRect(x, y, BTN_X, PLAY_BTN_Y, BTN_WIDTH, BTN_HEIGHT);
        bool newPass = isPointInRect(x, y, BTN_X, PASS_BTN_Y, BTN_WIDTH, BTN_HEIGHT);
        bool newHelp = isPointInRect(x, y, HELP_BTN_X, HELP_BTN_Y, HELP_BTN_W, HELP_BTN_H);
        if (newPlay != playButtonHovered_ || newPass != passButtonHovered_ || newHelp != helpButtonHovered_) {
            playButtonHovered_ = newPlay;
            passButtonHovered_ = newPass;
            helpButtonHovered_ = newHelp;
            needsRedraw = true;
        }
    }

    void handleMouseClick(float x, float y) {
        if (isPointInRect(x, y, HELP_BTN_X, HELP_BTN_Y, HELP_BTN_W, HELP_BTN_H)) {
            helpVisible_ = !helpVisible_;
            needsRedraw = true;
            return;
        }
        if (isPointInRect(x, y, BTN_X, PLAY_BTN_Y, BTN_WIDTH, BTN_HEIGHT)) {
            handlePlayButton();
            return;
        }
        if (isPointInRect(x, y, BTN_X, PASS_BTN_Y, BTN_WIDTH, BTN_HEIGHT)) {
            handlePassCommand();
            return;
        }

        int cardIdx = getCardIndexAtPoint(x, y);
        if (cardIdx >= 0) {
            selectedCards_[cardIdx] = !selectedCards_[cardIdx];
            needsRedraw = true;
        }
    }

    // -------------------------------------------------------------------------
    // Hit-testing helpers
    // -------------------------------------------------------------------------

    bool isPointInRect(float px, float py, float rx, float ry, float rw, float rh) const {
        return px >= rx && px < rx + rw && py >= ry && py < ry + rh;
    }

    /**
     * Returns the index of the card under (mx, my), or -1.
     * Checks in reverse order so the topmost (rightmost overlap) card wins.
     * Accounts for selected cards being offset upward.
     */
    int getCardIndexAtPoint(float mx, float my) const {
        const Player* currentPlayer = gameState.getCurrentPlayer();
        if (!currentPlayer) return -1;

        const auto& cards = currentPlayer->getHand().getCards();
        if (cards.empty()) return -1;

        float handX = 50.0f;
        float handY = renderer.getWindowHeight() - CardSprite::CARD_HEIGHT - 20.0f;
        float spacing = renderer.calculateCardSpacing(cards.size(), renderer.getWindowWidth() - 100.0f);

        for (int i = static_cast<int>(cards.size()) - 1; i >= 0; --i) {
            float cardX = handX + i * spacing;
            float cardY = (i < static_cast<int>(selectedCards_.size()) && selectedCards_[i])
                ? handY - CARD_SELECT_OFFSET
                : handY;

            if (isPointInRect(mx, my, cardX, cardY, CardSprite::CARD_WIDTH, CardSprite::CARD_HEIGHT)) {
                return i;
            }
        }
        return -1;
    }

    // -------------------------------------------------------------------------
    // Selection management
    // -------------------------------------------------------------------------

    void resetSelection() {
        const Player* p = gameState.getCurrentPlayer();
        size_t handSize = p ? p->getHand().size() : 0;
        selectedCards_.assign(handSize, false);
    }

    // -------------------------------------------------------------------------
    // Command queue processing (CLI fallback)
    // -------------------------------------------------------------------------

    void processCommands() {
        std::lock_guard<std::mutex> lock(queueMutex);
        while (!commandQueue.empty()) {
            std::string command = commandQueue.front();
            commandQueue.pop();
            handleCommand(command);
            needsRedraw = true;
        }
    }

    void handleCommand(const std::string& command) {
        std::cout << "Processing: " << command << std::endl;

        if (command == "quit" || command == "exit") {
            std::cout << "Thanks for playing!" << std::endl;
            window.close();
            running = false;
        } else if (command == "pass") {
            handlePassCommand();
        } else if (command.starts_with("play ")) {
            handlePlayCommand(command.substr(5));
        } else if (command == "sort" || command == "sort rank") {
            Player* p = gameState.getCurrentPlayer();
            if (p) {
                p->getHand().sort(SortOrder::ByRank);
                resetSelection();
                gameStatus = "Hand sorted by rank.";
                std::cout << gameStatus << std::endl;
            }
        } else if (command == "sort suit") {
            Player* p = gameState.getCurrentPlayer();
            if (p) {
                p->getHand().sort(SortOrder::BySuit);
                resetSelection();
                gameStatus = "Hand sorted by suit.";
                std::cout << gameStatus << std::endl;
            }
        } else if (command == "help") {
            printHelp();
        } else {
            std::cout << "Unknown command. Type 'help' for commands." << std::endl;
        }
    }

    // -------------------------------------------------------------------------
    // Game actions
    // -------------------------------------------------------------------------

    void handlePassCommand() {
        Player* currentPlayer = gameState.getCurrentPlayer();
        if (!currentPlayer) return;

        currentPlayer->setHasPassed(true);
        gameState.incrementPasses();
        gameStatus = currentPlayer->getName() + " passed.";
        std::cout << gameStatus << std::endl;

        gameState.nextTurn();

        if (gameState.allOthersHavePassed()) {
            const Player* roundWinner = gameState.getLastPlayingPlayer();
            if (roundWinner) {
                std::cout << "All others passed. " << roundWinner->getName()
                          << " wins the round!" << std::endl;
            }
            gameState.clearLastPlay();
        }

        showingTransition = true;
        gameStatus = gameState.getStatusMessage();
        std::cout << "\n=== Press SPACE to continue to next player ===" << std::endl;
        needsRedraw = true;
    }

    /**
     * Shared play execution used by both mouse (handlePlayButton) and CLI (handlePlayCommand).
     */
    void executePlay(const std::vector<Card>& cards) {
        Player* currentPlayer = gameState.getCurrentPlayer();
        if (!currentPlayer) return;

        bool isFirstPlay    = gameState.getLastPlay().empty();
        bool mustInclude3S  = gameState.isFirstPlayOfGame();

        PlayValidation validation = GameRules::validatePlay(
            cards, gameState.getLastPlay(), isFirstPlay, mustInclude3S);

        if (!validation.isValid) {
            gameStatus = "Invalid: " + validation.errorMessage;
            std::cout << "Invalid play: " << validation.errorMessage << std::endl;
            needsRedraw = true;
            return;
        }

        if (!currentPlayer->getHand().removeCards(cards)) {
            gameStatus = "Error playing cards.";
            needsRedraw = true;
            return;
        }

        gameState.setLastPlay(cards, gameState.getCurrentPlayerIndex());
        if (gameState.isFirstPlayOfGame()) gameState.setFirstPlayMade();

        std::string playName = GameRules::getPlayTypeName(validation.playType);
        gameStatus = currentPlayer->getName() + " played " + playName;
        std::cout << gameStatus << std::endl;

        if (currentPlayer->hasWon()) {
            gameStatus = currentPlayer->getName() + " wins!";
            gameState.setPhase(GamePhase::Finished);
            std::cout << "\n" << gameStatus << "\n" << std::endl;
            needsRedraw = true;
            return;
        }

        std::cout << "Cards remaining: " << currentPlayer->getHand().size() << std::endl;
        gameState.nextTurn();
        showingTransition = true;
        gameStatus = gameState.getStatusMessage();
        std::cout << "=== Press SPACE to continue to next player ===" << std::endl;
        needsRedraw = true;
    }

    /** Called when the Play button is clicked. */
    void handlePlayButton() {
        const Player* currentPlayer = gameState.getCurrentPlayer();
        if (!currentPlayer) return;

        const auto& cards = currentPlayer->getHand().getCards();
        std::vector<Card> toPlay;
        for (size_t i = 0; i < selectedCards_.size() && i < cards.size(); ++i) {
            if (selectedCards_[i]) toPlay.push_back(cards[i]);
        }

        if (toPlay.empty()) {
            gameStatus = "Select cards to play first.";
            needsRedraw = true;
            return;
        }

        executePlay(toPlay);
    }

    /** Called by CLI "play <cards>" command. */
    void handlePlayCommand(const std::string& cardsStr) {
        Player* currentPlayer = gameState.getCurrentPlayer();
        if (!currentPlayer) {
            std::cout << "Error: No current player!" << std::endl;
            return;
        }

        std::vector<std::string> cardStrs;
        std::istringstream iss(cardsStr);
        std::string token;
        while (iss >> token) cardStrs.push_back(token);

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

        executePlay(cards);
    }

    void handleTransitionContinue() {
        showingTransition = false;
        resetSelection();
        needsRedraw = true;

        Player* currentPlayer = gameState.getCurrentPlayer();
        if (currentPlayer) {
            std::cout << "\n=== " << currentPlayer->getName() << "'s Turn ===" << std::endl;
            std::cout << "Click cards to select, then click Play or Pass." << std::endl;
            std::cout << "> " << std::flush;
        }
    }

    // -------------------------------------------------------------------------
    // Rendering
    // -------------------------------------------------------------------------

    void render() {
        // Reset to default view so clear() covers the full framebuffer, not
        // just the last game viewport (matters on the second+ frame).
        window.setView(window.getDefaultView());
        renderer.clear();                    // fills entire window with green
        window.setView(computeGameView());   // centre 1280x720 game area
        if (showingTransition) {
            drawTransitionScreen();
        } else {
            drawGameScreen();
        }
        renderer.present();
    }

    // Draw all 52 cards around the screen border.
    // 18 top, 18 bottom, 10 left, 10 right — the 4 corner cards are shared
    // between edges, giving 18+18+10+10-4 = 52 unique cards.
    // Card indices: 0-17 top, 17-26 right, 26-43 bottom, 43-51+0 left.
    void drawCardBorder() {
        const std::vector<Rank> ranks = {
            Rank::Three, Rank::Four, Rank::Five, Rank::Six,
            Rank::Seven, Rank::Eight, Rank::Nine, Rank::Ten,
            Rank::Jack, Rank::Queen, Rank::King, Rank::Ace, Rank::Two
        };
        const std::vector<Suit> suits = { Suit::Spades, Suit::Hearts, Suit::Diamonds, Suit::Clubs };

        std::vector<Card> allCards;
        allCards.reserve(52);
        for (Rank r : ranks)
            for (Suit s : suits)
                allCards.emplace_back(r, s);

        const float W = renderer.getWindowWidth();
        const float H = renderer.getWindowHeight();
        const float scale = 0.5f;
        const float cw = CardSprite::CARD_WIDTH  * scale;
        const float ch = CardSprite::CARD_HEIGHT * scale;

        // Top edge (cards 0-18): left to right, corners land exactly at edges
        for (int i = 0; i < 19; ++i) {
            float x = i * (W - cw) / 18.0f;
            renderer.drawCard(allCards[i], x, 0, false, true, scale);
        }
        // Right edge (cards 18-26): top to bottom, card 18 is top-right corner
        for (int i = 0; i < 9; ++i) {
            float y = i * (H - ch) / 8.0f;
            renderer.drawCard(allCards[18 + i], W - cw, y, false, true, scale);
        }
        // Bottom edge (cards 26-44): right to left, card 26 is bottom-right corner
        for (int i = 0; i < 19; ++i) {
            float x = (W - cw) - i * (W - cw) / 18.0f;
            renderer.drawCard(allCards[26 + i], x, H - ch, false, true, scale);
        }
        // Left edge (cards 44-51, 0): bottom to top, card 44 is bottom-left corner
        for (int i = 0; i < 9; ++i) {
            float y = (H - ch) - i * (H - ch) / 8.0f;
            renderer.drawCard(allCards[(44 + i) % 52], 0, y, false, true, scale);
        }
    }

    void drawTransitionScreen() {
        Player* nextPlayer = gameState.getCurrentPlayer();
        if (!nextPlayer) return;

        drawCardBorder();

        renderer.drawText(nextPlayer->getName() + "'s Turn",
            renderer.getCenterX(), renderer.getCenterY() - 100,
            48, sf::Color::White, TextAlign::Center);

        renderer.drawText("Cards in hand: " + std::to_string(nextPlayer->getHand().size()),
            renderer.getCenterX(), renderer.getCenterY(),
            20, sf::Color::White, TextAlign::Center);

        renderer.drawText("Press SPACE to show your cards",
            renderer.getCenterX(), renderer.getCenterY() + 50,
            24, sf::Color(200, 200, 200), TextAlign::Center);
    }

    void drawGameScreen() {
        // Status panel (shortened to leave room for help button)
        renderer.drawStatusPanel(gameStatus, 50, 20,
            HELP_BTN_X - 70, 50);

        // Help button ("?") in top-right corner
        renderer.drawButton("?", HELP_BTN_X, HELP_BTN_Y, HELP_BTN_W, HELP_BTN_H, helpButtonHovered_);

        // Help panel
        if (helpVisible_) {
            const float px = 870.0f, py = 65.0f, pw = 390.0f, ph = 205.0f;
            renderer.drawStatusPanel("", px, py, pw, ph);
            renderer.drawText("How to Play", px + pw / 2, py + 10, 18, sf::Color::Yellow, TextAlign::Center);
            const sf::Color c(220, 220, 220);
            renderer.drawText("Click a card        - select / deselect",   px + 15, py + 40,  15, c);
            renderer.drawText("Play Selected btn   - play chosen cards",    px + 15, py + 65,  15, c);
            renderer.drawText("Pass btn            - skip your turn",       px + 15, py + 90,  15, c);
            renderer.drawText("Space / click       - advance to next player", px + 15, py + 115, 15, c);
            renderer.drawText("F11                 - toggle fullscreen",    px + 15, py + 140, 15, c);
            renderer.drawText("Esc                 - exit fullscreen / quit", px + 15, py + 165, 15, c);
        }

        // Player info panels (2x2 grid)
        for (size_t i = 0; i < gameState.getNumPlayers(); ++i) {
            const Player* player = gameState.getPlayer(i);
            if (!player) continue;
            bool isActive = (i == gameState.getCurrentPlayerIndex());
            float x = 50.0f + (i % 2) * 600.0f;
            float y = 100.0f + (i / 2) * 70.0f;
            renderer.drawPlayerInfo(player->getName(), player->getHand().size(), x, y, isActive);
        }

        // Center play area
        renderer.drawPlayArea(gameState.getLastPlay(),
            renderer.getCenterX(), renderer.getCenterY());

        // Current player's hand (with selection offsets)
        const Player* currentPlayer = gameState.getCurrentPlayer();
        if (currentPlayer) {
            float handY = renderer.getWindowHeight() - CardSprite::CARD_HEIGHT - 20.0f;
            renderer.drawHand(currentPlayer->getHand(), 50.0f, handY, true, selectedCards_);
            renderer.drawText(currentPlayer->getName() + "'s Hand:",
                50.0f, handY - 25.0f, 18, sf::Color::Yellow);
        }

        // Action buttons
        if (gameState.getPhase() == GamePhase::InProgress) {
            renderer.drawButton("Play Selected",
                BTN_X, PLAY_BTN_Y, BTN_WIDTH, BTN_HEIGHT, playButtonHovered_);
            renderer.drawButton("Pass",
                BTN_X, PASS_BTN_Y, BTN_WIDTH, BTN_HEIGHT, passButtonHovered_);
        }
    }

    void printHelp() {
        std::cout << "\n=== Commands ===" << std::endl;
        std::cout << "  Click cards       - Select/deselect (card rises when selected)" << std::endl;
        std::cout << "  Play Selected btn - Play selected cards" << std::endl;
        std::cout << "  Pass btn          - Pass your turn" << std::endl;
        std::cout << "  play <cards>      - CLI: e.g. 'play 3H 4H'" << std::endl;
        std::cout << "  pass              - CLI: pass your turn" << std::endl;
        std::cout << "  sort [rank|suit]  - Sort your hand" << std::endl;
        std::cout << "  quit              - Exit game" << std::endl;
        std::cout << std::endl;
    }
};

int main() {
    try {
        Game game;
        game.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

# thirteen-game

Welcome to my repository for a fully-featured version of the card game 'thirteen'. This is a card game I played at lunch times during high school with friends, and here I have brought it to life as a modern c++ 20 project utilising SFML 3 for graphics rendering.

The program features a complete rule engine, pass and play multiplayer for four human players, and a clear seperation of game logic from presentation.

![image alt](https://github.com/myszkov/thirteen-game/blob/6d7f5dea9ad292c39a72591c565adda4e2895745/thirteen-game-player-1-turn-new.JPG)
Main game interface showing cards, player information and play area

![image alt](https://github.com/myszkov/thirteen-game/blob/73d52bdc325a81c3fa16892372fb33c7e1f50f77/thirteen-player-1.JPG)
Example of a sequence play with multiple cards

![image alt](https://github.com/myszkov/thirteen-game/blob/73d52bdc325a81c3fa16892372fb33c7e1f50f77/thirteen-player-4-turn-console.JPG)
Terminal command system for gameplay

🎮 Features
Core Gameplay

✅ Complete Thirteen Rules - Authentic Vietnamese card game implementation
✅ Suit Ranking - Hearts > Diamonds > Clubs > Spades (reversed from Big Two)
✅ 7 Play Types - Singles, pairs, triples, four-of-a-kind, sequences, double sequences, triple sequences
  Example: 
✅ Flexible Sequences - 3+ card straights (not limited to 5 cards like Big Two)
✅ Four of a Kind Power Play - Beats any other combination

Multiplayer

🎯 Pass & Play Mode - 4 human players on same computer
🔄 Smooth Turn Transitions - Screen switching to hide cards between players
👥 Player Management - Clear visual indicators for active player

Technical Features

🎨 Procedural Card Rendering - No external texture files needed
🖥️ Dual Interface - SFML graphics + terminal command input
🧵 Multi-threaded Design - Separate threads for rendering and input
🎯 Comprehensive Validation - Full rule checking for all play types
📊 Real-time State Management - Live updates of game state

🛠️ Technologies Used

Language: C++20
Graphics: SFML 3.0
Build System: CMake 3.16+
Architecture: Model-View-Controller pattern
Threading: std::thread, std::atomic, std::mutex
Compiler: MSVC/GCC/Clang (C++20 features)

Design Patterns Implemented

State Pattern - GameState manages game phases and turn flow
Strategy Pattern - GameRules provides validation strategies for different play types
Observer Pattern - Renderer observes and displays GameState changes
Command Pattern - User input processed as commands in queue
Factory Pattern - Card and Player creation with configurable types

Key Technical Decisions

Thread-Safe Command Queue: Enables responsive UI while processing stdin
Immutable Card Objects: Cards compared by value, preventing state corruption
Procedural Rendering: Cards drawn algorithmically for resolution independence
Modular Validation: Each play type has dedicated validation logic
Flat Structure Option: Supports both organized and flat file layouts

🎯 Game Rules (Thirteen / Tiến Lên)
Objective
Be the first player to play all your cards.
Card Ranking
Rank: 2 (highest) > A > K > Q > J > 10 > 9 > 8 > 7 > 6 > 5 > 4 > 3 (lowest)
Suit: ♥ Hearts (highest) > ♦ Diamonds > ♣ Clubs > ♠ Spades (lowest)

Special Rules

Game starts with player holding 3♠ (must include in first play)
Players must match the type and number of cards from previous play
Four of a kind can beat ANY other play type
Pass if you cannot or don't want to play
When all others pass, you win the round and can play anything

🧪 Testing & Quality Assurance
Manual Testing Coverage

✅ All 7 play types validated
✅ Turn sequence management
✅ Round win detection
✅ Four of a kind special rules
✅ Starting card enforcement (3♠)
✅ Pass & play transitions
✅ Card sorting (rank/suit)

Error Handling

Invalid card combinations detected
Cards not in hand prevented
Turn sequence enforced
Thread-safe command processing

Code Style

Follow existing naming conventions (PascalCase for classes, camelCase for variables)
Comment all public functions
Keep functions focused and under 50 lines when possible
Use modern C++20 features where appropriate

🚀 Getting Started
Prerequisites
Windows:

Visual Studio 2019+ or MinGW-w64
CMake 3.16+
SFML 3.0+

Linux/macOS:

GCC 10+ or Clang 12+
CMake 3.16+
SFML 3.0+

Building from Source
1. Install SFML 3
Windows (vcpkg):
bashvcpkg install sfml:x64-windows
Linux (Ubuntu/Debian):
bashsudo apt-get install libsfml-dev
macOS (Homebrew):
bashbrew install sfml
2. Clone Repository
bashgit clone https://github.com/yourusername/thirteen-game.git
cd thirteen-game
3. Configure CMake
Windows:
bashmkdir build && cd build
cmake .. -DSFML_DIR="C:/path/to/SFML/lib/cmake/SFML"
Linux/macOS:
bashmkdir build && cd build
cmake ..
4. Build
bashcmake --build . --config Release
5. Run
bash# Windows
cd Release
thirteen-game.exe

# Linux/macOS
./thirteen-game

🤖 AI-Assisted Development
This project was developed with assistance from Claude (Anthropic's AI assistant). Here's how AI was utilized throughout the development process:
Architecture & Design

Initial Project Structure: AI helped design the layered architecture separating Core, Game Logic, and Rendering
Design Pattern Selection: Recommended and implemented State, Strategy, and Observer patterns
File Organization: Suggested modular structure with clear separation of concerns

Implementation Support

SFML 3 API Migration: AI provided crucial guidance on SFML 3 breaking changes (Vector2f constructors, sf::degrees(), bounds.size syntax)
Modern C++20 Features: Implemented std::optional, std::ranges, structured bindings, and concepts
Thread Safety: Designed and implemented thread-safe command queue with proper mutex usage
Rule Engine Logic: Developed comprehensive validation for all 7 Thirteen play types

Problem Solving

Bug Fixes: Identified and fixed issues with:

Missing closing braces
Incorrect suit ranking (reversed for Thirteen vs Big Two)
Four of a kind validation conflicts
Player transition state management


Build System: Resolved CMake configuration for SFML 3 on Windows
Code Refactoring: Converted from Big Two rules to Thirteen rules while preserving old code in comments

Documentation

Comprehensive Comments: Every function documented with purpose and parameters
README Creation: This document structure and content
User Guides: Created PASS_AND_PLAY_GUIDE.md and THIRTEEN_CONVERSION.md

Learning Approach
Rather than AI writing all code, the collaboration involved:

Explaining Concepts: AI explained card game rules, C++ patterns, and SFML usage
Iterative Development: User implemented code, AI reviewed and suggested improvements
Error Resolution: User reported errors, AI diagnosed and provided fixes
Feature Addition: Collaborative design of features like Pass & Play mode

This approach ensured genuine learning while leveraging AI for:

Rapid prototyping
Best practices guidance
API reference lookup
Code review and optimization

Key Takeaway: AI served as a knowledgeable pair programmer and mentor, accelerating development while ensuring understanding of all implemented features.

📝 License
This project is licensed under the MIT License - see the LICENSE file for details.

🙏 Acknowledgments

SFML Community - For excellent documentation and examples
Anthropic's Claude - AI pair programming partner throughout development
Vietnamese Card Game Community - For Thirteen (Tiến Lên) rules reference
Modern C++ Community - For C++20 best practices and patterns


📧 Contact
Your Name - your.email@example.com
Project Link: https://github.com/yourusername/thirteen-game

📊 Project Stats

Lines of Code: ~3,500+
Development Time: [Your timeframe]
Language: C++20
Files: 20+ source/header files
Design Patterns: 5+
Play Types Supported: 7

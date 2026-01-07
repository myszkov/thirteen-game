cmake_minimum_required(VERSION 3.16)
project(Thirteen VERSION 1.0 LANGUAGES CXX)

# Set C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add compiler warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# SFML Configuration - Set your SFML path here
if(WIN32)
    # Update this path to match YOUR SFML installation
    set(SFML_DIR "C:/Users/Viktor/source/SFML/lib/cmake/SFML")
endif()

# Find SFML 3 (note: component names are capitalized in SFML 3)
find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

# Include directories - point to the include/ folder
include_directories(${CMAKE_SOURCE_DIR}/include)

# ============================================
# Collect Source Files by Layer
# ============================================

# Core Layer - Card game fundamentals
set(CORE_SOURCES
    src/Core/Card.cpp
    src/Core/Deck.cpp
    src/Core/Hand.cpp
)

# Game Logic Layer - Rules and state management
set(GAME_SOURCES
    src/Game/Player.cpp
    src/Game/GameState.cpp
    src/Game/GameRules.cpp
)

# Rendering Layer - Graphics and UI
set(RENDERING_SOURCES
    src/Rendering/Renderer.cpp
    src/Rendering/CardSprite.cpp
    src/Rendering/UIElements.cpp
)

# Combine all sources
set(ALL_SOURCES
    ${CORE_SOURCES}
    ${GAME_SOURCES}
    ${RENDERING_SOURCES}
    main.cpp
)

# ============================================
# Create Executable
# ============================================

add_executable(thirteen-game ${ALL_SOURCES})

# Link SFML libraries (SFML 3 uses SFML:: namespace)
target_link_libraries(thirteen-game PRIVATE 
    SFML::Graphics
    SFML::Window
    SFML::System
)

# ============================================
# Windows-Specific Configuration
# ============================================

if(WIN32)
    # Automatically copy SFML DLLs to output directory
    add_custom_command(TARGET thirteen-game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SFML::Graphics>
            $<TARGET_FILE:SFML::Window>
            $<TARGET_FILE:SFML::System>
            $<TARGET_FILE_DIR:thirteen-game>
        COMMENT "Copying SFML DLLs to output directory"
    )
    
    # Set working directory for Visual Studio debugger
    set_target_properties(thirteen-game PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    )
endif()

# ============================================
# Build Information
# ============================================

message(STATUS "")
message(STATUS "=== Thirteen Card Game - Build Configuration ===")
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "")
if(SFML_FOUND)
    message(STATUS "SFML Found: YES")
    message(STATUS "SFML Version: ${SFML_VERSION}")
    message(STATUS "SFML Directory: ${SFML_DIR}")
else()
    message(STATUS "SFML Found: NO")
    message(WARNING "SFML not found! Set SFML_DIR or SFML_ROOT.")
endif()
message(STATUS "")
message(STATUS "Source Files:")
message(STATUS "  Core: ${CORE_SOURCES}")
message(STATUS "  Game: ${GAME_SOURCES}")
message(STATUS "  Rendering: ${RENDERING_SOURCES}")
message(STATUS "")
message(STATUS "===============================================")
message(STATUS "")
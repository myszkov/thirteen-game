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

# Viktor's SFML installation path
if(WIN32)
    set(SFML_DIR "C:/Users/Viktor/source/SFML/lib/cmake/SFML")
endif()

# Find SFML 3 (SFML 3 uses capitalized component names)
find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

# All source files in the root directory
set(ALL_SOURCES
    main.cpp
    Card.cpp
    Deck.cpp
    Hand.cpp
    Player.cpp
    GameState.cpp
    GameRules.cpp
    Renderer.cpp
    CardSprite.cpp
    UIElements.cpp
)

# Create executable
add_executable(thirteen-game ${ALL_SOURCES})

# Link SFML libraries
target_link_libraries(thirteen-game PRIVATE 
    SFML::Graphics
    SFML::Window
    SFML::System
)

# Windows-specific: Copy SFML DLLs to output directory
if(WIN32)
    add_custom_command(TARGET thirteen-game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SFML::Graphics>
            $<TARGET_FILE:SFML::Window>
            $<TARGET_FILE:SFML::System>
            $<TARGET_FILE_DIR:thirteen-game>
        COMMENT "Copying SFML DLLs to output directory"
    )
    
    set_target_properties(thirteen-game PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    )
endif()

# Print build configuration
message(STATUS "=== Thirteen Build Configuration ===")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
if(SFML_FOUND)
    message(STATUS "SFML Found: YES")
    message(STATUS "SFML Version: ${SFML_VERSION}")
else()
    message(STATUS "SFML Found: NO")
endif()
message(STATUS "====================================")
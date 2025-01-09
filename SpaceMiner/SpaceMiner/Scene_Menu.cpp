#include "Scene_Menu.h"
#include "GameEngine.h"
#include <iostream>
#include "Scene_Play.h"

Scene_Menu::Scene_Menu(GameEngine* game) : Scene(game) {
    // Load background texture
    if (!backgroundTexture.loadFromFile("../assets/images/space_background.png")) {
        std::cerr << "ERROR: Failed to load menu background!" << std::endl;
    }
    else {
        std::cout << "Menu background loaded successfully!" << std::endl;
    }

    backgroundSprite.setTexture(backgroundTexture);

    // Scale the background to fit the window size
    sf::Vector2u textureSize = backgroundTexture.getSize();
    sf::Vector2u windowSize = m_game->getWindow()->getSize();
    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
    backgroundSprite.setScale(scaleX, scaleY);

    // Load font for buttons
    if (!font.loadFromFile("../assets/fonts/arial.ttf")) {
        std::cerr << "ERROR: Failed to load font!" << std::endl;
    }

    // Set up "Start" button
    startButton.setFont(font);
    startButton.setString("START GAME");
    startButton.setCharacterSize(80);
    startButton.setFillColor(sf::Color::White);

    // Center the button in the window
    sf::FloatRect textBounds = startButton.getLocalBounds();
    startButton.setOrigin(textBounds.width / 2, textBounds.height / 2);
    startButton.setPosition(windowSize.x / 2, windowSize.y * 0.8f); // 80% down the screen
}

void Scene_Menu::update() {
    // Handle mouse input
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(*m_game->getWindow());

        // Check if the user clicks the "Start" button
        if (startButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            std::cout << "Starting Game..." << std::endl;
            m_game->changeScene(std::make_shared<Scene_Play>(m_game)); // Change to game scene
        }
    }
}

void Scene_Menu::render(sf::RenderWindow& window) {
    window.clear();
    window.draw(backgroundSprite);
    window.draw(startButton);
    window.display();
}
#include "Scene_Play.h"
#include "GameEngine.h"
#include <iostream>

Scene_Play::Scene_Play(GameEngine* game) : Scene(game) {
    // Load player texture
    if (!playerTexture.loadFromFile("assets/images/player.png")) {
        std::cerr << "ERROR: Failed to load player texture!" << std::endl;
    }
    playerSprite.setTexture(playerTexture);
    playerSprite.setPosition(400, 300);
}

void Scene_Play::update() {
    float speed = 5.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) playerSprite.move(-speed, 0);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) playerSprite.move(speed, 0);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) playerSprite.move(0, -speed);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) playerSprite.move(0, speed);
}

void Scene_Play::render(sf::RenderWindow& window) {
    window.clear();
    window.draw(playerSprite);
    window.display();
}
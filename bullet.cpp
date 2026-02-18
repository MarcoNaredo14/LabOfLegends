// bullet.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

class Bullet {
private:
    sf::Vector2f position;
    sf::Vector2f direction;
    sf::Sprite sprite;
    float speed = 2.5f;  // bullet speed

    void updateRotation() {
       
        float angle = 0.0f;
        if (direction.x > 0) angle = 0.0f;        // Right
        else if (direction.x < 0) angle = 180.0f;  // Left
        else if (direction.y < 0) angle = 270.0f;  // Up
        else if (direction.y > 0) angle = 90.0f;   // Down
        
        sprite.setRotation(angle);
    }

public:
    Bullet(float x, float y, const sf::Vector2f& dir, const std::string& texturePath) 
        : position(x, y), direction(dir) {
        static sf::Texture bulletTexture;
        static bool textureLoaded = false;
        
        if (!textureLoaded) {
            if (!bulletTexture.loadFromFile(texturePath)) {
                std::cout << "Failed to load bullet texture!" << std::endl;
            }
            textureLoaded = true;
        }
        
        sprite.setTexture(bulletTexture);
        sprite.setPosition(position);
        sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
        sprite.setScale(0.5f, 0.5f);
        
       
        updateRotation();
    }

    void update(float deltaTime) {
       
        position.x += direction.x * speed;
        position.y += direction.y * speed;
        sprite.setPosition(position);
        updateRotation(); 
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }

    sf::Vector2f getPosition() const {
        return position;
    }

    
    sf::FloatRect getGlobalBounds() const {
        return sprite.getGlobalBounds();
    }

    
    sf::FloatRect getBounds() const {
        return sprite.getLocalBounds();
    }
};

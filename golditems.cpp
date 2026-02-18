#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>

class GoldItem {
private:
    sf::Sprite sprite;
    sf::Vector2f position;
    bool collected = false;
    static std::vector<sf::Texture> textures;
    static bool texturesLoaded;

    // Animation properties
    float floatOffset = 0.0f;
    float floatSpeed = 0.8f;
    float floatAmount = 5.0f;
    const float PI = 3.14159f;

public:
    GoldItem(float x, float y) : position(x, y) {
        if (!texturesLoaded) {
            
            for (int i = 1; i <= 4; i++) {
                sf::Texture tex;
                if (!tex.loadFromFile("items/items (" + std::to_string(i) + ").png")) {
                    std::cout << "Failed to load gold" << i << ".png!" << std::endl;
                }
                textures.push_back(tex);
            }
            texturesLoaded = true;
        }
        int randomIndex = rand() % 4;
        sprite.setTexture(textures[randomIndex]);
        sprite.setPosition(position);
        
        float scale = 35.0f / sprite.getLocalBounds().width;
        sprite.setScale(scale, scale);
    }

    void update(float deltaTime) {
        if (!collected) {
            
            floatOffset += (floatSpeed * 0.5f) * deltaTime;  
            if (floatOffset >= 2 * PI) {
                floatOffset -= 2 * PI;
            }
            // Update position with floating effect
            sprite.setPosition(position.x, 
                             position.y + std::sin(floatOffset) * floatAmount);
        }
    }

    void draw(sf::RenderWindow& window) {
        if (!collected) {
            window.draw(sprite);
        }
    }

    bool isCollected() const {
        return collected;
    }

    void collect() {
        collected = true;
    }

    sf::FloatRect getBounds() const {
        return sprite.getGlobalBounds();
    }

    sf::Vector2f getPosition() const {
        return position;
    }
};

std::vector<sf::Texture> GoldItem::textures;
bool GoldItem::texturesLoaded = false;

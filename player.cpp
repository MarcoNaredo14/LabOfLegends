#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Player {
private:
    static const int ANIMATION_FRAMES ;  // Number of frames per direction
    static const float ANIMATION_SPEED ;  // Seconds per frame
    
    sf::Vector2f position;
    sf::Sprite sprite;
    std::vector<sf::Texture> texturesRight;
    std::vector<sf::Texture> texturesLeft;
    std::vector<sf::Texture> texturesUp;
    std::vector<sf::Texture> texturesDown;
    
    float animationTimer = 0.0f;
    int currentFrame = 0;
   
    bool isMoving = false;

public:
    enum Direction { Right, Left, Up, Down } currentDirection = Right;
    Player(float startX, float startY) : position(startX, startY) {
        // Load textures for each direction
        for (int i = 1; i <= ANIMATION_FRAMES; i++) {
            sf::Texture tex;
            
            // Right animations
            if (tex.loadFromFile("player/right" + std::to_string(i) + ".png")) {
                texturesRight.push_back(tex);
            }
            
            // Left animations
            if (tex.loadFromFile("player/left" + std::to_string(i) + ".png")) {
                texturesLeft.push_back(tex);
            }
            
            // Up animations
            if (tex.loadFromFile("player/up" + std::to_string(i) + ".png")) {
                texturesUp.push_back(tex);
            }
            
            // Down animations
            if (tex.loadFromFile("player/down" + std::to_string(i) + ".png")) {
                texturesDown.push_back(tex);
            }
        }

        // Set initial texture
        if (!texturesRight.empty()) {
            sprite.setTexture(texturesRight[0]);
        }
        
        sprite.setPosition(position);
      
        sprite.setScale(35.0f / sprite.getLocalBounds().width, 
                       35.0f / sprite.getLocalBounds().height);
    }

    void move(float dx, float dy, float deltaTime) {
        position.x += dx;
        position.y += dy;
        sprite.setPosition(position);
        
        isMoving = (dx != 0 || dy != 0);
        
        // Set direction based on movement
        if (dx > 0) currentDirection = Right;
        else if (dx < 0) currentDirection = Left;
        else if (dy < 0) currentDirection = Up;
        else if (dy > 0) currentDirection = Down;
        
        // Update animation
        if (isMoving) {
            animationTimer += deltaTime;
            if (animationTimer >= ANIMATION_SPEED) {
                animationTimer = 0;
                currentFrame = (currentFrame + 1) % ANIMATION_FRAMES;
                
               
                switch (currentDirection) {
                    case Right:
                        sprite.setTexture(texturesRight[currentFrame]);
                        break;
                    case Left:
                        sprite.setTexture(texturesLeft[currentFrame]);
                        break;
                    case Up:
                        sprite.setTexture(texturesUp[currentFrame]);
                        break;
                    case Down:
                        sprite.setTexture(texturesDown[currentFrame]);
                        break;
                }
            }
        } else {
            // Reset to standing frame
            currentFrame = 0;
            switch (currentDirection) {
                case Right:
                    sprite.setTexture(texturesRight[0]);
                    break;
                case Left:
                    sprite.setTexture(texturesLeft[0]);
                    break;
                case Up:
                    sprite.setTexture(texturesUp[0]);
                    break;
                case Down:
                    sprite.setTexture(texturesDown[0]);
                    break;
            }
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }

    sf::Vector2f getPosition() const {
        return position;
    }

	enum Direction getDirection() const {
		return currentDirection;
	}

    sf::FloatRect getBounds() const {
        return sprite.getGlobalBounds();
    }
};

const int Player::ANIMATION_FRAMES = 4;
const float Player::ANIMATION_SPEED = 0.2f;

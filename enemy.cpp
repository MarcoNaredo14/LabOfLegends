// enemy.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Enemy {
private:
    static const int ANIMATION_FRAMES;
    static const float ANIMATION_SPEED;
    
    sf::Vector2f position;
    sf::Vector2f targetPosition;
    sf::Sprite sprite;
    std::vector<sf::Texture> texturesRight;
    std::vector<sf::Texture> texturesLeft;
    std::vector<sf::Texture> texturesUp;
    std::vector<sf::Texture> texturesDown;
    
    float speed = 1.1f;
    float animationTimer = 0.0f;
    int currentFrame = 0;
    bool isMoving = false;
    static constexpr float RENDER_SIZE = 30.0f;

    void updateSpriteLayout() {
        const sf::FloatRect bounds = sprite.getLocalBounds();
        sprite.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
        sprite.setScale(RENDER_SIZE / bounds.width, RENDER_SIZE / bounds.height);
        sprite.setPosition(position.x + 17.5f, position.y + 19.0f);
    }
    bool movingHorizontally;
    bool movingRight = true;  // For horizontal movement
    bool movingDown = true;   // For vertical movement
    float movementRange = 100.0f;  // How far the enemy moves before turning
    float initialX;
    float initialY;
    float leftBoundary;   // Leftmost point of movement
    float rightBoundary;  // Rightmost point of movement
    float topBoundary;    // Topmost point of movement
    float bottomBoundary; // Bottommost point of movement
    float shootTimer = 0.8f;
    const float Shoot_cooldown = 2.5f;

public:
    enum Direction { Right, Left, Up, Down } currentDirection = Right;

    Enemy(float startX, float startY, bool isHorizontal, float pathStart, float pathEnd) 
        : position(startX, startY), 
          initialX(startX), 
          initialY(startY),
          movingHorizontally(isHorizontal) 
    {
        if (isHorizontal) {
            leftBoundary = pathStart;
            rightBoundary = pathEnd;
        } else {
            topBoundary = pathStart;
            bottomBoundary = pathEnd;
        }
        // Load textures for each direction
        for (int i = 1; i <= ANIMATION_FRAMES; i++) {
            sf::Texture tex;
            
            // Load enemy textures from enemy folder
            if (tex.loadFromFile("enemy/right (" + std::to_string(i) + ").png")) {
            texturesRight.push_back(tex);
            }
            if (tex.loadFromFile("enemy/left (" + std::to_string(i) + ").png")) {
            texturesLeft.push_back(tex);
            }
            if (tex.loadFromFile("enemy/up (" + std::to_string(i) + ").png")) {
            texturesUp.push_back(tex);
            }
            if (tex.loadFromFile("enemy/down (" + std::to_string(i) + ").png")) {
            texturesDown.push_back(tex);
            }
        }

        if (!texturesRight.empty()) {
            sprite.setTexture(texturesRight[0]);
        }
        
        updateSpriteLayout();
    }

    bool canShoot(){
        return shootTimer <= 0;
    }

    void updateShootTimer(float deltaTime){
        if (shootTimer > 0) {
            shootTimer -= deltaTime;
        }
    }

    void resetShootTimer(){
        shootTimer = Shoot_cooldown;
    }

    sf::Vector2f getShootDirection() const {
        switch (currentDirection) {
            case Right:
                return sf::Vector2f(1.f, 0.f);
            case Left:
                return sf::Vector2f(-1.f, 0.f);
            case Up:
                return sf::Vector2f(0.f, -1.f);
            case Down:
                return sf::Vector2f(0.f, 1.f);
            default:
                return sf::Vector2f(0.f, 0.f);
        }
    }

    void update(float deltaTime, const sf::Vector2f& playerPos) {
        if (movingHorizontally) {
            // Horizontal movement using full path boundaries
            if (movingRight) {
                position.x += speed;
                currentDirection = Right;
                if (position.x >= rightBoundary) {
                    movingRight = false;
                    currentDirection = Left;
                }
            } else {
                position.x -= speed;
                currentDirection = Left;
                if (position.x <= leftBoundary) {
                    movingRight = true;
                    currentDirection = Right;
                }
            }
        } else {
            // Vertical movement using full path boundaries
            if (movingDown) {
                position.y += speed;
                currentDirection = Down;
                if (position.y >= bottomBoundary) {
                    movingDown = false;
                    currentDirection = Up;
                }
            } else {
                position.y -= speed;
                currentDirection = Up;
                if (position.y <= topBoundary) {
                    movingDown = true;
                    currentDirection = Down;
                }
            }
        }

        // Update animation
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
            updateSpriteLayout();
        }
        
        updateSpriteLayout();
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }

    sf::Vector2f getPosition() const {
        return position;
    }

    sf::FloatRect getBounds() const {
        return sf::FloatRect(position.x + 2.5f, position.y + 2.5f, 30.0f, 30.0f);
    }

    // set Position
	void setPosition(sf::Vector2f pos) {
		position = pos;
		updateSpriteLayout();
	}

   
};

const int Enemy::ANIMATION_FRAMES = 8;

const float Enemy::ANIMATION_SPEED = 2.5f;
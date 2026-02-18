#pragma once
#include<SFML/Graphics.hpp>
#include<vector>
#include<iostream>

class Lives {
private:
    int currentLives;
    sf::Text livesText;
    sf::Font font;

public:
    Lives(int initial = 5) : currentLives(initial) {
        if (!font.loadFromFile("menu/menu.otf")) {
            std::cout << "Error loading font" << std::endl;
        }
        livesText.setFont(font);
        livesText.setCharacterSize(24);
        livesText.setFillColor(sf::Color::White);
        livesText.setPosition(10, 40);  
        updateText();
    }

    void loseLife() {
        if (currentLives > 0) {
            currentLives--;
            updateText();
        }
    }

    bool isGameOver() const {
        return currentLives <= 0;
    }

    void updateText() {
        livesText.setString("Lives: " + std::to_string(currentLives));
    }

    void draw(sf::RenderWindow& window) {
        window.draw(livesText);
    }
};

#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

class Scoreboard {
private:
    sf::Text scoreText;
    sf::Font font;
    int score = 0;

public:
    Scoreboard() {
        if (!font.loadFromFile("menu/menu.otf")) {
            std::cout << "Error loading font" << std::endl;
        }
        scoreText.setFont(font);
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(10, 10);
        updateScore(0);
    }

    void updateScore(int newScore) {
        score = newScore;
        scoreText.setString("Score: " + std::to_string(score));
    }

    void draw(sf::RenderWindow& window) {
        window.draw(scoreText);
    }
};
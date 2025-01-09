#ifndef SCENE_H
#define SCENE_H

#include <SFML/Graphics.hpp>

class GameEngine;

class Scene {
protected:
    GameEngine* m_game;
public:
    Scene(GameEngine* game) : m_game(game) {}
    virtual void update() = 0;
    virtual void render(sf::RenderWindow& window) = 0;
};

#endif
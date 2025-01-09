#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <SFML/Graphics.hpp>
#include <memory>
#include "Scene.h"
#include "Scene_Menu.h"
#include "Scene_Play.h"  //include Scene_Play

class GameEngine {
private:
    sf::RenderWindow m_window;
    std::shared_ptr<Scene> m_activeScene;

public:
    GameEngine();
    void run();
    void changeScene(std::shared_ptr<Scene> newScene);
    sf::RenderWindow* getWindow() { return &m_window; } // Needed for mouse input
};

#endif
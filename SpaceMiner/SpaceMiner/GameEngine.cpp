#include "GameEngine.h"
#include "Scene_Menu.h"
#include <iostream>

GameEngine::GameEngine()
    : m_window(sf::VideoMode(1600, 1200), "Space Miner") {
    std::cout << "Game Engine Initialized!" << std::endl;

    // Start in the menu
    changeScene(std::make_shared<Scene_Menu>(this));
}

void GameEngine::run() {
    while (m_window.isOpen()) {
        sf::Event event;
        while (m_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                m_window.close();
        }

        if (m_activeScene) {
            m_activeScene->update();
        }

        m_window.clear();
        if (m_activeScene) {
            m_activeScene->render(m_window);
        }
        m_window.display();
    }
}

void GameEngine::changeScene(std::shared_ptr<Scene> newScene) {
    m_activeScene = newScene;
}
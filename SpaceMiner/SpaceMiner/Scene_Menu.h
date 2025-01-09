#ifndef SCENE_MENU_H
#define SCENE_MENU_H

#include <SFML/Graphics.hpp>
#include "Scene.h"

class Scene_Menu : public Scene {
private:
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;

    sf::Font font;
    sf::Text startButton;

public:
    Scene_Menu(GameEngine* game);

    void update() override;
    void render(sf::RenderWindow& window) override;
};

#endif
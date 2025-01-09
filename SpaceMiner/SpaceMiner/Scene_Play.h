#ifndef SCENE_PLAY_H
#define SCENE_PLAY_H

#include <SFML/Graphics.hpp>
#include "Scene.h"

class Scene_Play : public Scene {
private:
    sf::Texture playerTexture;
    sf::Sprite playerSprite;

public:
    Scene_Play(GameEngine* game);
    void update() override;
    void render(sf::RenderWindow& window) override;
};

#endif
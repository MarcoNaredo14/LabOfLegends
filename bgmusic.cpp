// bgmusic.hpp
#pragma once
#include <SFML/Audio.hpp>
#include <iostream>
#include <algorithm>

class BackgroundMusic {
private:
    sf::Music music;
    bool isPlaying = false;

public:
    BackgroundMusic() {
        if (!music.openFromFile("items/bgmusic.wav")) {  
            std::cout << "Error loading background music" << std::endl;
        }
        // Set the music to loop continuously
        music.setLoop(true);
        music.setVolume(50.0f);
    }

    void play() {
        if (!isPlaying) {
            music.play();
            isPlaying = true;
        }
    }

    void pause() {
        if (isPlaying) {
            music.pause();
            isPlaying = false;
        }
    }

    void stop() {
        music.stop();
        isPlaying = false;
    }

    void setVolume(float volume) {
        // Clamp volume between 0 and 100
        volume = std::max(0.0f, std::min(100.0f, volume));
        music.setVolume(volume);
    }

    bool getIsPlaying() const {
        return isPlaying;
    }
};

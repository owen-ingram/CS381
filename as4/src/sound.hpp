#ifndef SOUND_HPP
#define SOUND_HPP

#include <raylib-cpp.hpp>

class SoundManager {
public:
    SoundManager();
    void PlayBackgroundMusic();
    raylib::Music& GetMusic();  // Getter for music

private:
    raylib::Music music;
};

#endif // SOUND_HPP

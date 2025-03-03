#include "sound.hpp"

SoundManager::SoundManager() {
    // Load the music directly, no error checks
    music = raylib::Music("../assets/sounds/background_music.ogg");

    // Play the music immediately
    PlayBackgroundMusic();
}

void SoundManager::PlayBackgroundMusic() {
    PlayMusicStream(music);  // Start playing the music
    SetMusicVolume(music, 0.5f);  // Optional: Set volume level
}

// Getter method to access the music object
raylib::Music& SoundManager::GetMusic() {
    return music;
}

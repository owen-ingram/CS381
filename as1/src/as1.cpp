#include <iostream>
#include <raylib-cpp.hpp>
#include "VolumeControl.h"

extern Sound pingSound;

int main() {
    raylib::Window window(800, 400, "CS381 - Assignment 1");
    window.SetState(FLAG_WINDOW_RESIZABLE);

    InitAudioDevice();

    pingSound = LoadSound("../src/ping.wav");
    Music bgMusic = LoadMusicStream("../src/sunflower.mp3");
    Music dialogue = LoadMusicStream("../src/crowd.wav");

    PlayMusicStream(bgMusic);
    PlayMusicStream(dialogue);

    GuiVolumeControlState volumeState = InitGuiVolumeControl();

    bool isDarkMode = true;

    while (!window.ShouldClose()) {
        UpdateMusicStream(bgMusic);
        UpdateMusicStream(dialogue);
        SetMusicVolume(bgMusic, volumeState.MusicSliderValue / 100.0f);
        SetMusicVolume(dialogue, volumeState.DialogueSliderValue / 100.0f);

        window.BeginDrawing();

        if (isDarkMode) {
            window.ClearBackground(DARKGRAY);
        } else {
            window.ClearBackground(RAYWHITE);
        }

        if (GuiButton((Rectangle){ 100, 350, 100, 20 }, isDarkMode ? "Light" : "Dark")) {
            isDarkMode = !isDarkMode;
        }

        GuiVolumeControl(&volumeState);

        window.EndDrawing();
    }

    UnloadSound(pingSound);
    UnloadMusicStream(bgMusic);
    UnloadMusicStream(dialogue);
    CloseAudioDevice();

    return 0;
}

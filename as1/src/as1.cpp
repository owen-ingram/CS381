#include <raylib-cpp.hpp>
#include "VolumeControl.h"

int main() {
    raylib::Window window(800, 400, "CS381 - Assignment 1");
    InitAudioDevice();

    GuiVolumeControlState volumeState = InitGuiVolumeControl();

    Sound sfx = LoadSound("src/ping.wav");
    Sound vocal = LoadSound("src/crowd.wav");
    Music music = LoadMusicStream("src/price-of-freedom.mp3");

    PlaySound(vocal);
    PlayMusicStream(music);

    while (!window.ShouldClose()) {
        UpdateMusicStream(music);
        SetSoundVolume(sfx, volumeState.SFXSliderValue / 100.0f);
        SetSoundVolume(vocal, volumeState.DialogueSliderValue / 100.0f);
        SetMusicVolume(music, volumeState.MusicSliderValue / 100.0f);
        BeginDrawing();
        ClearBackground(RAYWHITE);
        GuiVolumeControl(&volumeState);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), {24, 304, 256, 24})) {
            PingButton();
        }
        EndDrawing();
    }
    UnloadSound(sfx);
    UnloadSound(vocal);
    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();
    
    return 0;
}

void PingButton() {
    static Sound sfx = LoadSound("src/ping.wav");
    PlaySound(sfx);
}

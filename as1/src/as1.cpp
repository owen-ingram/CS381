#include <raylib-cpp.hpp>
#include "VolumeControl.h"

int main() {
    raylib::Window window(800, 400, "CS381 - Assignment 1");
    raylib::AudioDevice audioDevice;
    audioDevice.Init();
    GuiVolumeControlState volumeState = InitGuiVolumeControl();
    raylib::Sound sfx("src/ping.wav");
    raylib::Sound vocal("src/crowd.wav");
    raylib::Music music("src/price-of-freedom.mp3");

    vocal.Play();
    music.Play();

    while (!window.ShouldClose()) {
        UpdateMusicStream(music);
        sfx.SetVolume(volumeState.SFXSliderValue / 100.0f);
        vocal.SetVolume(volumeState.DialogueSliderValue / 100.0f);
        music.SetVolume(volumeState.MusicSliderValue / 100.0f);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        GuiVolumeControl(&volumeState);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), {24, 304, 256, 24})) {
            PingButton();
        }
        EndDrawing();
    }

    sfx.Unload();
    vocal.Unload();
    music.Unload();
    audioDevice.Close();
    CloseWindow();
    return 0;
}

void PingButton() {
    static raylib::Sound sfx("src/ping.wav");
    sfx.Play();
}
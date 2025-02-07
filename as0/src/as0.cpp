#include <iostream>
#include <raylib-cpp.hpp>

int main() {
    raylib::Window window(800, 400, "CS381 - Assignment 0");
    window.SetState(FLAG_WINDOW_RESIZABLE);

    raylib::Text text;
    text.SetText("Hello World");
    text.SetFontSize(30);

    raylib::Vector2 size = text.Measure();


    while (!window.ShouldClose()) {
        float x = (window.GetWidth() - size.x) / 2.0f;
        float y = (window.GetHeight() - size.y) / 2.0f;

        window.BeginDrawing();
            window.ClearBackground(raylib::Color(10, 255, 100));
            text.Draw({x, y});
        window.EndDrawing();
    }
}
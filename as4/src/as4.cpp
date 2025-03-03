#include <iostream>
#include <thread>
#include <chrono>
#include <raylib-cpp.hpp>
#include "skybox.hpp"

template <typename T>
concept Transformer = requires(T t, raylib::Matrix m) {
    { t(m) } -> std::convertible_to<raylib::Matrix>;
};

void DrawBoundedModel(raylib::Model& model, Transformer auto transformer) {
    raylib::Matrix backup = model.transform;
    model.transform = transformer(backup);
    model.Draw({});
    model.transform = backup;
}

void DrawCone(int offset) {
    std::cout << "\033[H";

    for (int i = 0; i < offset; ++i) {
        std::cout << std::endl;
    }
    std::cout << "    \\   /    \n";
    std::cout << "     \\_/     \n";
    std::cout << "     / \\     \n";
    std::cout << "    /   \\    \n";
    std::cout << "   /     \\   \n";
    std::cout << "  /       \\  \n";
    std::cout << " /_________\\ \n";
    std::cout << "     | |     \n";
    std::cout << "     | |     \n";
    std::cout << "    /   \\    \n";
    std::cout << "   /_____|    \n";
    std::cout << "   (___)      \n";
}

int main() {
    std::cout << "Welcome to Cone-nado! Watch out for flying cones! \n\n";
    
    raylib::Window window(800, 600, "CS381 - Conenado");
    window.SetState(FLAG_WINDOW_RESIZABLE);

    raylib::Model model("../assets/Kenny Car Kit/cone.glb");
    model.transform = raylib::Matrix::Identity().Scale(5).RotateY(raylib::Degree(90.0f));

    float targetSpeed = 0;
    float carHeading = 0, carSpeed = 0;
    raylib::Vector3 carPosition = { 0.0f, 0.0f, 0.0f };

    raylib::Model grass = raylib::Mesh::Plane(100, 100, 1, 1).LoadModelFrom();
    raylib::Texture grassTexture = raylib::Texture("../assets/textures/grass.jpg");
    grass.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = grassTexture;

    cs381::SkyBox sky("textures/skybox.png");

    InitAudioDevice();
    Music ambientMusic = LoadMusicStream("../src/background_music.mp3");
    PlayMusicStream(ambientMusic);

    raylib::Camera3D camera(
        raylib::Vector3{ 0.0f, 10.0f, 30.0f },
        carPosition,
        raylib::Vector3{ 0.0f, 1.0f, 0.0f },
        45.0f, CAMERA_PERSPECTIVE
    );

    int offset = 0; 
    while (!window.ShouldClose()) {

        carSpeed = std::lerp(carSpeed, targetSpeed, window.GetFrameTime());
        raylib::Vector3 velocity = {
            cos(raylib::Degree(carHeading)) * carSpeed, 0,
            -sin(raylib::Degree(carHeading)) * carSpeed
        };
        carPosition = carPosition + velocity * window.GetFrameTime();

        camera.target = carPosition;


        DrawCone(offset);
        offset = (offset + 1) % 10;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));


        window.BeginDrawing(); {
            window.ClearBackground(raylib::Color::White());
            camera.BeginMode(); {
                sky.Draw();
                grass.Draw({});
                DrawBoundedModel(model, [&carPosition, &carHeading](raylib::Matrix m) {
                    return m.Translate(carPosition).RotateY(raylib::Degree(carHeading));
                });
            }
            camera.EndMode();

            // Keyboard controls for car movement
            if (raylib::Keyboard::IsKeyPressed(KEY_W)) {
                targetSpeed += 2.0f;
            }
            if (raylib::Keyboard::IsKeyPressed(KEY_S)) {
                targetSpeed -= 2.0f;
            }
            if (raylib::Keyboard::IsKeyPressed(KEY_A)) {
                carHeading += 45.0f;
            }
            if (raylib::Keyboard::IsKeyPressed(KEY_D)) {
                carHeading -= 45.0f;
            }
            if (raylib::Keyboard::IsKeyDown(KEY_Q)) {
                carPosition.y += carSpeed * window.GetFrameTime();
            }
            if (raylib::Keyboard::IsKeyDown(KEY_E)) {
                carPosition.y -= carSpeed * window.GetFrameTime();
            }
            if (raylib::Keyboard::IsKeyPressed(KEY_SPACE)) {
                carSpeed = 0.0f;
                targetSpeed = 0.0f;
            }
        }
        window.EndDrawing();
    }

    UnloadMusicStream(ambientMusic);
    CloseAudioDevice();

    return 0;
}

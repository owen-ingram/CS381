#include <iostream>
#include <raylib-cpp.hpp>
#include "skybox.hpp"

template <typename T>
concept Transformer = requires(T t, raylib::Matrix m) {
    { t(m) } -> std::convertible_to<raylib::Matrix>;
};

void DrawBoundedModel(raylib::Model& model, Transformer auto transformer) {
    raylib::Matrix backup = model.transform;  // Backup the current model transform
    model.transform = transformer(backup);    // Apply the transformation to the model
    model.Draw({});                           // Draw the model
    model.transform = backup;     // Restore the original transformation
}

int main() {
    raylib::Window window(800, 600, "CS381 - Assignment 3");
    window.SetState(FLAG_WINDOW_RESIZABLE);

    raylib::Model model("../assets/Kenny Car Kit/ambulance.glb");
    model.transform = raylib::Matrix::Identity().Scale(5);

    float targetSpeed = 0;
    float carHeading = 0, carSpeed = 0;
    raylib::Vector3 carPosition = { 0.0f, 0.0f, 0.0f };

    raylib::Model grass = raylib::Mesh::Plane(100, 100, 1, 1).LoadModelFrom();
    raylib::Texture grassTexture = raylib::Texture("../assets/textures/grass.jpg");
    grass.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = grassTexture;

    cs381::SkyBox sky("textures/skybox.png");

    raylib::Camera3D camera(raylib::Vector3{ 0.0f, 10.0f, 30.0f }, raylib::Vector3{ 0.0f, 0.0f, 0.0f }, raylib::Vector3{ 0.0f, 1.0f, 0.0f }, 45.0f, CAMERA_PERSPECTIVE);

    while(!window.ShouldClose()) {


        carSpeed = std::lerp(carSpeed, targetSpeed, window.GetFrameTime());
    
        raylib::Vector3 velocity = { cos(raylib::Degree(carHeading)) * carSpeed, 0, -sin(raylib::Degree(carHeading)) * carSpeed };
        carPosition = carPosition + velocity * window.GetFrameTime();

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
            if (raylib::Keyboard::IsKeyDown(KEY_W)) {
                targetSpeed += 0.1f;
            }
            if (raylib::Keyboard::IsKeyDown(KEY_S)) {
                targetSpeed -= 0.1f;  
            }
            if (raylib::Keyboard::IsKeyDown(KEY_A)) {
                carHeading += 0.5f;   
            }
            if (raylib::Keyboard::IsKeyDown(KEY_D)) {
                carHeading -= 0.5f;
            }
            if (raylib::Keyboard::IsKeyPressed(KEY_SPACE)) {
                carSpeed = 0.0f;
                targetSpeed = 0.0f;
            }
        }
        window.EndDrawing();
    }
}

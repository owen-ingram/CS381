#include <iostream>
#include <vector>
#include <raylib-cpp.hpp>
#include "CO.hpp"
#include "skybox.hpp"

int main() {
    raylib::Window window(800, 600, "CS381 - Assignment 6");
    window.SetState(FLAG_WINDOW_RESIZABLE);

    raylib::Model carModel("../assets/Kenny Car Kit/ambulance.glb");
    raylib::Model rocketModel("../assets/Kenny Space Kit/rocketA.glb");

    std::vector<cs381::Entity> entities;

    // 🚗 Create five different cars with unique physics properties
    entities.push_back(cs381::Entity());
    entities.back().AddComponent<cs381::TransformComponent>(raylib::Vector3{0, 0, 0}, 0);
    entities.back().AddComponent<cs381::PhysicsComponent>(10.0f, 5.0f);
    entities.back().AddComponent<cs381::RenderComponent>(&carModel);  // Add RenderComponent
    entities.back().AddComponent<cs381::InputComponent>();

    entities.push_back(cs381::Entity());
    entities.back().AddComponent<cs381::TransformComponent>(raylib::Vector3{5, 0, 0}, 0);
    entities.back().AddComponent<cs381::PhysicsComponent>(12.0f, 4.0f);
    entities.back().AddComponent<cs381::RenderComponent>(&carModel);  // Add RenderComponent
    entities.back().AddComponent<cs381::InputComponent>();

    entities.push_back(cs381::Entity());
    entities.back().AddComponent<cs381::TransformComponent>(raylib::Vector3{-5, 0, 0}, 0);
    entities.back().AddComponent<cs381::PhysicsComponent>(15.0f, 3.0f);
    entities.back().AddComponent<cs381::RenderComponent>(&carModel);  // Add RenderComponent
    entities.back().AddComponent<cs381::InputComponent>();

    entities.push_back(cs381::Entity());
    entities.back().AddComponent<cs381::TransformComponent>(raylib::Vector3{10, 0, 0}, 0);
    entities.back().AddComponent<cs381::PhysicsComponent>(9.0f, 6.0f);
    entities.back().AddComponent<cs381::RenderComponent>(&carModel);  // Add RenderComponent
    entities.back().AddComponent<cs381::InputComponent>();

    entities.push_back(cs381::Entity());
    entities.back().AddComponent<cs381::TransformComponent>(raylib::Vector3{-10, 0, 0}, 0);
    entities.back().AddComponent<cs381::PhysicsComponent>(11.0f, 4.5f);
    entities.back().AddComponent<cs381::RenderComponent>(&carModel);  // Add RenderComponent
    entities.back().AddComponent<cs381::InputComponent>();

    // 🚀 Create a rocket with its own physics properties
    entities.push_back(cs381::Entity());
    entities.back().AddComponent<cs381::TransformComponent>(raylib::Vector3{0, 0, -5}, 90);
    entities.back().AddComponent<cs381::PhysicsComponent>(12.0f, 3.0f);
    entities.back().AddComponent<cs381::RenderComponent>(&rocketModel);  // Add RenderComponent
    entities.back().AddComponent<cs381::InputComponent>();

    cs381::SkyBox sky("textures/skybox.png");
    raylib::Camera3D camera({0.0f, 10.0f, 30.0f}, {0, 0, 0}, {0.0f, 1.0f, 0.0f}, 45.0f, CAMERA_PERSPECTIVE);

    int selectedIndex = 0;

    while (!window.ShouldClose()) {
        float deltaTime = window.GetFrameTime();

        // TAB Key to switch selection
        if (raylib::Keyboard::IsKeyPressed(KEY_TAB)) {
            selectedIndex = (selectedIndex + 1) % entities.size();
        }

        // Update all entities
        for (auto& entity : entities) {
            entity.Update(deltaTime);
        }

        window.BeginDrawing();
        window.ClearBackground(raylib::Color::White());

        camera.BeginMode();
        sky.Draw();

        for (size_t i = 0; i < entities.size(); i++) {
            if (i == selectedIndex) {
                // Ensure that the RenderComponent exists before drawing
                if (auto renderCompOpt = entities[i].GetComponent<cs381::RenderComponent>()) {
                    auto& renderComp = renderCompOpt.value().get();
                    // Draw the bounding box for the selected entity
                    DrawBoundingBox(renderComp.model->GetBoundingBox(), WHITE);
                }
            }
        }

        camera.EndMode();
        window.EndDrawing();
    }

    return 0;
}

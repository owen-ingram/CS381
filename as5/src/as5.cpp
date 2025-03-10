#include <iostream>
#include <vector>
#include <raylib-cpp.hpp>
#include "skybox.hpp"

enum class EntityType { CAR, ROCKET };

struct Entity {
    raylib::Vector3 position;
    float rotation;
    float speed;
    raylib::Model* model;
    EntityType type;
};

std::vector<Entity> entities;
int selectedIndex = 0;

void UpdateEntity(Entity& entity, float deltaTime) {
    raylib::Vector3 velocity = {0, 0, 0};
    float radians = entity.rotation * (PI / 180.0f);

    if (entity.type == EntityType::CAR) {
        velocity.x = cos(radians) * entity.speed;
        velocity.z = -sin(radians) * entity.speed;
    } else {  

        float pitch = entity.rotation * (PI / 180.0f); 
        velocity.x = cos(pitch) * entity.speed;  

        float yaw = entity.rotation * (PI / 180.0f);  
        velocity.z = sin(yaw) * entity.speed;
    }

    entity.position = entity.position + velocity * deltaTime;
}

void DrawEntity(const Entity& entity, bool isSelected) {
    raylib::Matrix transform = raylib::Matrix::Identity()
        .Translate(entity.position);
    
    if (entity.type == EntityType::CAR) {
        transform = transform.RotateY(raylib::Degree(90.0f)); 
        transform = transform.RotateY(raylib::Degree(entity.rotation));
    }
    else if (entity.type == EntityType::ROCKET) {
        transform = transform.RotateX(raylib::Degree(entity.rotation));  
        transform = transform.RotateY(raylib::Degree(entity.rotation));  
        transform = transform.RotateZ(raylib::Degree(entity.rotation));  
    }

    entity.model->transform = transform;
    entity.model->Draw({});

    if (isSelected) {
        DrawBoundingBox(entity.model->GetBoundingBox(), WHITE);
    }
}


int main() {
    raylib::Window window(800, 600, "CS381 - Assignment 5");
    window.SetState(FLAG_WINDOW_RESIZABLE);

    raylib::Model carModel("../assets/Kenny Car Kit/ambulance.glb");
    raylib::Model rocketModel("../assets/Kenny Space Kit/rocketA.glb");

    entities.push_back({{0, 0, 0}, 0, 0, &carModel, EntityType::CAR});
    entities.push_back({{5, 0, 0}, 0, 0, &carModel, EntityType::CAR});
    entities.push_back({{0, 5, 0}, 0, 0, &rocketModel, EntityType::ROCKET});

    cs381::SkyBox sky("textures/skybox.png");
    raylib::Camera3D camera({0.0f, 10.0f, 30.0f}, {0, 0, 0}, {0.0f, 1.0f, 0.0f}, 45.0f, CAMERA_PERSPECTIVE);

    while (!window.ShouldClose()) {
        float deltaTime = window.GetFrameTime();

        if (raylib::Keyboard::IsKeyPressed(KEY_TAB)) {
            selectedIndex = (selectedIndex + 1) % entities.size();
        }

        Entity& selected = entities[selectedIndex];

        if (raylib::Keyboard::IsKeyPressed(KEY_W)) selected.speed += 2.0f;
        if (raylib::Keyboard::IsKeyPressed(KEY_S)) selected.speed -= 2.0f;
        if (raylib::Keyboard::IsKeyPressed(KEY_A)) selected.rotation += 10.0f;  
        if (raylib::Keyboard::IsKeyPressed(KEY_D)) selected.rotation -= 10.0f;  
        if (raylib::Keyboard::IsKeyPressed(KEY_Q)) selected.rotation += 10.0f;  
        if (raylib::Keyboard::IsKeyPressed(KEY_E)) selected.rotation -= 10.0f;  
        if (raylib::Keyboard::IsKeyPressed(KEY_R)) selected.rotation += 10.0f;  
        if (raylib::Keyboard::IsKeyPressed(KEY_F)) selected.rotation -= 10.0f;  

        if (raylib::Keyboard::IsKeyPressed(KEY_SPACE)) selected.speed = 0.0f;

        for (auto& entity : entities) {
            UpdateEntity(entity, deltaTime);
        }

        window.BeginDrawing();
        window.ClearBackground(raylib::Color::White());

        camera.BeginMode();
        sky.Draw();
        for (size_t i = 0; i < entities.size(); i++) {
            DrawEntity(entities[i], i == selectedIndex);
        }
        camera.EndMode();

        window.EndDrawing();
    }
    return 0;
}
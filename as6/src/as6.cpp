#include <iostream>
#include <vector>
#include <raylib-cpp.hpp>
#include "CO.hpp"
#include "skybox.hpp"

enum class EntityType { CAR, ROCKET };

struct Entity {
    raylib::Vector3 position;
    float rotation;
    float speed;
    raylib::Model* model;
    EntityType type;
    cs381::Entity entity;
};

std::vector<Entity> entities;
int selectedIndex = 0;

void UpdateEntity(Entity& entity, float deltaTime) {
    raylib::Vector3 velocity = { 0, 0, 0 };
    float radians = entity.rotation * (PI / 180.0f);

    if (entity.type == EntityType::CAR) {
        velocity.x = cos(radians) * entity.speed;
        velocity.z = -sin(radians) * entity.speed;
    }
    else if (entity.type == EntityType::ROCKET) {
        velocity.x = cos(radians) * entity.speed;
        velocity.y = sin(radians) * entity.speed;
    }

    entity.position = entity.position + velocity * deltaTime;
}

void DrawEntity(const Entity& entity, bool isSelected) {
    raylib::Matrix transform = raylib::Matrix::Identity().Translate(entity.position);

    if (entity.type == EntityType::CAR) {
        transform = transform.RotateY(raylib::Degree(90.0f));
        transform = transform.RotateY(raylib::Degree(entity.rotation));
    }
    else if (entity.type == EntityType::ROCKET) {
        transform = transform.RotateZ(raylib::Degree(270.0f));
        transform = transform.RotateZ(raylib::Degree(entity.rotation));
    }

    entity.model->transform = transform;
    entity.model->Draw({});

    if (isSelected) {
        DrawBoundingBox(entity.model->GetBoundingBox(), WHITE);
    }
}

int main() {
    raylib::Window window(800, 600, "CS381 - Assignment 6");
    window.SetState(FLAG_WINDOW_RESIZABLE);

    raylib::Model ambulanceModel("../assets/Kenny Car Kit/ambulance.glb");
    raylib::Model sedanModel("../assets/Kenny Car Kit/sedan.glb");
    raylib::Model firetruckModel("../assets/Kenny Car Kit/firetruck.glb");
    raylib::Model policeModel("../assets/Kenny Car Kit/police.glb");
    raylib::Model truckModel("../assets/Kenny Car Kit/truck.glb");

    cs381::Entity ambulance;
    ambulance.AddComponent<cs381::TransformComponent>();
    ambulance.AddComponent<cs381::PhysicsComponent>(&ambulanceModel, 10.0f, 2.0f);

    cs381::Entity sedan;
    sedan.AddComponent<cs381::TransformComponent>();
    sedan.AddComponent<cs381::PhysicsComponent>(&sedanModel, 10.0f, 2.0f);

    cs381::Entity firetruck;
    firetruck.AddComponent<cs381::TransformComponent>();
    firetruck.AddComponent<cs381::PhysicsComponent>(&firetruckModel, 10.0f, 2.0f);

    cs381::Entity police;
    police.AddComponent<cs381::TransformComponent>();
    police.AddComponent<cs381::PhysicsComponent>(&policeModel, 10.0f, 2.0f);

    cs381::Entity truck;
    truck.AddComponent<cs381::TransformComponent>();
    truck.AddComponent<cs381::PhysicsComponent>(&truckModel, 10.0f, 2.0f);

    entities.push_back({{0, 0, 0}, 0, 0, &ambulanceModel, EntityType::CAR, ambulance});
    entities.push_back({{5, 0, 0}, 0, 0, &sedanModel, EntityType::CAR, sedan});
    entities.push_back({{10, 0, 0}, 0, 0, &firetruckModel, EntityType::CAR, firetruck});
    entities.push_back({{15, 0, 0}, 0, 0, &policeModel, EntityType::CAR, police});
    entities.push_back({{20, 0, 0}, 0, 0, &truckModel, EntityType::CAR, truck});

    cs381::SkyBox sky("textures/skybox.png");
    raylib::Camera3D camera({ 0.0f, 10.0f, 30.0f }, { 0, 0, 0 }, { 0.0f, 1.0f, 0.0f }, 45.0f, CAMERA_PERSPECTIVE);

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
        if (raylib::Keyboard::IsKeyPressed(KEY_SPACE)) selected.speed = 0.0f;

        for (auto& entity : entities) {
            if (auto* transform = entity.entity.GetComponent<cs381::TransformComponent>()) {
                transform->position = entity.position;
            }

            if (auto* physics = entity.entity.GetComponent<cs381::PhysicsComponent>()) {
                physics->Tick(deltaTime);
            }
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

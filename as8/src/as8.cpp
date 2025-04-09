#include <raylib-cpp.hpp>
#include <vector>
#include <cmath>
#include "skybox.hpp"
#include "ECS.hpp"

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;
constexpr int MAX_ENTITIES = 100;

using EntityID = size_t;

inline EntityID CreateEntity() {
    static EntityID next = 0;
    return next++;
}

struct TransformComponent {
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
};

struct RenderComponent {
    raylib::Model* model;
    bool showBoundingBox;
};

struct VelocityComponent {
    Vector3 velocity;
    float speed;
    float targetSpeed;
    float maxSpeed;
    float acceleration;
};

struct Physics2DComponent {
    float heading;
    float turnRate;
};

struct Physics3DComponent {
    Quaternion rotation;
    Vector3 angular;
};

std::vector<TransformComponent> transformPool(MAX_ENTITIES);
std::vector<RenderComponent> renderPool(MAX_ENTITIES);
std::vector<VelocityComponent> velocityPool(MAX_ENTITIES);
std::vector<Physics2DComponent> physics2DPool(MAX_ENTITIES);
std::vector<Physics3DComponent> physics3DPool(MAX_ENTITIES);
std::vector<bool> hasTransform(MAX_ENTITIES, false);
std::vector<bool> hasRender(MAX_ENTITIES, false);
std::vector<bool> hasVelocity(MAX_ENTITIES, false);
std::vector<bool> hasPhysics2D(MAX_ENTITIES, false);
std::vector<bool> hasPhysics3D(MAX_ENTITIES, false);
std::vector<bool> selectionPool(MAX_ENTITIES, false);

raylib::Model carModels[5];
raylib::Model rocketModel;
raylib::Texture skyTex;
cs381::SkyBox* skybox;

raylib::Camera3D camera;
std::vector<EntityID> entityOrder;
int selectedIndex = 0;

EntityID CreateCar(Vector3 pos, float maxSpeed, float accel, float turnRate, raylib::Model& model) {
    EntityID e = CreateEntity();
    transformPool[e] = { pos, {0, 0, 0}, {1, 1, 1} };
    renderPool[e] = { &model, true };
    velocityPool[e] = { {0, 0, 0}, 0.0f, 0.0f, maxSpeed, accel };
    physics2DPool[e] = { 0.0f, turnRate };
    hasTransform[e] = hasRender[e] = hasVelocity[e] = hasPhysics2D[e] = true;
    entityOrder.push_back(e);
    return e;
}

EntityID CreateRocket(Vector3 pos, raylib::Model& model) {
    EntityID e = CreateEntity();
    transformPool[e] = { pos, {0, 0, 0}, {1, 1, 1} };
    renderPool[e] = { &model, true };
    velocityPool[e] = { {0, 0, 0}, 0.0f, 0.0f, 20.0f, 8.0f };
    physics3DPool[e] = { QuaternionIdentity(), {0, 0, 0} };
    hasTransform[e] = hasRender[e] = hasVelocity[e] = hasPhysics3D[e] = true;
    entityOrder.push_back(e);
    return e;
}

void KinematicsSystem(float dt) {
    for (EntityID e = 0; e < MAX_ENTITIES; ++e) {
        if (hasTransform[e] && hasVelocity[e]) {
            transformPool[e].position = Vector3Add(transformPool[e].position, Vector3Scale(velocityPool[e].velocity, dt));
        }
    }
}

void Physics2DSystem(float dt) {
    for (EntityID e = 0; e < MAX_ENTITIES; ++e) {
        if (hasPhysics2D[e] && hasVelocity[e]) {
            float headingRad = physics2DPool[e].heading * DEG2RAD;
            velocityPool[e].velocity = { cosf(headingRad) * velocityPool[e].speed, 0, -sinf(headingRad) * velocityPool[e].speed };
        }
    }
}

void Physics3DSystem(float dt) {
    for (EntityID e = 0; e < MAX_ENTITIES; ++e) {
        if (hasPhysics3D[e] && hasVelocity[e]) {
            Vector3 dir = Vector3RotateByQuaternion({1, 0, 0}, physics3DPool[e].rotation);
            velocityPool[e].velocity = Vector3Scale(dir, velocityPool[e].speed);
        }
    }
}

void RenderSystem() {
    for (EntityID e = 0; e < MAX_ENTITIES; ++e) {
        if (hasTransform[e] && hasRender[e]) {
            Matrix transform = MatrixTranslate(transformPool[e].position.x, transformPool[e].position.y, transformPool[e].position.z);
            transform = MatrixMultiply(transform, MatrixScale(transformPool[e].scale.x, transformPool[e].scale.y, transformPool[e].scale.z));
            renderPool[e].model->transform = transform;
            renderPool[e].model->Draw({});

            if (selectionPool[e]) {
                DrawBoundingBox(renderPool[e].model->GetBoundingBox(), RED);
            }
        }
    }
}

void InputSystem(float dt) {
    EntityID selected = entityOrder[selectedIndex];
    if (!hasVelocity[selected]) return;

    auto& vel = velocityPool[selected];

    if (IsKeyDown(KEY_W)) vel.speed = std::min(vel.speed + vel.acceleration * dt, vel.maxSpeed);
    if (IsKeyDown(KEY_S)) vel.speed = std::max(vel.speed - vel.acceleration * dt, -vel.maxSpeed);
    if (hasPhysics2D[selected]) {
        auto& phys = physics2DPool[selected];
        if (IsKeyDown(KEY_A)) phys.heading += phys.turnRate * dt;
        if (IsKeyDown(KEY_D)) phys.heading -= phys.turnRate * dt;
    }
    if (hasPhysics3D[selected]) {
        auto& phys = physics3DPool[selected];
        if (IsKeyDown(KEY_A)) phys.angular.y += 1 * dt;
        if (IsKeyDown(KEY_D)) phys.angular.y -= 1 * dt;
        if (IsKeyDown(KEY_R)) phys.angular.x += 1 * dt;
        if (IsKeyDown(KEY_F)) phys.angular.x -= 1 * dt;
        if (IsKeyDown(KEY_Q)) phys.angular.z += 1 * dt;
        if (IsKeyDown(KEY_E)) phys.angular.z -= 1 * dt;
    }
    if (IsKeyPressed(KEY_SPACE)) vel.speed = 0.0f;
}

void SelectionSystem() {
    if (IsKeyPressed(KEY_TAB)) {
        selectionPool[entityOrder[selectedIndex]] = false;
        selectedIndex = (selectedIndex + 1) % entityOrder.size();
        selectionPool[entityOrder[selectedIndex]] = true;
    }
}

int main() {
    raylib::Window window(SCREEN_WIDTH, SCREEN_HEIGHT, "CS381 - Assignment 8");
    SetTargetFPS(60);
    camera = raylib::Camera3D({0.0f, 10.0f, 30.0f}, {0, 0, 0}, {0.0f, 1.0f, 0.0f}, 45.0f, CAMERA_PERSPECTIVE);

    carModels[0] = raylib::Model("../assets/Kenny Car Kit/ambulance.glb");
    carModels[1] = raylib::Model("../assets/Kenny Car Kit/police.glb");
    carModels[2] = raylib::Model("../assets/Kenny Car Kit/taxi.glb");
    carModels[3] = raylib::Model("../assets/Kenny Car Kit/suv.glb");
    carModels[4] = raylib::Model("../assets/Kenny Car Kit/sedan.glb");
    rocketModel = raylib::Model("../assets/Kenny Space Kit/rocketA.glb");

    cs381::SkyBox sky("textures/skybox.png");

    for (int i = 0; i < 5; i++) {
        float speed = 10 + i * 2;
        float accel = 4 + i;
        float turn = 60 - i * 5;
        CreateCar({ (float)i * 4.0f, 0, 0 }, speed, accel, turn, carModels[i]);
    }

    for (int i = 0; i < 5; i++) {
        CreateRocket({ (float)i * 4.0f, 0, -6 }, rocketModel);
    }

    selectionPool[entityOrder[0]] = true;

    while (!window.ShouldClose()) {
        float dt = GetFrameTime();

        SelectionSystem();
        InputSystem(dt);
        Physics2DSystem(dt);
        Physics3DSystem(dt);
        KinematicsSystem(dt);

        window.BeginDrawing();
        window.ClearBackground(RAYWHITE);
        camera.BeginMode();

        sky.Draw();
        RenderSystem();

        camera.EndMode();
        window.EndDrawing();
    }
    return 0;
}
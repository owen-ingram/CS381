#include <raylib-cpp.hpp>
#include <vector>
#include <cmath>
#include <string>
#include <iostream>
#include <thread>
#include <mutex>
#include "skybox.hpp"
#include "ECS.hpp"

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;
constexpr int MAX_ENTITIES = 100;

std::string chatInput;
std::vector<std::string> chatMessages;
bool chatActive = false;
const int maxChatMessages = 5;


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

std::vector<TransformComponent> transformPool(MAX_ENTITIES);
std::vector<RenderComponent> renderPool(MAX_ENTITIES);
std::vector<VelocityComponent> velocityPool(MAX_ENTITIES);
std::vector<Physics2DComponent> physics2DPool(MAX_ENTITIES);
std::vector<bool> hasTransform(MAX_ENTITIES, false);
std::vector<bool> hasRender(MAX_ENTITIES, false);
std::vector<bool> hasVelocity(MAX_ENTITIES, false);
std::vector<bool> hasPhysics2D(MAX_ENTITIES, false);
std::vector<bool> selectionPool(MAX_ENTITIES, false);

std::mutex entityMutex;

raylib::Model carModel;
raylib::Model goalModel;
raylib::Texture skyTex;
cs381::SkyBox* skybox;

raylib::Camera3D camera;
std::vector<EntityID> entityOrder;
int selectedIndex = 0;

int score = 0;
bool alreadyScored = false;
EntityID goalEntity;

EntityID CreateCar(Vector3 pos, float maxSpeed, float accel, float turnRate, raylib::Model& model) {
    EntityID e = CreateEntity();
    transformPool[e] = { pos, {0, 0, 0}, {6, 6, 6} };
    renderPool[e] = { &model, true };
    velocityPool[e] = { {0, 0, 0}, 0.0f, 0.0f, maxSpeed, accel };
    physics2DPool[e] = { 0.0f, turnRate };
    hasTransform[e] = hasRender[e] = hasVelocity[e] = hasPhysics2D[e] = true;
    entityMutex.lock();
    entityOrder.push_back(e);
    entityMutex.unlock();
    return e;
}

EntityID CreateGoal(Vector3 pos, raylib::Model& model) {
    EntityID e = CreateEntity();
    transformPool[e] = { pos, {0, 0, 0}, {2, 2, 2} };
    renderPool[e] = { &model, false };
    hasTransform[e] = hasRender[e] = true;
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
            velocityPool[e].velocity = {
                cosf(headingRad) * velocityPool[e].speed,
                0,
                -sinf(headingRad) * velocityPool[e].speed
            };
        }
    }
}

void RenderSystem() {
    for (EntityID e = 0; e < MAX_ENTITIES; ++e) {
        if (hasTransform[e] && hasRender[e]) {
            Matrix transform = MatrixIdentity();
            transform = MatrixMultiply(transform, MatrixScale(transformPool[e].scale.x, transformPool[e].scale.y, transformPool[e].scale.z));

            if (hasPhysics2D[e]) {
                Matrix rotationMatrix = MatrixRotateY(physics2DPool[e].heading * DEG2RAD);
                transform = MatrixMultiply(transform, rotationMatrix);
            }

            transform = MatrixMultiply(transform, MatrixTranslate(
                transformPool[e].position.x,
                transformPool[e].position.y,
                transformPool[e].position.z
            ));

            renderPool[e].model->transform = transform;
            renderPool[e].model->Draw({});

            if (selectionPool[e]) {
                DrawBoundingBox(renderPool[e].model->GetBoundingBox(), RED);
            }
            if (e == goalEntity) {
                DrawBoundingBox(renderPool[e].model->GetBoundingBox(), GREEN);
            }
        }
    }
}

void InputSystem(float dt) {
    EntityID selected = entityOrder[selectedIndex];
    if (!hasVelocity[selected]) return;

    auto& vel = velocityPool[selected];

    if (hasPhysics2D[selected]) {
        if (IsKeyDown(KEY_W)) vel.speed = std::min(vel.speed + vel.acceleration * dt, vel.maxSpeed);
        if (IsKeyDown(KEY_S)) vel.speed = std::max(vel.speed - vel.acceleration * dt, -vel.maxSpeed);

        auto& phys = physics2DPool[selected];
        if (IsKeyDown(KEY_A)) phys.heading += phys.turnRate * dt;
        if (IsKeyDown(KEY_D)) phys.heading -= phys.turnRate * dt;
    }
}

void SelectionSystem() {
    if (IsKeyPressed(KEY_TAB)) {
        selectionPool[entityOrder[selectedIndex]] = false;
        selectedIndex = (selectedIndex + 1) % entityOrder.size();
        selectionPool[entityOrder[selectedIndex]] = true;
    }
}

void GoalSystem() {
    EntityID car = entityOrder[selectedIndex];
    EntityID goal = goalEntity;

    Vector3 carPos = transformPool[car].position;
    Vector3 goalPos = transformPool[goal].position;

    float dx = carPos.x - goalPos.x;
    float dz = carPos.z - goalPos.z;
    float distance = sqrtf(dx * dx + dz * dz);

    float carRadius = 1.5f;
    float goalRadius = 2.0f;
    float triggerDistance = carRadius + goalRadius;

    DrawText(TextFormat("Distance to Goal: %.2f", distance), 20, 50, 20, GRAY);

    if (distance < triggerDistance) {
        if (!alreadyScored) {
            score++;
            alreadyScored = true;

            float offsetX = GetRandomValue(-30, 30);
            float offsetZ = GetRandomValue(-30, 30);
            transformPool[goal].position = { offsetX, goalPos.y, offsetZ };
        }
    } else {
        alreadyScored = false;
    }
}

void ChatSystem() {
    if (IsKeyPressed(KEY_ENTER)) {
        if (chatActive && !chatInput.empty()) {
            chatMessages.push_back(chatInput);
            if (chatMessages.size() > maxChatMessages) {
                chatMessages.erase(chatMessages.begin());
            }
            chatInput.clear();
        }
        chatActive = !chatActive;
    }

    if (chatActive) {
        int key = GetCharPressed();
        while (key > 0) {
            if (key >= 32 && key <= 125) {
                chatInput += static_cast<char>(key);
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && !chatInput.empty()) {
            chatInput.pop_back();
        }
    }
}

void DrawUI() {
    DrawText(TextFormat("Score: %d", score), 20, 20, 20, DARKGRAY);

    int chatX = 20;
    int chatY = SCREEN_HEIGHT - 150;
    DrawRectangle(chatX - 10, chatY - 10, 400, 130, Fade(LIGHTGRAY, 0.5f));
    DrawRectangleLines(chatX - 10, chatY - 10, 400, 130, DARKGRAY);

    for (int i = 0; i < chatMessages.size(); ++i) {
        DrawText(chatMessages[i].c_str(), chatX, chatY + i * 20, 20, BLACK);
    }

    if (chatActive) {
        DrawText(("> " + chatInput).c_str(), chatX, chatY + chatMessages.size() * 20, 20, DARKBLUE);
    }
}


int main() {
    raylib::Window window(SCREEN_WIDTH, SCREEN_HEIGHT, "Conenado");
    SetTargetFPS(60);
    camera = raylib::Camera3D({ 0.0f, 25.0f, 50.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 75.0f, CAMERA_PERSPECTIVE);

    carModel = raylib::Model("../assets/Kenny Car Kit/cone.glb");
    goalModel = raylib::Mesh::Sphere(1.0f, 16, 16).LoadModelFrom();
    goalModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = GREEN;

    cs381::SkyBox sky("textures/skybox.png");

    InitAudioDevice();
    Music ambientMusic = LoadMusicStream("../src/background_music.mp3");

    raylib::Model grass = raylib::Mesh::Plane(100, 100, 1, 1).LoadModelFrom();
    raylib::Texture grassTexture = raylib::Texture("../assets/textures/grass.jpg");
    grass.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = grassTexture;

    EntityID car = CreateCar({ 0.0f, 0.0f, 0.0f }, 10.0f, 4.0f, 60.0f, carModel);
    selectionPool[car] = true;
    goalEntity = CreateGoal({ 0.0f, 1.0f, -10.0f }, goalModel);
    transformPool[goalEntity].scale = { 2.0f, 2.0f, 2.0f };

    bool gameStarted = false;

    while (!window.ShouldClose()) {
        float dt = GetFrameTime();

        window.BeginDrawing();
        window.ClearBackground(RAYWHITE);

        if (!gameStarted) {
            const char* startText = "START";
            int btnWidth = 200;
            int btnHeight = 60;
            int btnX = SCREEN_WIDTH / 2 - btnWidth / 2;
            int btnY = SCREEN_HEIGHT / 2 - btnHeight / 2;
            Rectangle startButton = { (float)btnX, (float)btnY, (float)btnWidth, (float)btnHeight };

            DrawText("Conenado", SCREEN_WIDTH/2 - MeasureText("Conenado", 50)/2, SCREEN_HEIGHT/2 - 150, 50, DARKGRAY);
            DrawRectangleRec(startButton, LIGHTGRAY);
            DrawRectangleLinesEx(startButton, 2, DARKGRAY);
            DrawText(startText, btnX + btnWidth/2 - MeasureText(startText, 30)/2, btnY + btnHeight/2 - 15, 30, DARKGRAY);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), startButton)) {
                gameStarted = true;
                PlayMusicStream(ambientMusic);
            }
        } else {
            UpdateMusicStream(ambientMusic);

            std::thread inputThread(InputSystem, dt);
            std::thread selectionThread(SelectionSystem);
            std::thread physicsThread(Physics2DSystem, dt);
            std::thread kinematicsThread(KinematicsSystem, dt);
            std::thread goalThread(GoalSystem);

            inputThread.join();
            selectionThread.join();
            physicsThread.join();
            kinematicsThread.join();
            goalThread.join();

            camera.BeginMode();

            sky.Draw();
            grass.Draw({});
            RenderSystem();

            camera.EndMode();
            DrawUI();
            ChatSystem();
        }

        window.EndDrawing();
    }

    UnloadMusicStream(ambientMusic);
    CloseAudioDevice();

    return 0;
}

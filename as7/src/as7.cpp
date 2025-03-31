#include <iostream>
#include <raylib-cpp.hpp>
#include <vector>
#include <chrono>
#include "skybox.hpp"
#include "CO.hpp"

using namespace cs381;

template<typename T> 
concept Transformer = requires(T t, raylib::Matrix m) {
    { t(m) } -> std::convertible_to<raylib::Matrix>;
};

void DrawBoundedModel(raylib::Model& model, Transformer auto transformer) {
    raylib::Matrix backup = model.transform;
    model.transform = transformer(backup);
    model.Draw({});
    model.transform = backup;
}

Entity* gSelectedEntity = nullptr;

struct MeshRenderComponent : public Component {
    raylib::Model* model = nullptr;
    bool drawBoundingBox = false;
    using Component::Component;
    
    void Tick(float dt) override {
        if (model == nullptr) return;
        auto& transform = Object().Transform();
        DrawBoundedModel(*model, [&](raylib::Matrix matrix) {
            return matrix.Translate(transform.position)
                         .RotateY(static_cast<float>(transform.heading) * DEG2RAD);
        });
        if (drawBoundingBox) {
            raylib::BoundingBox box = model->GetBoundingBox();
            box.min = raylib::Vector3(box.min) * 5.0f + transform.position;
            box.max = raylib::Vector3(box.max) * 5.0f + transform.position;
            DrawBoundingBox(box, RED);
        }
    }
};

struct PhysicsComponent : public Component {
    float velocity = 0;
    float acceleration = 10;
    
    PhysicsComponent(Entity& e, float vel, float accel)
        : Component(e), velocity(vel), acceleration(accel) {}
    
    void Tick(float dt) override {
        auto& transform = Object().Transform();
        raylib::Vector3 direction = raylib::Vector3(
            cos(static_cast<float>(transform.heading) * DEG2RAD),
            0,
            sin(static_cast<float>(transform.heading) * DEG2RAD)
        );
        transform.position += direction * velocity * dt;
    }
};

struct InputComponent : public Component {
    PhysicsComponent* physics;
    TransformComponent* transform;
    
    InputComponent(Entity& e, PhysicsComponent* phys, TransformComponent* trans)
        : Component(e), physics(phys), transform(trans) {}
    
    void Tick(float dt) override {
        if (&Object() != gSelectedEntity) return;
        if (IsKeyDown(KEY_W)) physics->velocity += physics->acceleration * dt;
        if (IsKeyDown(KEY_S)) physics->velocity -= physics->acceleration * dt;
        if (IsKeyDown(KEY_A)) transform->heading += 80.0f * dt;
        if (IsKeyDown(KEY_D)) transform->heading -= 80.0f * dt;
        if (IsKeyPressed(KEY_SPACE)) physics->velocity = 0;
    }
};

int main() {
    raylib::Window window(800, 600, "CS381 - Conenado");
    window.SetState(FLAG_WINDOW_RESIZABLE);

    cs381::SkyBox sky("../assets/textures/skybox.png");

    raylib::Model carModel("../assets/Kenny Car Kit/cone.glb");
    carModel.transform = raylib::Matrix::Identity().Scale(5);

    raylib::Model goalModel = raylib::Mesh::Sphere(1.0f, 16, 16).LoadModelFrom();
    goalModel.transform = raylib::Matrix::Identity().Scale(2.0f);

    raylib::Model grass = raylib::Mesh::Plane(100, 100, 1, 1).LoadModelFrom();
    raylib::Texture grassTexture = raylib::Texture("../assets/textures/grass.jpg");
    grass.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = grassTexture;

    std::vector<Entity> entities;
    int score = 0;
    int selectedIndex = 0;

    // Define static goal position
    raylib::Vector3 goalPosition(0.0f, 2.0f, -30.0f);
    float goalRadius = 5.0f;

    // Spawn car
    Entity car;
    auto& transform = car.GetComponent<TransformComponent>().value().get();
    transform.position = raylib::Vector3(5.0f, 0.0f, -5.0f);
    
    car.AddComponent<PhysicsComponent>(0.0f, 20.0f);
    car.AddComponent<MeshRenderComponent>();
    car.GetComponent<MeshRenderComponent>().value().get().model = &carModel;

    auto physicsOpt = car.GetComponent<PhysicsComponent>();
    auto transformOpt = car.GetComponent<TransformComponent>();
    car.AddComponent<InputComponent>(&physicsOpt.value().get(), &transformOpt.value().get());

    entities.push_back(std::move(car));
    gSelectedEntity = &entities[0];


    raylib::Camera3D camera(
        raylib::Vector3(0.0f, 20.0f, 40.0f), // Higher and farther
        raylib::Vector3(0.0f, 0.0f, 0.0f), 
        raylib::Vector3(0.0f, 1.0f, 0.0f), 
        60.0f
    );
        auto lastTime = std::chrono::high_resolution_clock::now();

    while (!window.ShouldClose()) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        float dt = deltaTime.count();

        // Collision detection
        auto& carTransform = entities[0].Transform();
        if (CheckCollisionSpheres(carTransform.position, 2.5f, goalPosition, goalRadius)) {
            score++;
            std::cout << "Goal Reached! Score: " << score << std::endl;
            carTransform.position = raylib::Vector3(5.0f, 0.0f, -5.0f);
        }

        // Entity updates
        for (auto& entity : entities) {
            entity.Tick(dt);
        }

        window.BeginDrawing();
        window.ClearBackground(raylib::Color(0, 0, 0, 255));

        BeginMode3D(camera);
            sky.Draw();
            grass.Draw({});
            goalModel.Draw(goalPosition, 1.0f, WHITE);
            for (auto& entity : entities) {
                if (auto renderCompOpt = entity.GetComponent<MeshRenderComponent>(); renderCompOpt.has_value()) {
                    renderCompOpt->get().Tick(0);
                }
            }
        EndMode3D();

        raylib::DrawText(("Score: " + std::to_string(score)).c_str(), 10, 10, 20, raylib::Color(255, 255, 255, 255));
        window.EndDrawing();
    }
    return 0;
}

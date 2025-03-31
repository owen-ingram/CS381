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

//global pointer to the currently selected entity
Entity* gSelectedEntity = nullptr;

//component to render a model (and optionally a bounding box)
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

//component to update position based on velocity
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

//added input component to control model movement physics
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
    raylib::Window window(800, 600, "CS381 - Assignment 6");
    window.SetState(FLAG_WINDOW_RESIZABLE);

    /*InitAudioDevice();
    Music engineMusic = LoadMusicStream("../assets/audio/engine.mp3");
    PlayMusicStream(engineMusic);*/

    cs381::SkyBox sky("../assets/textures/skybox.png");

    raylib::Model carModel("../assets/Kenny Car Kit/sedan.glb");
    carModel.transform = raylib::Matrix::Identity().Scale(5);

    raylib::Model rocketModel("../assets/Kenny Space Kit/rocketA.glb");
    rocketModel.transform = raylib::Matrix::Identity().Scale(2);

    raylib::Model grass = raylib::Mesh::Plane(100, 100, 1, 1).LoadModelFrom();
    raylib::Texture grassTexture = raylib::Texture("../assets/textures/grass.jpg");
    grass.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = grassTexture;

    std::vector<Entity> entities;

    for (int i = 0; i < 5; i++) {
        Entity car;
        auto& transform = car.GetComponent<TransformComponent>().value().get();
        transform.position = raylib::Vector3(i * 5.0f, 0.0f, i * -5.0f);

        //cars start with 0 velocity.
        car.AddComponent<PhysicsComponent>(0.0f, 20.0f + i * 5);
        car.AddComponent<MeshRenderComponent>();
        car.GetComponent<MeshRenderComponent>().value().get().model = &carModel;

        auto physicsOpt = car.GetComponent<PhysicsComponent>();
        auto transformOpt = car.GetComponent<TransformComponent>();
        car.AddComponent<InputComponent>(&physicsOpt.value().get(), &transformOpt.value().get());

        entities.push_back(std::move(car));
    }

    for (int i = 0; i < 5; i++) {
        Entity rocket;
        auto& transform = rocket.GetComponent<TransformComponent>().value().get();
        //positioned behind cars.
        transform.position = raylib::Vector3(i * 5.0f, 0.0f, i * -5.0f + 3.0f);

        rocket.AddComponent<PhysicsComponent>(0.0f, 10.0f);
        rocket.AddComponent<MeshRenderComponent>();
        rocket.GetComponent<MeshRenderComponent>().value().get().model = &rocketModel;

        auto physicsOpt = rocket.GetComponent<PhysicsComponent>();
        auto transformOpt = rocket.GetComponent<TransformComponent>();
        rocket.AddComponent<InputComponent>(&physicsOpt.value().get(), &transformOpt.value().get());

        entities.push_back(std::move(rocket));
    }

    int selectedIndex = 0;
    gSelectedEntity = &entities[selectedIndex];

    //create a Camera3D
    raylib::Camera3D camera(
         raylib::Vector3(0.0f, 10.0f, 10.0f),  
         raylib::Vector3(0.0f, 0.0f, 0.0f),    
         raylib::Vector3(0.0f, 1.0f, 0.0f), 
         60.0f                                  
    );

    auto lastTime = std::chrono::high_resolution_clock::now();

    while (!window.ShouldClose()) {
        //update music stream so it loops properly EC
        //UpdateMusicStream(engineMusic);

        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        float dt = deltaTime.count();

        //process entity selection
        if (IsKeyPressed(KEY_TAB)) {
            selectedIndex = (selectedIndex + 1) % entities.size();
            gSelectedEntity = &entities[selectedIndex];
        }
        //update bounding box flag based on selection
        for (int i = 0; i < entities.size(); i++) {
            auto renderOpt = entities[i].GetComponent<MeshRenderComponent>();
            if (renderOpt.has_value()) {
                renderOpt->get().drawBoundingBox = (&entities[i] == gSelectedEntity);
            }
        }

        for (auto& entity : entities) {
            for(auto& compPtr : entity.components) {
                if (dynamic_cast<MeshRenderComponent*>(compPtr.get()) == nullptr) {
                    compPtr->Tick(dt);
                }
            }
        }

        for (auto& entity : entities) {
            auto& t = entity.GetComponent<TransformComponent>().value().get();
            if (t.position.x < -50.0f) { t.position.x = -50.0f; }
            if (t.position.x > 50.0f)  { t.position.x = 50.0f;  }
            if (t.position.z < -50.0f) { t.position.z = -50.0f; }
            if (t.position.z > 50.0f)  { t.position.z = 50.0f;  }
        }

        if (gSelectedEntity != nullptr) {
            auto& selTransform = gSelectedEntity->Transform();
            float angle = static_cast<float>(selTransform.heading) * DEG2RAD;
            raylib::Vector3 localOffset(0.0f, 10.0f, -20.0f);
            raylib::Vector3 rotatedOffset(
                localOffset.x * cos(angle) - localOffset.z * sin(angle),
                localOffset.y,
                localOffset.x * sin(angle) + localOffset.z * cos(angle)
            );
            camera.position = selTransform.position + rotatedOffset;
            camera.target = selTransform.position;
        }

        window.BeginDrawing();
        window.ClearBackground(raylib::Color::Black());

        BeginMode3D(camera);
            sky.Draw();
            grass.Draw({});
            for (auto& entity : entities) {
                auto renderOpt = entity.GetComponent<MeshRenderComponent>();
                if (renderOpt.has_value()) {
                    renderOpt->get().Tick(0);
                }
            }
        EndMode3D();

        window.EndDrawing();
    }

    /*UnloadMusicStream(engineMusic);
    CloseAudioDevice();*/

    return 0;
}
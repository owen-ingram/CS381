#pragma once
#include <vector>
#include <memory>  
#include <optional>  
#include <algorithm> 
#include <iostream>  
#include "raylib-cpp.hpp" 

namespace cs381 {

    // Forward declare Entity, to be used later in Component
    struct Entity;

    // Component class definition (base class for all components)
    struct Component {
    protected:
        Entity* owner;  // Points to the Entity that owns this component

    public:
        // Constructor takes an Entity reference and sets the owner pointer
        explicit Component(Entity& e) : owner(&e) {}
        virtual ~Component() = default;  // Destructor
        virtual void Update(float dt) {}  // Virtual function for updating components
    };

    // TransformComponent class, stores position and heading (direction) of an entity
    struct TransformComponent : public Component {
        raylib::Vector3 position;  // Position of the entity in 3D space
        float heading;  // Direction the entity is facing (in degrees)

        // Constructor initializes the position and heading
        TransformComponent(Entity& e, raylib::Vector3 pos = {0, 0, 0}, float head = 0.0f)
            : Component(e), position(pos), heading(head) {}
    };

    // Entity class that owns Components
    struct Entity {
        std::vector<std::unique_ptr<Component>> components;  // List of components

        // Constructor that adds a TransformComponent by default
        Entity() {
            AddComponent<TransformComponent>();  // Ensures TransformComponent is always added
        }

        // Template function to add a component to the entity
        template <typename T, typename... Args>
        void AddComponent(Args&&... args) {
            // Prevent adding multiple TransformComponent instances
            if (std::none_of(components.begin(), components.end(), [](const auto& c) {
                    return dynamic_cast<TransformComponent*>(c.get());
                })) {
                components.push_back(std::make_unique<T>(*this, std::forward<Args>(args)...));
                std::cout << "Adding component: " << typeid(T).name() << std::endl;  // Prints the type of component added
            }
        }

        // Template function to get a component of a specific type
        template <typename T>
        std::optional<std::reference_wrapper<T>> GetComponent() {
            for (auto& component : components) {
                if (auto casted = dynamic_cast<T*>(component.get())) {
                    return *casted;  // Return the component if found
                }
            }
            std::cerr << "Component of type " << typeid(T).name() << " not found!\n";  // Print error if component not found
            std::cerr << "Existing components: ";
            for (const auto& component : components) {
                std::cerr << typeid(*component).name() << " ";  // List all existing component types
            }
            std::cerr << std::endl;
            return std::nullopt;  // Return empty optional if component not found
        }

        // Update function to update all components
        void Update(float dt) {
            for (auto& component : components) {
                component->Update(dt);  // Call update on each component
            }
        }
    };

    // PhysicsComponent definition, handles movement and physics-related behavior
    struct PhysicsComponent : public Component {
        float speed;  // Current speed of the entity
        float acceleration;  // Acceleration rate
        float maxSpeed;  // Maximum speed

        // Constructor to initialize physics properties
        PhysicsComponent(Entity& e, float maxSpd, float accel)
            : Component(e), speed(0.0f), acceleration(accel), maxSpeed(maxSpd) {}

        // Update function to apply physics to the entity
        void Update(float dt) override {
            // Ensure the entity has a TransformComponent before proceeding
            auto transformOpt = owner->GetComponent<TransformComponent>();
            if (!transformOpt) {
                std::cerr << "Error: Entity is missing TransformComponent!\n";  // Print error if missing TransformComponent
                return;
            }
            TransformComponent& transform = transformOpt.value();
            
            // Apply acceleration to speed and clamp within bounds
            speed += acceleration * dt;
            speed = std::clamp(speed, 0.0f, maxSpeed);

            // Move the entity based on heading and speed
            float radians = transform.heading * (PI / 180.0f);  // Convert heading to radians
            transform.position.x += cos(radians) * speed * dt;  // Move along X-axis
            transform.position.z -= sin(radians) * speed * dt;  // Move along Z-axis (note the inversion in raylib)
        }
    };

    // RenderComponent definition, handles drawing the entity's model
    struct RenderComponent : public Component {
        raylib::Model* model;  // Pointer to the model being rendered

        // Constructor to initialize the model
        RenderComponent(Entity& e, raylib::Model* m)
            : Component(e), model(m) {
            std::cout << "RenderComponent added!" << std::endl;  // Print message when added
        }

        // Update function to update the model's transformation and render it
        void Update(float dt) override {
            // Ensure the entity has a TransformComponent before proceeding
            auto transformOpt = owner->GetComponent<TransformComponent>();
            if (!transformOpt.has_value()) {
                std::cerr << "Error: TransformComponent not found!" << std::endl;  // Print error if missing TransformComponent
                return;
            }

            auto& transform = transformOpt.value().get();
            std::cout << "Updating RenderComponent at position: "
                      << transform.position.x << ", "
                      << transform.position.y << ", "
                      << transform.position.z << std::endl;  // Print position

            // Update the model's transformation matrix based on the transform component
            model->transform = raylib::Matrix::Identity()
                .Translate(transform.position)  // Apply translation to position
                .RotateY(raylib::Degree(transform.heading));  // Apply rotation based on heading

            // Draw the model on screen
            model->Draw({});
        }
    };

    // InputComponent definition, handles user input for controlling the entity
    struct InputComponent : public Component {
        float turnSpeed = 90.0f;  // Turning speed in degrees per second
        bool prevW = false, prevS = false, prevA = false, prevD = false;  // Flags for tracking key presses

        // Constructor to initialize the input component
        InputComponent(Entity& e) : Component(e) {}

        // Update function to handle input and update the entity's movement
        void Update(float dt) override {
            // Ensure the entity has both TransformComponent and PhysicsComponent before proceeding
            auto transformOpt = owner->GetComponent<TransformComponent>();
            auto physicsOpt = owner->GetComponent<PhysicsComponent>();

            if (!transformOpt.has_value() || !physicsOpt.has_value()) {
                std::cerr << "Error: Missing TransformComponent or PhysicsComponent!" << std::endl;  // Print error if missing components
                return;
            }

            auto& transform = transformOpt.value().get();
            auto& physics = physicsOpt.value().get();

            // Handle buffered input and update the speed/heading
            if (IsKeyPressed(KEY_W) && !prevW) {
                physics.speed += 2.0f * dt;  // Increase speed when 'W' is pressed
            }
            if (IsKeyPressed(KEY_S) && !prevS) {
                physics.speed -= 2.0f * dt;  // Decrease speed when 'S' is pressed
            }
            if (IsKeyPressed(KEY_A) && !prevA) {
                transform.heading += turnSpeed * dt;  // Turn left when 'A' is pressed
            }
            if (IsKeyPressed(KEY_D) && !prevD) {
                transform.heading -= turnSpeed * dt;  // Turn right when 'D' is pressed
            }

            // Update previous key states for next frame
            prevW = IsKeyDown(KEY_W);
            prevS = IsKeyDown(KEY_S);
            prevA = IsKeyDown(KEY_A);
            prevD = IsKeyDown(KEY_D);

            // Reset speed to 0 when spacebar is pressed
            if (IsKeyPressed(KEY_SPACE)) physics.speed = 0;
        }
    };

} // namespace cs381

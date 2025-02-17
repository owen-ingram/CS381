#include <iostream>
#include <raylib-cpp.hpp>
#include "skybox.hpp"

template <typename T>
concept Transformer = requires(T t, raylib::Matrix m) {
    { t(m) } -> std::convertible_to<raylib::Matrix>;
};

void DrawBoundedModel(raylib::Model& model, Transformer auto transformer) {
    raylib::Matrix backup = model.transform;//backup the current state of the model
    model.transform = transformer(backup);//use the transformer function to apply the transformations
    model.Draw({});//draw the model with the transformations
    model.transform = backup;//return the model to its original transformation
    BoundingBox bounds = model.GetBoundingBox();
    DrawBoundingBox(bounds, WHITE);
}

int main() {
    raylib::Window window(800, 600, "CS381 - Assignment 2");
    window.SetState(FLAG_WINDOW_RESIZABLE);

    auto camera = raylib::Camera({0, 120, 500}, {0, 0, 0}, {0, 1, 0}, 45);

    auto rocket = raylib::Model("meshes/rocket.glb");
    rocket.transform = MatrixScale(30, 30, 30);

    auto car = raylib::Model("meshes/car.glb");
    car.transform = MatrixScale(30, 30, 30);

    cs381::SkyBox sky("textures/skybox.png");

    while(!window.ShouldClose()) {
        window.BeginDrawing();
            camera.BeginMode();
                window.ClearBackground(raylib::Color::Black());
                sky.Draw();

                DrawBoundedModel(rocket, [](raylib::Matrix transform) -> raylib::Matrix {
                    return MatrixMultiply(transform, MatrixIdentity());
                });
                
                DrawBoundedModel(rocket, [](raylib::Matrix transform) -> raylib::Matrix {
                    return MatrixMultiply(MatrixMultiply(MatrixMultiply(MatrixScale(30, 30, 30), MatrixTranslate(-100, 100, 0)), MatrixScale(1, -1, 1)), MatrixRotateY(180));
                });
                
                DrawBoundedModel(car, [](raylib::Matrix transform) -> raylib::Matrix {
                    return MatrixMultiply(MatrixScale(30, 30, 30), MatrixTranslate(-200, 0, 0));
                });
                
                DrawBoundedModel(car, [](raylib::Matrix transform) -> raylib::Matrix {
                    return MatrixMultiply(MatrixMultiply(MatrixScale(30, 30, 30), MatrixTranslate(200, 0, 0)), MatrixRotateY(90));
                });
                
                DrawBoundedModel(car, [](raylib::Matrix transform) -> raylib::Matrix {
                    return MatrixMultiply(MatrixMultiply(MatrixMultiply(MatrixScale(30, 30, 30), MatrixTranslate(100, 100, 0)), MatrixScale(1, 2, 1)), MatrixRotateY(270));
                });
                

            camera.EndMode();
        window.EndDrawing();
    }
}

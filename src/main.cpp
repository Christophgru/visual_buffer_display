#include "SDL3/SDL.h"
#include "renderer/renderer.h"
#include "shapes/shape.h"
#include "shapes/rectangle.h"
#include "shapes/circle.h"
#include "shapes/triangle.h"
#include <exception>
#include <iostream>
#include <memory>
#include "physics_engine/physics_engine.h"

int main(float argc, char* argv[]) {
    try {
    int WIDTH = 800;
    int HEIGHT = 600;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL could not initialize! SDL_Error: %s", SDL_GetError());
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Pixel Buffer Renderer", WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE);
    if (!window) {
        SDL_Log("Window could not be created! SDL_Error: %s", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    // Create shapes
    auto shapes = std::make_shared<std::vector<Shape*>>(std::initializer_list<Shape*>{
        new Rectangle({100, 100,0},{0,0,0},{1,1,1}, 50, 50, 255, 0, 0),  // Red rectangle
        new Circle({400, 300,0},{0,0,0},{1,1,1}, 50, 0, 255, 0),         // Green circle
        new Triangle({600, 400,0},{0,0,0},{1,1,1}, 60, 0, 0, 255)       // Blue triangle
    });

    Renderer renderer(window, WIDTH, HEIGHT,shapes);
    bool running = true;
    SDL_Event event;

    uint32_t lastTime = SDL_GetTicks();
    uint32_t lastMoveTime = SDL_GetTicks();
    float frameCount = 0;
    PhysicsEngine physicsEngine(shapes,renderer);
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            } else if (event.type == SDL_EVENT_WINDOW_RESIZED) {
                WIDTH = event.window.data1;
                HEIGHT = event.window.data2;
                renderer.resize(WIDTH, HEIGHT);
            }
        }
        renderer.clearBuffer(0x000000FF); // Black background

        uint32_t currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastMoveTime) / 1000.0f; // Time in seconds
        lastMoveTime = currentTime;
        physicsEngine.update();
        // Move and draw shapes
        renderer.render();

        frameCount++;
        if (currentTime - lastTime >= 1000) {
            float fps = frameCount / ((currentTime - lastTime) / 1000.0f);
            SDL_Log("FPS: %.2f", fps);
            frameCount = 0;
            lastTime = currentTime;
        }
    }

    SDL_Quit();
    return 0;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "Unknown exception occurred!" << std::endl;
        return -1;
    }
}

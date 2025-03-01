#include "SDL3/SDL.h"
#include "renderer/renderer.h"
#include "shapes/shape.h"
#include "shapes/rectangle.h"
#include "shapes/circle.h"
#include "shapes/triangle.h"


int main(int argc, char* argv[]) {
    const int WIDTH = 800;
    const int HEIGHT = 600;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL could not initialize! SDL_Error: %s", SDL_GetError());
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Pixel Buffer Renderer", WIDTH, HEIGHT, 0);
    if (!window) {
        SDL_Log("Window could not be created! SDL_Error: %s", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    Renderer renderer(window, WIDTH, HEIGHT);

        // Create shapes
        std::vector<Shape*> shapes;
        shapes.push_back(new Rectangle({100, 100,0}, 80, 50, 255, 0, 0));  // Red rectangle
        shapes.push_back(new Circle({400, 300,0}, 50, 0, 255, 0));         // Green circle
        shapes.push_back(new Triangle({600, 400,0}, 60, 0, 0, 255));       // Blue triangle

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }
        renderer.clearBuffer(0x000000FF); // Black background

 // Move and draw shapes
 for (auto& shape : shapes) {
    shape->move(1, 0,0); // Move shapes to the right
    shape->draw(renderer);
 for (auto &&shape : shapes)
 {
    auto pos=shape->get_coords();
    if(pos.at(0)>WIDTH){
        shape->move_to(0,pos.at(1),pos.at(2));
    }
 }
 
}
        renderer.render();
        SDL_Delay(16); // ~60 FPS
    }

    SDL_Quit();
    return 0;
}

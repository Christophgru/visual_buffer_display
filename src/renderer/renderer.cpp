#include "Renderer.h"
#include <cmath>

// Constructor: Initializes SDL Renderer
Renderer::Renderer(SDL_Window* window) {
    renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        SDL_Log("Renderer could not be created! SDL_Error: %s", SDL_GetError());
    }
    r = 0; g = 0; b = 255; // Start with blue
}

// Destructor: Cleans up SDL Renderer
Renderer::~Renderer() {
    SDL_DestroyRenderer(renderer);
}

// Updates color values over time using a sine wave function for smooth transitions
void Renderer::updateColor() {
    float time = SDL_GetTicks() * 0.001f; // Convert milliseconds to seconds
    r = static_cast<int>(std::sin(time) * 127 + 128); // Red oscillates
    g = static_cast<int>(std::cos(time) * 127 + 128); // Green oscillates
    b = static_cast<int>(std::sin(time + 3.14f) * 127 + 128); // Blue opposite phase
}

// Renders the background color
void Renderer::render() {
    updateColor();
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

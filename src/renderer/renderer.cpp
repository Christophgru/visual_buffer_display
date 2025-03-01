#include "Renderer.h"
#include <cstring> // For memset

// Constructor: Initializes SDL Renderer and Texture
Renderer::Renderer(SDL_Window* window, int width, int height)
    : width(width), height(height), buffer(width * height, 0x000000FF) { // Default black
    renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        SDL_Log("Renderer could not be created! SDL_Error: %s", SDL_GetError());
    }

    // Create a texture to store the pixel data
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
}

// Destructor: Cleans up SDL resources
Renderer::~Renderer() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
}

// Set an individual pixel in the buffer
void Renderer::setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    if (x < 0 || x >= width || y < 0 || y >= height) return; // Out-of-bounds check
    buffer[y * width + x] = (r << 24) | (g << 16) | (b << 8) | 0xFF; // RGBA format
}

// Clear the buffer to a specific color
void Renderer::clearBuffer(uint32_t color) {
    std::fill(buffer.begin(), buffer.end(), color);
}

// Render the buffer to the screen
void Renderer::render() {
    SDL_UpdateTexture(texture, nullptr, buffer.data(), width * sizeof(uint32_t));

    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, nullptr, nullptr); // SDL3 function replaces SDL_RenderCopy

    SDL_RenderPresent(renderer);
}

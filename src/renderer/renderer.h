#ifndef RENDERER_H
#define RENDERER_H

#include "SDL3/SDL.h"
#include <vector>

class Renderer {
public:
    Renderer(SDL_Window* window, int width, int height);
    ~Renderer();

    void setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b); // Set individual pixel
    void clearBuffer(uint32_t color = 0x000000FF); // Clear buffer (default black)
    void render(); // Render the buffer

private:
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    std::vector<uint32_t> buffer; // Stores the RGB buffer
    int width, height;
};

#endif // RENDERER_H

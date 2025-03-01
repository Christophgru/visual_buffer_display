#ifndef RENDERER_H
#define RENDERER_H

#include "SDL3/SDL.h"

class Renderer {
public:
    Renderer(SDL_Window* window);
    ~Renderer();

    void updateColor();
    void render();

private:
    SDL_Renderer* renderer;
    int r, g, b;
};

#endif // RENDERER_H

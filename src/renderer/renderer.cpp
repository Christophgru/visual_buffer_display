#include "Renderer.h"
#include <cstring> // For memset
#include <stdio.h>
#include <iostream>
#include <array>

// Constructor: Initializes SDL Renderer and Texture
Renderer::Renderer(SDL_Window* window, int width, int height, std::shared_ptr<std::vector<Shape*>> shapes)
    : width(width), height(height), buffer(width * height, 0x000000FF) { // Default black
    renderer = SDL_CreateRenderer(window, NULL);
    this->shapes = shapes;
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
    int xpos=shapes->at(0)->get_coords().at(0);
    int ypos=shapes->at(0)->get_coords().at(1);
    int zpos=shapes->at(0)->get_coords().at(2);
    for (Shape* shape : *shapes) {
        std::array<uint8_t,3> colors = shape->get_color();
        uint8_t r = colors[0];
        uint8_t g = colors[1]; 
        uint8_t b = colors[2];
        if (shape->get_shape_type()==RECTANGLE) {
            auto rect = static_cast<Rectangle*>(shape);
                auto pos = rect->get_coords();
                auto width = rect->get_width();
                auto height = rect->get_height();
                for (float i = pos[0]; i < pos[0] + width; ++i) {
                    for (float j = pos[1]; j < pos[1] + height; ++j) {
                        setPixel(i, j, r, g, b);
                    }
                }
            }else if (auto circle = static_cast<Circle*>(shape)) {
                auto pos = circle->get_coords();
                auto radius = circle->get_radius();
                for (float i = pos[0] - radius; i < pos[0] + radius; ++i) {
                    for (float j = pos[1] - radius; j < pos[1] + radius; ++j) {
                        if (std::sqrt((i - pos[0]) * (i - pos[0]) + (j - pos[1]) * (j - pos[1])) <= radius) {
                            setPixel(i, j, r, g, b);
                        }
                    }
                }
            }
            else if (auto triangle = static_cast<Triangle*>(shape)) {
                auto pos = triangle->get_coords();
                auto size = triangle->get_size();
                for (float i = pos[0]; i < pos[0] + size; ++i) {
                    for (float j = pos[1]; j < pos[1] + size; ++j) {
                        if (i <= pos[0] + size && j <= pos[1] + size && j >= pos[1] + size - i) {
                            setPixel(i, j, r, g, b);
                        }
                    }
                }
            
        }
    }
    SDL_UpdateTexture(texture, nullptr, buffer.data(), width * sizeof(uint32_t));

    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, nullptr, nullptr); // SDL3 function replaces SDL_RenderCopy

    SDL_RenderPresent(renderer);
}

// Handle window resizing
void Renderer::resize(int newWidth, int newHeight) {
    width = newWidth;
    height = newHeight;
    buffer.resize(width * height, 0x000000FF); // Default black
    SDL_DestroyTexture(texture);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
}

int Renderer::getWindowWidth() {
    return width;
}
int Renderer::getWindowHeight() {
    return height;
}

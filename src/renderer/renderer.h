#ifndef RENDERER_H
#define RENDERER_H

#include "SDL3/SDL.h"
#include <vector>
#include <memory>
#include "../shapes/shape.h"
#include "../shapes/rectangle.h"
#include "../shapes/circle.h"
#include "../shapes/triangle.h"
#include "../shapes/vertex.h"
#include "../camera/camera.h"

class Renderer {
public:
    Renderer(SDL_Window* window, int width, int height,std::shared_ptr<std::vector<Shape *>> shapes, 
        std::shared_ptr<Camera> camera,std::shared_ptr<std::vector<std::array<int,3>>> index_buffer);
    ~Renderer();

    void setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b); // Set individual pixel
    void clearBuffer(uint32_t color = 0x000000FF); // Clear buffer (default black)
    void render(); // Render the buffer
    void resize(int newWidth, int newHeight);
    int getWindowWidth() ;
    int getWindowHeight() ;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    std::vector<uint32_t> buffer; // Stores the RGB buffer
    int width, height;
    
private:
void drawTriangle(const std::array<float, 2>& v0,
    const std::array<float, 2>& v1,
    const std::array<float, 2>& v2);
    void fillGradient();
    std::array<float,2> project(std::vector<float> pos,std::vector<float> camera_orientation,std::vector<float> camera_pos);
    std::shared_ptr<std::vector<Shape *>> shapes;
    std::shared_ptr<std::vector<std::array<int,3>>> index_buffer;
    std::shared_ptr<Camera> camera;
};

#endif // RENDERER_H

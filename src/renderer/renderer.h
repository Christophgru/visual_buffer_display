#ifndef RENDERER_H
#define RENDERER_H
#define GLEW_STATIC

#include <windows.h>
#include <GL/glew.h>  // Must be included before any other GL headers.
#include "SDL3/SDL.h"
#include <vector>
#include <memory>
#include <array>
#include "../shapes/shape.h"
#include "../shapes/rectangle.h"
#include "../shapes/circle.h"
#include "../shapes/triangle.h"
#include "../shapes/vertex.h"
#include "../camera/camera.h"

class Renderer {
public:
    Renderer(SDL_Window* window, int width, int height, 
             std::shared_ptr<std::vector<Shape*>> shapes, 
             std::shared_ptr<Camera> camera,
             std::shared_ptr<std::vector<std::array<int, 3>>> index_buffer);
    ~Renderer();

    
    // Render function that now uses OpenGL for accelerated rendering
    void render();
    void resize(int newWidth, int newHeight);
    int getWindowWidth();
    int getWindowHeight();

    // Legacy SDL renderer and texture (if you still need them)
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    std::vector<uint32_t> buffer;
    int width, height;
    
private:
    std::array<float, 2> project(std::vector<float> pos,
                                 std::vector<float> camera_orientation,
                                 std::vector<float> camera_pos);
    
    // Data shared with the rest of your project
    std::shared_ptr<std::vector<Shape*>> shapes;
    std::shared_ptr<std::vector<std::array<int, 3>>> index_buffer;
    std::shared_ptr<Camera> camera;

    // OpenGL-specific members for hardware-accelerated rendering
    GLuint shaderProgram = 0;
    GLuint VAO = 0;
    GLuint VBO = 0;

    // Helper to compile and link shaders
    GLuint createShaderProgram(const char* vertexSource, const char* fragmentSource);
};

#endif // RENDERER_H


#define GLEW_STATIC

#include <windows.h>
#include <GL/glew.h>  // Must be included before any other GL headers.
#include "SDL3/SDL.h"
#include <vector>
#include <memory>
#include <array>
#include "shapes/object.h"
#include "shapes/rectangle.h"
#include "shapes/circle.h"
#include "shapes/triangle.h"
#include "shapes/vertex.h"
#include "camera/camera.h"
#include <unordered_map>
#include "scene/scene.h"
using ObjSP   = std::shared_ptr<Object>;
using ObjVec  = std::vector<ObjSP>;
using ObjVecP = std::shared_ptr<ObjVec>;
using IdMap   = std::unordered_map<int, std::weak_ptr<Object>>; // non-owning
#ifndef RENDERER_H
#define RENDERER_H

class SimpleRenderer {
public:
    SimpleRenderer(SDL_Window* window, int width, int height, 
             std::shared_ptr<Scene> scene);
    ~SimpleRenderer();

    
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
    std::shared_ptr<Scene> scene;
    
private:
bool is_point_in_frame(const std::vector<float> point, const std::vector<float> camera_pos, const std::vector<float> camera_orientation);
void hand_data_to_shader(std::vector<float> triangleData,    std::vector<float> pointData);
void hand_data_to_shader(std::vector<float> triangleData,
                                   std::vector<float> pointData,
                                   const IdMap& idMap);
    std::array<float, 2> project(std::vector<float> pos,
                                 std::vector<float> camera_orientation,
                                 std::vector<float> camera_pos);
    

    // OpenGL-specific members for hardware-accelerated rendering
    GLuint shaderProgram = 0;
    GLuint VAO = 0;
    GLuint VBO = 0;

    // Helper to compile and link shaders
    GLuint createShaderProgram(const char* vertexSource, const char* fragmentSource);
};

#endif // RENDERER_H
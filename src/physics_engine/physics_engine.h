#ifndef PHYSICS_ENGINE_H
#define PHYSICS_ENGINE_H
#include <vector>
#include <memory>
#include "../shapes/shape.h"
#include "../renderer/renderer.h"
enum detected_actions{
    terminate,
    window_resize,
    no_action
};

class PhysicsEngine {
private:
    std::shared_ptr<std::vector<Shape*>> shapes;
    Renderer& renderer;
    Uint64 lastMoveTime=SDL_GetTicks();
    std::shared_ptr<Camera> camera;
    bool mouse_clicked=false;
    std::tuple<float,float> mouse_movement={0.0,0.0};
    std::tuple<int,int> display_dimensions;
public:
    PhysicsEngine(std::shared_ptr<std::vector<Shape*>> shapes, Renderer& renderer, std::shared_ptr<Camera> camera, std::tuple<int,int> display_dimensions) : display_dimensions(display_dimensions), camera(camera), shapes(shapes), renderer(renderer) {
        // Initialize the physics engine
    }
    ~PhysicsEngine() {
        // Clean up the physics engine
    }
    void update();
    std::tuple<detected_actions,std::vector<int>> handleEvent(SDL_Event event);
};

#endif // PHYSICS_ENGINE_H
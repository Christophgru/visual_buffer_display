#ifndef PHYSICS_ENGINE_H
#define PHYSICS_ENGINE_H
#include <vector>
#include <memory>
#include "../shapes/object.h"
#include "../renderer/renderer.h"

enum detected_actions{
    terminate,
    window_resize,
    no_action
};

class PhysicsEngine {
private:
    std::shared_ptr<std::vector<std::shared_ptr<Object>>> shapes;
    Renderer& renderer;
    Uint64 lastMoveTime=SDL_GetTicks();
    std::shared_ptr<Camera> camera;
    bool mouse_clicked=false;
    std::tuple<float,float> mouse_movement={0.0,0.0};
    std::tuple<int,int> display_dimensions;
    std::vector<float> calculate_new_position(std::vector<float> pos, std::vector<float> orientation, std::vector<float> direction, float speed) ;
public:
    PhysicsEngine(std::shared_ptr<std::vector<std::shared_ptr<Object>>> shapes, Renderer& renderer, std::shared_ptr<Camera> camera, std::tuple<int,int> display_dimensions) : display_dimensions(display_dimensions), camera(camera), shapes(shapes), renderer(renderer) {
        // Initialize the physics engine
    }
    ~PhysicsEngine() {
        // Clean up the physics engine
    }
    void update();
    std::tuple<detected_actions,std::vector<int>> handleEvent(SDL_Event event);
};



#endif // PHYSICS_ENGINE_H
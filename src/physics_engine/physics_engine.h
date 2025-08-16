#ifndef PHYSICS_ENGINE_H
#define PHYSICS_ENGINE_H
#include <vector>
#include <memory>
#include "shapes/object.h"
#include "renderer/renderer.h"

enum detected_actions{
    terminate,
    window_resize,
    no_action
};

class PhysicsEngine {
private:
    std::shared_ptr<std::vector<std::shared_ptr<Object>>> shapes;
    Uint64 lastMoveTime=SDL_GetTicks();
    bool mouse_clicked=false;
    std::shared_ptr<SimpleRenderer> renderer;
    std::shared_ptr<Scene> scene;
    std::tuple<float,float> mouse_movement={0.0,0.0};
    std::vector<float> calculate_new_position(std::vector<float> pos, std::vector<float> orientation, std::vector<float> direction, float speed) ;
public:
    PhysicsEngine( std::shared_ptr<SimpleRenderer> renderer_passed, std::shared_ptr<Scene> scene_passed) : renderer(renderer_passed), scene(scene_passed) {
        // Initialize the physics engine
    }
    ~PhysicsEngine() {
        // Clean up the physics engine
    }
    void update();
    std::tuple<detected_actions,std::vector<int>> handleEvent(SDL_Event event);
};



#endif // PHYSICS_ENGINE_H
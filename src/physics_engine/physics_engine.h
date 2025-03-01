#ifndef PHYSICS_ENGINE_H
#define PHYSICS_ENGINE_H
#include <vector>
#include <memory>
#include "../shapes/shape.h"
#include "../renderer/renderer.h"

class PhysicsEngine {
private:
    std::shared_ptr<std::vector<Shape*>> shapes;
    Renderer& renderer;
    Uint64 lastMoveTime=SDL_GetTicks();
public:
    PhysicsEngine(std::shared_ptr<std::vector<Shape*>> shapes, Renderer& renderer) : shapes(shapes), renderer(renderer) {
        // Initialize the physics engine
    }
    ~PhysicsEngine() {
        // Clean up the physics engine
    }
    void update();
};

#endif // PHYSICS_ENGINE_H
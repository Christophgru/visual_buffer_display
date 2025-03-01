#include "physics_engine.h"

void PhysicsEngine::update() {
    
    
        auto time= SDL_GetTicks();
        auto deltaTime = (time - lastMoveTime) / 1000.0f; // Time in seconds
        lastMoveTime = time;
        for (auto shape : *shapes) {
            shape->move(deltaTime*20, deltaTime*20,0); // Move shapes to the right
            auto pos = shape->get_coords();
            if (pos.at(0) > renderer.getWindowWidth()) {
                shape->move_to(0, pos.at(1), pos.at(2));
            }
            if (pos.at(1) > renderer.getWindowHeight()) {
                shape->move_to(pos.at(0), 0, pos.at(2));
            }
            
        }
        
    }

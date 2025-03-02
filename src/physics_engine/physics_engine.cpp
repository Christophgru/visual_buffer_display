#include "physics_engine.h"
#include <iostream>

void PhysicsEngine::update() {
    try
    {
        auto time= SDL_GetTicks();
        auto deltaTime = (time - lastMoveTime) / 1000.0f; // Time in seconds
        lastMoveTime = time;
        for (auto shape : *shapes) {
            if(shape->get_shape_type()==VERTEX){
                shape->move(0, -5*deltaTime,0);
            }else{
                shape->move(5*deltaTime, 5*deltaTime,0);
            };
            auto pos = shape->get_coords();
            if (pos.at(0) > renderer.getWindowWidth()) {
                shape->move_to(0, pos.at(1), pos.at(2));
            }
            if (pos.at(1) > renderer.getWindowHeight()) {
                shape->move_to(pos.at(0), 0, pos.at(2));
            }
            
        }
        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    
    }

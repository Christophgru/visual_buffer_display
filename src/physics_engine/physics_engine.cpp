#include "physics_engine.h"
#include <iostream>

void PhysicsEngine::update() {
    try
    {
        auto time= SDL_GetTicks();
        auto deltaTime = (time - lastMoveTime) / 1000.0f; // Time in seconds
        lastMoveTime = time;
        camera->orientation={camera->orientation.at(0),camera->orientation.at(1),camera->orientation.at(2)};
        for (auto shape : *shapes) {
            if(shape->get_shape_type()==VERTEX){
                shape->move(0, -5*deltaTime,0);
                if(shape->get_coords().at(1)<0){
                    shape->move_to(shape->get_coords().at(0), 100.0f,shape->get_coords().at(2));
                }
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

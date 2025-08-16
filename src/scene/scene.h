#include <vector>
#include <memory>
#include "shapes/object.h"
#include "camera/camera.h"
#include "physics_engine/physics_engine.h"

#include <memory>
#include <vector>
#include <array>
#include "shapes/object.h"
#include <camera/camera.h>
#include "shapes/triangle.h"
#include "shapes/vertex.h"
#include "shapes/circle.h"
#include "shapes/rectangle.h"
#include <filesystem>
#include "object_loader/object_loader.h"
#ifndef SCENE_H
#define SCENE_H

class Scene
{
private:
    friend class PhysicsEngine; // Allow SimpleRenderer to access private members
    std::shared_ptr<std::vector<std::shared_ptr<Object>>> objects;
    std::shared_ptr<std::vector<std::array<int,3>>> index_buffer;
    //camera
    std::shared_ptr<Camera> camera;
    void set_camera_position(std::vector<float> pos, std::vector<float> orientation={}) ;

public:
    Scene(/* args */);
    ~Scene();
    void populate_scene(std::shared_ptr<std::vector<std::shared_ptr<Object>>> objects,std::shared_ptr<std::vector<std::array<int,3>>> index_buffer);
    void add_object(std::string filename_obj = "external/newell_teaset/spoon.obj", 
                    std::string filename_mtl = "external/newell_teaset/spoon.mtl");
    std::shared_ptr<std::vector<std::shared_ptr<Object>>> get_objects() ;
    std::shared_ptr<std::vector<std::array<int,3>>> get_index_buffer() ;
    std::shared_ptr<Camera> get_camera() ;
};

#endif // SCENE_H
#include "scene.h"
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
#include <filesystem>
#include <fstream>

void Scene::set_camera_position(std::vector<float> pos, std::vector<float> orientation) {
        camera->pos = pos;
        if(!orientation.empty())
            camera->orientation = orientation;
    }

    std::shared_ptr<std::vector<std::shared_ptr<Object>>> Scene::get_objects() { return objects; }
    std::shared_ptr<std::vector<std::array<int,3>>> Scene::get_index_buffer() { return index_buffer; }
    std::shared_ptr<Camera> Scene::get_camera() { return camera; }


    
Scene::Scene(/* args */)
{
    objects = std::make_shared<std::vector<std::shared_ptr<Object>>>();
    index_buffer = std::make_shared<std::vector<std::array<int,3>>>();
    // Populate the scene with objects and indices
    populate_scene(objects, index_buffer);
    // Create shapes (your current objects)
    camera = std::make_shared<Camera>(std::vector<float>{0, 0, 0},
                                               std::vector<float>{0, 100, 0},
                                                40);
}

Scene::~Scene()
{
}


void Scene::populate_scene(std::shared_ptr<std::vector<std::shared_ptr<Object>>> objects,std::shared_ptr<std::vector<std::array<int,3>>> index_buffer )
{

         // Green circle
        objects->push_back(std::shared_ptr<Object>(new Circle({400, 300,0},{0,0,0},{1,1,1}, 50, 0, 255, 0)));
        // Blue triangle
        //objects->push_back(std::shared_ptr<Object>(new Triangle({100, 50,0},{0,0,0},{1,1,1}, 60, 0, 0, 255)));
        // Red rectangle
        objects->push_back(std::shared_ptr<Object>(new Rect({100, 100,0},{0,0,0},{1,1,1}, 50, 50, 255, 0, 0))); // Red Rect
        // Add a vertex that comes from straight ahead
        for (float i = -5; i <= 5; i++) {
            for (float j = -5; j <= 5; j++) {
                objects->push_back(std::make_shared<Vertex>(std::vector<float>{i, 80, j}, std::vector<float>{0,0,0}, std::vector<float>{1,1,1},
                                               (int)(255 - i) % 255,
                                               (int)(255 + j) % 255,
                                               (int)(255 + i - j) % 255,"moving_over")); // Yellow vertex
            }
        }//*/
        // Add a vertex that tiles the floor
      std::shared_ptr<Object> floor = std::make_shared<Object>(Object({0, 0, -2}, {0, 0, 0}, {1, 1, 1}, 255, 255, 0, "floor"));
        objects->push_back(floor);
        for (float i = -10; i <= 10; i+=0.1) {            
            for (float j = -10; j <= 10; j+=0.1) {
                 std::shared_ptr<Object> vx = std::make_shared<Vertex>(std::vector<float>{i, j, -2}, std::vector<float>{0,0,0}, std::vector<float>{1,1,1},
                                               (int)(255 - i) % 255,
                                               (int)(255 + j) % 255,
                                               (int)(255 + i - j) % 255,"floor"); // Yellow vertex
                floor->add_child(vx);
            }
        }
        std::cout << "Floor initialized" << std::endl;

        
        index_buffer.get()->push_back(std::array<int,3>{{3, 10, 87}});//*/

        add_object("external/newell_teaset/spoon.obj", "external/newell_teaset/spoon.mtl");

}
void Scene::add_object(std::string filename_obj, std::string filename_mtl) {
    // Load the OBJ and MTL files
        std::cout << "Loading OBJ and MTL files..." << std::endl;
        std::string objText;
        std::string mtlText;
            try
        {
            //load string from file
            //create absolute path
            std::string absolute_path_spoon = std::filesystem::absolute(filename_obj).string();
            std::string absolute_path_mtl = std::filesystem::absolute(filename_mtl).string();
            std::cout << "Absolute path to spoon.obj: " << absolute_path_spoon << std::endl;
            std::ifstream objFile(absolute_path_spoon);
            std::ifstream mtlFile(absolute_path_mtl);
            if (!objFile || !mtlFile) {
                throw std::runtime_error("Failed to open OBJ/MTL file");
            }
             objText=std::string((std::istreambuf_iterator<char>(objFile)), std::istreambuf_iterator<char>());
             mtlText=std::string((std::istreambuf_iterator<char>(mtlFile)), std::istreambuf_iterator<char>());

        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        if(objText.empty() || mtlText.empty()) {
            std::cerr << "Error: OBJ or MTL file is empty." << std::endl;
            return;
        }
        std::cout << "OBJ and MTL files loaded" << std::endl;
        try
        {
           std::pair<std::vector<objmini::Vertex>, std::vector<uint32_t>> result;

            result=objmini::BuildVerticesAndIndices(objText, mtlText);
           Object spoon(
                std::vector<float>{0, 0, 0}, // Position
                std::vector<float>{0, 0, 0}, // Orientation
                std::vector<float>{1, 1, 1}, // Scale
                255, 255, 255, // Color (white)
                "spoon" // Name
            );
            int first_index = -1;
            for (const auto& vertex : result.first) {
                // Create a Vertex object for each vertex in the spoon model
                std::shared_ptr<Vertex> v = std::make_shared<Vertex>(
                    std::vector<float>{vertex.pos.x, vertex.pos.y, vertex.pos.z},
                    std::vector<float>{0, 0, 0}, // Orientation
                    std::vector<float>{1, 1, 1}, // Scale
                    static_cast<uint8_t>(vertex.u * 255), // Color R
                    static_cast<uint8_t>(vertex.v * 255), // Color G
                    static_cast<uint8_t>(vertex.norm.x * 255), // Color B
                    "spoon_vertex"
                );
                if(first_index == -1) {
                    first_index = v.get()->id; // Store the ID of the first vertex
                }
                v.get()->id;
                spoon.add_child(v);
            }
            for (size_t i = 0; i < result.second.size(); i += 3) {
                // Create an index triplet for the spoon model
                std::array<int, 3> index_triplet = {
                    first_index + result.second[i],
                    first_index + result.second[i + 1],
                    first_index + result.second[i + 2]
                };
                if(i%100 == 0) std::cout << "Adding index triplet: " << index_triplet[0] << ", " << index_triplet[1] << ", " << index_triplet[2] << std::endl;

                index_buffer->push_back(index_triplet);
            }
        objects->push_back(std::make_shared<Object>(spoon));
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
}
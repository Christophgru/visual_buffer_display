#define GLEW_STATIC

#include <windows.h>
#include <GL/glew.h>  // Must be included before any other GL headers.
#include "SDL3/SDL.h"
#include "SDL3/SDL_opengl.h" // Include OpenGL functions
#include "renderer/renderer.h"
#include "shapes/object.h"
#include "shapes/rectangle.h"
#include "shapes/circle.h"
#include "shapes/triangle.h"
#include <exception>
#include <iostream>
#include <memory>
#include <fstream>
#include "physics_engine/physics_engine.h"
#include "object_loader/object_loader.h"
#include "camera/camera.h"
#include <filesystem>
//#include <SDL_mouse_c.h"

// Declare the function if it's defined elsewhere
void populate_szene(std::shared_ptr<std::vector<std::shared_ptr<Object>>> objects,std::shared_ptr<std::vector<std::array<int,3>>> index_buffer);


int main(int argc, char* argv[]) {
    printf("Starting program...\n");

    try {
        int WIDTH = 800;
        int HEIGHT = 600;

        // Initialize SDL with video support
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            SDL_Log("SDL could not initialize! SDL_Error: %s", SDL_GetError());
            return -1;
        }
        printf("SDL initialized successfully.\n");

        // Create an SDL window with the SDL_WINDOW_OPENGL flag
        SDL_Window* window = SDL_CreateWindow("Pixel Buffer Renderer",
                                              WIDTH, HEIGHT,
                                              SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
        if (!window) {
            SDL_Log("Window could not be created! SDL_Error: %s", SDL_GetError());
            SDL_Quit();
            return -1;
        }
        printf("SDL window created successfully.\n");

        // Create an OpenGL context for the window
        SDL_GLContext glContext = SDL_GL_CreateContext(window);
        if (!glContext) {
            SDL_Log("OpenGL context could not be created! SDL_Error: %s", SDL_GetError());
            SDL_DestroyWindow(window);
            SDL_Quit();
            return -1;
        }

        // Set up the OpenGL viewport
        glViewport(0, 0, WIDTH, HEIGHT);
        printf("OpenGL viewport set up successfully.\n");

        // Create shapes (your current objects)
        auto objects = std::make_shared<std::vector<std::shared_ptr<Object>>>();
        std::shared_ptr<std::vector<std::array<int,3>>> index_buffer = std::make_shared<std::vector<std::array<int,3>>>();
        populate_szene(objects, index_buffer);
        auto camera = std::make_shared<Camera>(std::vector<float>{0, 0, 0},
                                               std::vector<float>{0, 100, 0},
                                                40);
        std::cout << "Camera initialized" << std::endl;
        // Initialize your renderer (ensure it is adapted to use OpenGL if needed)
        Renderer renderer(window, WIDTH, HEIGHT, objects, camera, index_buffer);
        std::cout << "Renderer initialized" << std::endl;

        bool running = true;
        SDL_Event event;
        uint32_t lastTime = SDL_GetTicks();
        uint32_t lastMoveTime = SDL_GetTicks();
        float frameCount = 0;
        PhysicsEngine physicsEngine(objects, renderer, camera,{WIDTH,HEIGHT});
        //SDL_SetRelativeMouseMode(true);
        std::cout << "Physics Engine initialized" << std::endl; 

        while (running) {
            while (SDL_PollEvent(&event)) {
                
                auto data=physicsEngine.handleEvent(event);
                switch (std::get<0>(data))
                {
                    case terminate:
                        running=false;
                        break;
                    case window_resize:
                        WIDTH=std::get<1>(data).at(0);
                        HEIGHT=std::get<1>(data).at(1);
                        break;
                    default:
                        break;
                }
            }

            

            // Clear the screen using OpenGL (black background)
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            uint32_t currentTime = SDL_GetTicks();
            lastMoveTime = currentTime;
            
            physicsEngine.update();
            // Render your objects using your renderer which should now be utilizing OpenGL calls
            renderer.render();

            // Swap the OpenGL buffers
            SDL_GL_SwapWindow(window);

            frameCount++;
            if (currentTime - lastTime >= 1000) {
                float fps = frameCount / ((currentTime - lastTime) / 1000.0f);
                SDL_Log("FPS: %.2f", fps);
                frameCount = 0;
                lastTime = currentTime;
            }
        }

        // Cleanup OpenGL context and SDL resources
        SDL_DestroyWindow(window);
        SDL_Quit();
        printf("Program terminated successfully.\n");
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "Unknown exception occurred!" << std::endl;
        return -1;
    }
}

        
void populate_szene(std::shared_ptr<std::vector<std::shared_ptr<Object>>> objects,std::shared_ptr<std::vector<std::array<int,3>>> index_buffer )
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
        std::string objText;
        std::string mtlText;
            try
        {
            //load string from file
            //create absolute path
            std::string path_spoon = "external/newell_teaset/spoon.obj";
            std::string path_mtl = "external/newell_teaset/spoon.mtl";
            std::string absolute_path_spoon = std::filesystem::absolute(path_spoon).string();
            std::string absolute_path_mtl = std::filesystem::absolute(path_mtl).string();
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

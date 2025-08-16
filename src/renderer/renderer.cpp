#include "Renderer.h"
#include <SDL3/SDL_opengl.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <array>
#include <algorithm>
#include "../math/own_math.h"
#include <functional>
#include <iterator>

// Vertex and Fragment Shader source code
const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;
out vec3 fragColor;
void main() {
    fragColor = color;
    gl_PointSize = 1.0; // For rendering vertices as points
    gl_Position = vec4(position, 0.0, 1.0);
}
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    in vec3 fragColor;
    out vec4 outColor;
    void main() {
        outColor = vec4(fragColor, 1.0);
    }
    )";

// Utility function to compile shaders and link a program.
GLuint SimpleRenderer::createShaderProgram(const char* vertexSource, const char* fragmentSource) {
    // Compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    GLint status;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    if (!status) {
        char buffer[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
        std::cerr << "Vertex shader compilation error: " << buffer << std::endl;
    }
    
    // Compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
    if (!status) {
        char buffer[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
        std::cerr << "Fragment shader compilation error: " << buffer << std::endl;
    }
    
    // Link shaders into a program
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glBindAttribLocation(program, 0, "position");
    glBindAttribLocation(program, 1, "color");
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (!status) {
        char buffer[512];
        glGetProgramInfoLog(program, 512, NULL, buffer);
        std::cerr << "Shader program linking error: " << buffer << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

SimpleRenderer::SimpleRenderer(SDL_Window* window, int width, int height, 
    std::shared_ptr<Scene> scene)
    : width(width), height(height), scene(scene)
{
    // Assume an OpenGL context has already been created (with SDL_WINDOW_OPENGL)
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cerr << "Error initializing GLEW: " << glewGetErrorString(err) << std::endl;
    }
    
    // Set the viewport and enable blending for transparency if needed.
    glViewport(0, 0, width, height);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_PROGRAM_POINT_SIZE);
    // Compile and link the shader program.
    shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    
    // Generate a Vertex Array Object and a Vertex Buffer Object.
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
}


static void flattenInto(const ObjSP& obj, const ObjVecP& out) {
    out->push_back(obj);
    const auto& children = obj->get_children(); // shared_ptr<vector<shared_ptr<Object>>>
    if (!children) return;
    out->reserve(out->size() + children->size());
    for (const auto& ch : *children) {
        flattenInto(ch, out);
    }
}
// In this OpenGL version, we build a vertex array (positions + colors) from the scene->get_objects().
// For simplicity, we assume all scene->get_objects() are rendered as triangles in normalized device coordinates.
// You’ll need to convert your shape coordinates (which are in screen space) into NDC.
void SimpleRenderer::render() {
    // Clear the screen.
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(shaderProgram);

    // Data containers: one for triangles and one for vertices.
    std::vector<float> triangleData;
    std::vector<float> pointData;
    //flatten  tree of scene->get_objects() into list of object references


   // If your roots are shared_ptr<Object>
    ObjVecP flat = std::make_shared<ObjVec>();
    for (const auto& root : *scene->get_objects()) flattenInto(root, flat);

    // id -> weak_ptr so we don’t extend lifetimes
    IdMap idMap;
    idMap.reserve(flat->size());
    for (const auto& sp : *flat) idMap.emplace(sp->id, sp);

    // Iterate the *flattened* list so child vertices get added to pointData
    for (const auto& shape : *flat) {
        // Common color data.
        
        //print orientation of camera
        shape->in_frame=is_point_in_frame(shape->get_coords(),scene->get_camera()->pos,scene->get_camera()->orientation);

       
        std::array<uint8_t, 3> colors = shape->get_color();
        float r = colors[0] / 255.0f;
        float g = colors[1] / 255.0f;
        float b = colors[2] / 255.0f;
      

        if (shape->get_shape_type() == RECTANGLE) {
            auto rect = static_cast<Rect*>(shape.get());
            auto pos = rect->get_coords();
            float w = rect->get_width();
            float h = rect->get_height();
            // Convert screen coordinates to NDC.
            float x = (pos[0] / width) * 2.0f - 1.0f;
            float y = 1.0f - (pos[1] / height) * 2.0f;
            float ndcW = (w / width) * 2.0f;
            float ndcH = (h / height) * 2.0f;
            // Two triangles for the rectangle.
            triangleData.insert(triangleData.end(), { x, y, r, g, b });
            triangleData.insert(triangleData.end(), { x + ndcW, y, r, g, b });
            triangleData.insert(triangleData.end(), { x, y - ndcH, r, g, b });
            
            triangleData.insert(triangleData.end(), { x + ndcW, y, r, g, b });
            triangleData.insert(triangleData.end(), { x + ndcW, y - ndcH, r, g, b });
            triangleData.insert(triangleData.end(), { x, y - ndcH, r, g, b });
        }
        else if (shape->get_shape_type() == CIRCLE) {
            auto circle = static_cast<Circle*>(shape.get());
            auto pos = circle->get_coords();
            float radius = circle->get_radius();
            float cx = (pos[0] / width) * 2.0f - 1.0f;
            float cy = 1.0f - (pos[1] / height) * 2.0f;
            float ndcRadiusX = (radius / width) * 2.0f;
            float ndcRadiusY = (radius / height) * 2.0f;
            const int segments = 32;
            // Use a triangle fan for the circle.
            for (int i = 0; i < segments; ++i) {
                float theta1 = (2.0f * M_PI * i) / segments;
                float theta2 = (2.0f * M_PI * (i + 1)) / segments;
                triangleData.insert(triangleData.end(), { cx, cy, r, g, b });
                triangleData.insert(triangleData.end(), { cx + ndcRadiusX * static_cast<float>(cos(theta1)), cy + ndcRadiusY * static_cast<float>(sin(theta1)), r, g, b });
                triangleData.insert(triangleData.end(), { cx + ndcRadiusX * static_cast<float>(cos(theta2)), cy + ndcRadiusY * static_cast<float>(sin(theta2)), r, g, b });
            }
        }
        else if (shape->get_shape_type() == TRIANGLE) {
            if(shape->in_frame==false){
                continue; // Skip scene->get_objects() that are not in the frame
            }
            auto triangle = static_cast<Triangle*>(shape.get());
            auto pos = triangle->get_coords();
            float size = triangle->get_size();
            float x = (pos[0] / width) * 2.0f - 1.0f;
            float y = 1.0f - (pos[1] / height) * 2.0f;
            float ndcSizeX = (size / width) * 2.0f;
            float ndcSizeY = (size / height) * 2.0f;
            triangleData.insert(triangleData.end(), { x, y, r, g, b });
            triangleData.insert(triangleData.end(), { x + ndcSizeX, y, r, g, b });
            triangleData.insert(triangleData.end(), { x + ndcSizeX / 2, y - ndcSizeY, r, g, b });
        }
        else if (shape->get_shape_type() == VERTEX) {
            if(shape->in_frame==false){
                continue; // Skip scene->get_objects() that are not in the frame
            }
            // For vertices, use the camera's orientation and position to project.
            auto vertex = static_cast<Vertex*>(shape.get());
            auto pos = vertex->get_coords();
            // Use your custom projection function.
            std::array<float,2> coords = project(pos, scene->get_camera()->orientation, scene->get_camera()->pos);
            // In this OpenGL version, we simulate drawing a "block" by using a point with an increased size.
            // (Alternatively, you could add several points to form a quad.)
            // Assume 'coords' are already in NDC or convert from screen space if needed.
            pointData.insert(pointData.end(), { coords[0], coords[1], r, g, b });
        }
        // Extend with other shape types if needed.
    }

    // Now process the index_buffer to draw triangles based on shape ids.
    // For each index triplet, find the corresponding scene->get_objects(), project their coordinates,
    // and add a triangle with per-vertex colors.
    hand_data_to_shader(triangleData, pointData, idMap);
}

void SimpleRenderer::hand_data_to_shader(std::vector<float> triangleData,
                                   std::vector<float> pointData,
                                   const IdMap& idMap)
{
    auto getObj = [&](int id) -> std::shared_ptr<Object> {
        auto it = idMap.find(id);
        if (it == idMap.end())
            throw std::runtime_error("Object not found for given index: " + std::to_string(id));
        auto sp = it->second.lock();
        if (!sp)
            throw std::runtime_error("Indexed object expired: " + std::to_string(id));
        return sp;
    };

    for (const std::array<int,3>& idx : *scene->get_index_buffer()) {
        auto s1 = getObj(idx[0]);
        auto s2 = getObj(idx[1]);
        auto s3 = getObj(idx[2]);

        auto p1 = project(s1->get_coords(), scene->get_camera()->orientation, scene->get_camera()->pos);
        auto p2 = project(s2->get_coords(), scene->get_camera()->orientation, scene->get_camera()->pos);
        auto p3 = project(s3->get_coords(), scene->get_camera()->orientation, scene->get_camera()->pos);

        auto c1 = s1->get_color(); auto c2 = s2->get_color(); auto c3 = s3->get_color();
        float r1=c1[0]/255.f,g1=c1[1]/255.f,b1=c1[2]/255.f;
        float r2=c2[0]/255.f,g2=c2[1]/255.f,b2=c2[2]/255.f;
        float r3=c3[0]/255.f,g3=c3[1]/255.f,b3=c3[2]/255.f;

        triangleData.insert(triangleData.end(), {
            p1[0], p1[1], r1, g1, b1,
            p2[0], p2[1], r2, g2, b2,
            p3[0], p3[1], r3, g3, b3
        });
    }

    // --- draw triangles (same as your original) ---
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    if (!triangleData.empty()) {
        glBufferData(GL_ARRAY_BUFFER, triangleData.size()*sizeof(float), triangleData.data(), GL_DYNAMIC_DRAW);
        GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
        glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
        glEnableVertexAttribArray(posAttrib);
        GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
        glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));
        glEnableVertexAttribArray(colAttrib);
        glDrawArrays(GL_TRIANGLES, 0, triangleData.size()/5);
    }

    // --- draw points (same as your original) ---
    if (!pointData.empty()) {
        glBufferData(GL_ARRAY_BUFFER, pointData.size()*sizeof(float), pointData.data(), GL_DYNAMIC_DRAW);
        GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
        glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
        glEnableVertexAttribArray(posAttrib);
        GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
        glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));
        glEnableVertexAttribArray(colAttrib);
        glDrawArrays(GL_POINTS, 0, pointData.size()/5);
    }
    glBindVertexArray(0);
}



bool SimpleRenderer::is_point_in_frame(const std::vector<float> point, const std::vector<float> camera_pos, const std::vector<float> camera_orientation) {
    // Calculate the vector from the camera to the point
    Vector3 camera_to_point = {point[0] - camera_pos[0],point[1]- camera_pos[1] , point[2] - camera_pos[2]};
    
    // Calculate the dot product with the camera orientation vector
    Vector3 orientation_vector = {-camera_orientation[0], camera_orientation[1], camera_orientation[2]};
    
    float dot_product = dotProduct(camera_to_point, orientation_vector);
    
    // If the dot product is negative, the point is behind the camera
    return  1;
}


void SimpleRenderer::hand_data_to_shader(std::vector<float> triangleData,    std::vector<float> pointData){
    for (const std::array<int,3>& index : *scene->get_index_buffer()) {
        Object* shape1 = nullptr;
        Object* shape2 = nullptr;
        Object* shape3 = nullptr;
        for (const auto& shape : *scene->get_objects()) {
            if (shape->id == index[0])
                shape1 = shape.get();
            else if (shape->id == index[1])
                shape2 = shape.get();
            else if (shape->id == index[2])
                shape3 = shape.get();
        }
        if (!shape1 || !shape2 || !shape3) {
            throw std::runtime_error("Object not found for given index");
        }
        // Project each shape's coordinate using the camera parameters.
        std::array<float,2> coords1 = project(shape1->get_coords(), scene->get_camera()->orientation, scene->get_camera()->pos);
        std::array<float,2> coords2 = project(shape2->get_coords(), scene->get_camera()->orientation, scene->get_camera()->pos);
        std::array<float,2> coords3 = project(shape3->get_coords(), scene->get_camera()->orientation, scene->get_camera()->pos);
        // Get colors for each shape.
        std::array<uint8_t, 3> colors1 = shape1->get_color();
        std::array<uint8_t, 3> colors2 = shape2->get_color();
        std::array<uint8_t, 3> colors3 = shape3->get_color();
        // Convert color components.
        float r1 = colors1[0] / 255.0f, g1 = colors1[1] / 255.0f, b1 = colors1[2] / 255.0f;
        float r2 = colors2[0] / 255.0f, g2 = colors2[1] / 255.0f, b2 = colors2[2] / 255.0f;
        float r3 = colors3[0] / 255.0f, g3 = colors3[1] / 255.0f, b3 = colors3[2] / 255.0f;
        // Add the triangle vertices.
        triangleData.insert(triangleData.end(), { coords1[0], coords1[1], r1, g1, b1 });
        triangleData.insert(triangleData.end(), { coords2[0], coords2[1], r2, g2, b2 });
        triangleData.insert(triangleData.end(), { coords3[0], coords3[1], r3, g3, b3 });
    }

    // Bind the VAO and VBO for rendering.
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // --- Render triangle-based scene->get_objects() ---
    if (!triangleData.empty()) {
        glBufferData(GL_ARRAY_BUFFER, triangleData.size() * sizeof(float), triangleData.data(), GL_DYNAMIC_DRAW);
        GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
        glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(posAttrib);
        GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
        glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(colAttrib);
        glDrawArrays(GL_TRIANGLES, 0, triangleData.size() / 5);
    }

    // --- Render vertices as points ---
    if (!pointData.empty()) {
        // Set an increased point size to simulate a pixel block.
        glPointSize(1.0f);
        glBufferData(GL_ARRAY_BUFFER, pointData.size() * sizeof(float), pointData.data(), GL_DYNAMIC_DRAW);
        GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
        glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(posAttrib);
        GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
        glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(colAttrib);
        glDrawArrays(GL_POINTS, 0, pointData.size() / 5);
    }
    
    // Unbind the VAO.
    glBindVertexArray(0);
}

std::array<float,2> SimpleRenderer::project(std::vector<float> pos, std::vector<float> camera_orientation, std::vector<float> camera_pos){
    // Calculate camera and relative angles (in degrees)
    float camara_elev = atan2(camera_orientation[2], sqrt(pow(camera_orientation[0],2) + pow(camera_orientation[1],2))) * 180.0f / M_PI;
    float camera_azimuth = -atan2(camera_orientation[1], camera_orientation[0]) * 180.0f / M_PI + 90.0f;
    float relative_elev = -atan2(pos[2] - camera_pos[2], sqrt(pow(pos[0] - camera_pos[0],2) + pow(pos[1] - camera_pos[1],2))) * 180.0f / M_PI;
    float relative_azimuth = -atan2(pos[1] - camera_pos[1], pos[0] - camera_pos[0]) * 180.0f / M_PI + 90.0f;
    
    float az_for_screen = relative_azimuth + camera_azimuth;
    if (az_for_screen > 180.0f)  az_for_screen -= 360.0f;
    if (az_for_screen < -180.0f) az_for_screen += 360.0f;
    if (fabsf(camera_azimuth) > 90.0f) {
        // Use shortest signed angle between point and camera heading
        float dYaw = relative_azimuth + camera_azimuth;
        // wrap to [-180, 180]
        while (dYaw > 180.0f)  dYaw -= 360.0f;
        while (dYaw < -180.0f) dYaw += 360.0f;
        az_for_screen = dYaw;  // only in the >±90° case
    }
    float el_for_screen = relative_elev + camara_elev;
    if (fabsf(camera_azimuth) > 90.0f) {
        // Use shortest signed pitch difference to avoid flip when heading past ±90°
        float dPit = relative_elev - camara_elev;
        while (dPit > 180.0f)  dPit -= 360.0f;
        while (dPit < -180.0f) dPit += 360.0f;
        el_for_screen = dPit;
    }


    float screenX = width / 2.0f + (az_for_screen) / scene->get_camera()->fov_width_deg * width / 1000.0f;
    float screenY = height / 2.0f + (relative_elev + camara_elev) / scene->get_camera()->fov_height_deg * height / 1000.0f;

    // Convert screen-space coordinates to NDC: 
    // x_ndc = (screenX / width)*2 - 1, y_ndc = 1 - (screenY / height)*2
    float ndcX = (screenX / width) * 2.0f - 1.0f;
    float ndcY = 1.0f - (screenY / height) * 2.0f;
    
    return {ndcX, ndcY}; 
}



void SimpleRenderer::resize(int newWidth, int newHeight) {
    width = newWidth;
    height = newHeight;
    // Update the viewport to the new window size.
    glViewport(0, 0, width, height);
}

int SimpleRenderer::getWindowWidth() {
    return width;
}
int SimpleRenderer::getWindowHeight() {
    return height;
}

SimpleRenderer::~SimpleRenderer() {
    glDeleteProgram(shaderProgram);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

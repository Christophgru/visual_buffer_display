#include "Renderer.h"
#include <SDL3/SDL_opengl.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <array>
#include <algorithm>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Vertex and Fragment Shader source code
const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;
out vec3 fragColor;
void main() {
    fragColor = color;
    gl_PointSize = 20.0; // For rendering vertices as points
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
GLuint Renderer::createShaderProgram(const char* vertexSource, const char* fragmentSource) {
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

Renderer::Renderer(SDL_Window* window, int width, int height, 
    std::shared_ptr<std::vector<Shape*>> shapes,
    std::shared_ptr<Camera> camera,
    std::shared_ptr<std::vector<std::array<int,3>>> index_buffer)
    : width(width), height(height), shapes(shapes), camera(camera), index_buffer(index_buffer)
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

// In this OpenGL version, we build a vertex array (positions + colors) from the shapes.
// For simplicity, we assume all shapes are rendered as triangles in normalized device coordinates.
// You’ll need to convert your shape coordinates (which are in screen space) into NDC.
void Renderer::render() {
    // Clear the screen.
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(shaderProgram);

    // Data containers: one for triangles and one for vertices.
    std::vector<float> triangleData;
    std::vector<float> pointData;

    // Process each shape in the scene.
    for (Shape* shape : *shapes) {
        // Common color data.
        std::array<uint8_t, 3> colors = shape->get_color();
        float r = colors[0] / 255.0f;
        float g = colors[1] / 255.0f;
        float b = colors[2] / 255.0f;

        if (shape->get_shape_type() == RECTANGLE) {
            auto rect = static_cast<Rect*>(shape);
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
            auto circle = static_cast<Circle*>(shape);
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
            auto triangle = static_cast<Triangle*>(shape);
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
            // For vertices, use the camera's orientation and position to project.
            auto vertex = static_cast<Vertex*>(shape);
            auto pos = vertex->get_coords();
            // Use your custom projection function.
            std::array<float,2> coords = project(pos, camera->orientation, camera->pos);
            // In this OpenGL version, we simulate drawing a "block" by using a point with an increased size.
            // (Alternatively, you could add several points to form a quad.)
            // Assume 'coords' are already in NDC or convert from screen space if needed.
            pointData.insert(pointData.end(), { coords[0], coords[1], r, g, b });
        }
        // Extend with other shape types if needed.
    }

    // Now process the index_buffer to draw triangles based on shape ids.
    // For each index triplet, find the corresponding shapes, project their coordinates,
    // and add a triangle with per-vertex colors.
    for (const std::array<int,3>& index : *index_buffer) {
        Shape* shape1 = nullptr;
        Shape* shape2 = nullptr;
        Shape* shape3 = nullptr;
        for (Shape* shape : *shapes) {
            if (shape->id == index[0])
                shape1 = shape;
            else if (shape->id == index[1])
                shape2 = shape;
            else if (shape->id == index[2])
                shape3 = shape;
        }
        if (!shape1 || !shape2 || !shape3) {
            throw std::runtime_error("Shape not found for given index");
        }
        // Project each shape's coordinate using the camera parameters.
        std::array<float,2> coords1 = project(shape1->get_coords(), camera->orientation, camera->pos);
        std::array<float,2> coords2 = project(shape2->get_coords(), camera->orientation, camera->pos);
        std::array<float,2> coords3 = project(shape3->get_coords(), camera->orientation, camera->pos);
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

    // --- Render triangle-based shapes ---
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

std::array<float,2> Renderer::project(std::vector<float> pos, std::vector<float> camera_orientation, std::vector<float> camera_pos){
    // Calculate camera and relative angles (in degrees)
    float camara_elev = atan2(camera_orientation[2], sqrt(pow(camera_orientation[0],2) + pow(camera_orientation[1],2))) * 180.0f / M_PI;
    float camera_azimuth = -atan2(camera_orientation[1], camera_orientation[0]) * 180.0f / M_PI + 90.0f;
    float relative_elev = atan2(pos[2] - camera_pos[2], sqrt(pow(pos[0] - camera_pos[0],2) + pow(pos[1] - camera_pos[1],2))) * 180.0f / M_PI;
    float relative_azimuth = -atan2(pos[1] - camera_pos[1], pos[0] - camera_pos[0]) * 180.0f / M_PI + 90.0f;
    
    // Calculate screen-space coordinates (in pixels)
    float screenX = width / 2.0f + (relative_azimuth + camera_azimuth) / camera->fov_width_deg * width / 1000.0f;
    float screenY = height / 2.0f + (relative_elev + camara_elev) / camera->fov_height_deg * height / 1000.0f;

    // Convert screen-space coordinates to NDC: 
    // x_ndc = (screenX / width)*2 - 1, y_ndc = 1 - (screenY / height)*2
    float ndcX = (screenX / width) * 2.0f - 1.0f;
    float ndcY = 1.0f - (screenY / height) * 2.0f;
    
    return {ndcX, ndcY}; 
}



void Renderer::resize(int newWidth, int newHeight) {
    width = newWidth;
    height = newHeight;
    // Update the viewport to the new window size.
    glViewport(0, 0, width, height);
}

int Renderer::getWindowWidth() {
    return width;
}
int Renderer::getWindowHeight() {
    return height;
}

Renderer::~Renderer() {
    glDeleteProgram(shaderProgram);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

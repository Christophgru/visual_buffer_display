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

    // Two separate containers: one for triangle-based shapes and one for vertices.
    std::vector<float> triangleData;
    std::vector<float> pointData;

    for (Shape* shape : *shapes) {
        // Get the common color data.
        std::array<uint8_t, 3> colors = shape->get_color();
        float r = colors[0] / 255.0f;
        float g = colors[1] / 255.0f;
        float b = colors[2] / 255.0f;

        if (shape->get_shape_type() == RECTANGLE) {
            auto rect = static_cast<Rect*>(shape);
            auto pos = rect->get_coords();
            float w = rect->get_width();
            float h = rect->get_height();
            float x = (pos[0] / width) * 2.0f - 1.0f;
            float y = 1.0f - (pos[1] / height) * 2.0f;
            float ndcW = (w / width) * 2.0f;
            float ndcH = (h / height) * 2.0f;
            // Two triangles for the rectangle.
            // Triangle 1.
            triangleData.insert(triangleData.end(), { x, y, r, g, b });
            triangleData.insert(triangleData.end(), { x + ndcW, y, r, g, b });
            triangleData.insert(triangleData.end(), { x, y - ndcH, r, g, b });
            // Triangle 2.
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
                // Center vertex.
                triangleData.insert(triangleData.end(), { cx, cy, r, g, b });
                // First edge vertex.
                triangleData.insert(triangleData.end(), { cx + ndcRadiusX * cos(theta1), cy + ndcRadiusY * sin(theta1), r, g, b });
                // Second edge vertex.
                triangleData.insert(triangleData.end(), { cx + ndcRadiusX * cos(theta2), cy + ndcRadiusY * sin(theta2), r, g, b });
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
            // For a simple equilateral triangle.
            triangleData.insert(triangleData.end(), { x, y, r, g, b });
            triangleData.insert(triangleData.end(), { x + ndcSizeX, y, r, g, b });
            triangleData.insert(triangleData.end(), { x + ndcSizeX / 2, y - ndcSizeY, r, g, b });
        }
        else if (shape->get_shape_type() == VERTEX) {
            // Handle vertices by drawing them as points.
            auto vertex = static_cast<Vertex*>(shape);
            auto pos = vertex->get_coords();
            float x = (pos[0] / width) * 2.0f - 1.0f;
            float y = 1.0f - (pos[1] / height) * 2.0f;
            pointData.insert(pointData.end(), { x, y, r, g, b });
        }
        // Extend similarly for other shape types if needed...
    }
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    // Draw triangle-based shapes.
    if (!triangleData.empty()) {
        glBufferData(GL_ARRAY_BUFFER, triangleData.size() * sizeof(float), triangleData.data(), GL_DYNAMIC_DRAW);
        // Set up attribute pointers.
        GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
        glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(posAttrib);
        GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
        glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(colAttrib);
        glDrawArrays(GL_TRIANGLES, 0, triangleData.size() / 5);
    }

    // Draw vertices as points.
    if (!pointData.empty()) {
        // Set an appropriate point size.
        glPointSize(5.0f);
        glBufferData(GL_ARRAY_BUFFER, pointData.size() * sizeof(float), pointData.data(), GL_DYNAMIC_DRAW);
        // Setup attribute pointers again.
        GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
        glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(posAttrib);
        GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
        glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(colAttrib);
        glDrawArrays(GL_POINTS, 0, pointData.size() / 5);
    }
    
    // Unbind VAO.
    glBindVertexArray(0);
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

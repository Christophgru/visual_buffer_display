#include "Renderer.h"
#include <cstring> // For memset
#include <stdio.h>
#include <iostream>
#include <array>
#include <cmath>
#include <vector>
#include <algorithm>

// Constructor: Initializes SDL Renderer and Texture
Renderer::Renderer(SDL_Window* window, int width, int height, 
    std::shared_ptr<std::vector<Shape*>> shapes, std::shared_ptr<Camera> camera,std::shared_ptr<std::vector<std::array<int,3>>> index_buffer)
    :index_buffer(index_buffer), width(width), height(height), buffer(width * height, 0x000000FF),camera(camera) { // Default black
    renderer = SDL_CreateRenderer(window, NULL);
    this->shapes = shapes;
    if (!renderer) {
        SDL_Log("Renderer could not be created! SDL_Error: %s", SDL_GetError());
    }

    // Create a texture to store the pixel data
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
}

// Destructor: Cleans up SDL resources
Renderer::~Renderer() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
}

// Set an individual pixel in the buffer
void Renderer::setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    if (x < 0 || x >= width || y < 0 || y >= height) return; // Out-of-bounds check
    buffer[y * width + x] = (r << 24) | (g << 16) | (b << 8) | 0xFF; // RGBA format
}

// Clear the buffer to a specific color
void Renderer::clearBuffer(uint32_t color) {
    std::fill(buffer.begin(), buffer.end(), color);
}

// Fill the buffer with a gradient from blue to grey
void Renderer::fillGradient() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            uint8_t blue = 255 - (255 * y / height);
            uint8_t grey = 255 * y / height;
            setPixel(x, y, 0, 0, 0);
        }
    }
}
std::array<float,2> Renderer::project(std::vector<float> pos,std::vector<float> camera_orientation,std::vector<float> camera_pos){
    float camara_elev=atan2(camera_orientation[2],sqrt(pow(camera_orientation[0],2)+pow(camera_orientation[1],2)))*180/3.14159265359;
    float camera_azimuth=-atan2(camera_orientation[1],camera_orientation[0])*180/3.14159265359+90;
    float relative_elev=atan2(pos[2]-camera_pos[2],sqrt(pow(pos[0]-camera_pos[0],2)+pow(pos[1]-camera_pos[1],2)))*180/3.14159265359;
    float relative_azimuth=-atan2(pos[1]-camera_pos[1],pos[0]-camera_pos[0])*180/3.14159265359+90;
    //printf("Relative azimuth: %f, Relative elevation: %f\n", relative_azimuth, relative_elev);	
    float y=height/2+ (relative_elev+camara_elev)/camera->fov_height_deg*height/1000;
    float x=width/2+(relative_azimuth+camera_azimuth)/camera->fov_width_deg*width/1000;
    return {x,y}; 
}

// Render the buffer to the screen
void Renderer::render() {
    fillGradient(); // Fill the buffer with the gradient before rendering

    for (Shape* shape : *shapes) {
        //printf("Shape type: %d\n", shape->get_shape_type());
        std::array<uint8_t, 3> colors = shape->get_color();
        uint8_t r = colors[0];
        uint8_t g = colors[1];
        uint8_t b = colors[2];
        if (shape->get_shape_type() == RECTANGLE) {
            auto rect = static_cast<Rectangle*>(shape);
            auto pos = rect->get_coords();
            auto width = rect->get_width();
            auto height = rect->get_height();
            for (float i = pos[0]; i < pos[0] + width; ++i) {
                for (float j = pos[1]; j < pos[1] + height; ++j) {
                    setPixel(i, j, r, g, b);
                }
            }
        } else if (shape->get_shape_type() == CIRCLE) {
            auto circle = static_cast<Circle*>(shape);
            auto pos = circle->get_coords();
            auto radius = circle->get_radius();
            for (float i = pos[0] - radius; i < pos[0] + radius; ++i) {
                for (float j = pos[1] - radius; j < pos[1] + radius; ++j) {
                    if (std::sqrt((i - pos[0]) * (i - pos[0]) + (j - pos[1]) * (j - pos[1])) <= radius) {
                        setPixel(i, j, r, g, b);
                    }
                }
            }
        } else if (shape->get_shape_type() == TRIANGLE) {
            auto triangle = static_cast<Triangle*>(shape);
            auto pos = triangle->get_coords();
            auto size = triangle->get_size();
            for (float i = pos[0]; i < pos[0] + size; ++i) {
                for (float j = pos[1]; j < pos[1] + size; ++j) {
                    if (i <= pos[0] + size && j <= pos[1] + size && j >= pos[1] + size - i) {
                        setPixel(i, j, r, g, b);
                    }
                }
            }
        }
        else if (shape->get_shape_type() == VERTEX) {
            auto vertex = static_cast<Vertex*>(shape);
            auto pos = vertex->get_coords();
            //printf("Vertex at %f, %f, %f\n", pos[0], pos[1],pos[2]);
            std::vector<float> camera_orientation = camera->orientation;
            //printf("Camera orientation: %f, %f, %f\n", camera_orientation[0], camera_orientation[1],camera_orientation[2]);
            std::vector<float> camera_pos = camera->pos;
            //printf("Camera pos: %f, %f, %f\n", camera_pos[0], camera_pos[1],camera_pos[2]);
            std::array<float,2> coords=project(pos,camera_orientation,camera_pos);
            for(int i=-2;i<4;i++){
                for(int j=-2;j<4;j++){
                    setPixel((int)coords[0]+i,(int)coords[1]+j, r, g, b);
                }
            }
           // printf("Vertex at %f, %f, %f Displayed at %d %d height: %d, width: %d\n", pos[0], pos[1],pos[2],(int)x,(int)y,height,width);
        }
        //increment over index_buffer of type std::shared_ptr<std::vector<std::array<int,3>>> index_buffer;
        for(std::array<int,3> index: *index_buffer){
            //printf("Index: %d, %d, %d\n", index[0], index[1], index[2]);
            //find shape with shape->get_id()==index[0]
            Shape* shape1;
            Shape* shape2;
            Shape* shape3;
            for(Shape* shape: *shapes){
                if(shape->id==index[0]){
                    shape1=shape;
                }else if(shape->id==index[1]){
                    shape2=shape;}
                else if(shape->id==index[2]){
                    shape3=shape;
                }
            }
            if(!shape1 || !shape2 || !shape3){
                throw std::runtime_error("Shape not found");
            }
             std::array<float,2> coords1=project(shape1->get_coords(),camera->orientation,camera->pos);
             std::array<uint8_t, 3> colors1 = shape1->get_color();
             std::array<float,2> coords2=project(shape2->get_coords(),camera->orientation,camera->pos);  
             std::array<uint8_t, 3> colors2 = shape2->get_color();
             std::array<float,2> coords3=project(shape3->get_coords(),camera->orientation,camera->pos);
             std::array<uint8_t, 3> colors3 = shape3->get_color();
             
             drawTriangleColor(coords1,coords2,coords3,colors1,colors2,colors3);
             //printf("coords1: %f, %f, coords2: %f, %f, coords3: %f, %f\n", coords1[0], coords1[1],coords2[0], coords2[1],coords3[0], coords3[1]);
        }
             
           
        
    }
    SDL_UpdateTexture(texture, nullptr, buffer.data(), width * sizeof(uint32_t));

    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, nullptr, nullptr); // SDL3 function replaces SDL_RenderCopy

    SDL_RenderPresent(renderer);
}



// Function to draw a triangle given three 2D points.
void Renderer::drawVerticalLine(int x, int yStart, int yEnd, const std::array<uint8_t, 3>& color) {
    if (yStart > yEnd) std::swap(yStart, yEnd);
    for (int y = yStart; y <= yEnd; ++y) {
        setPixel(x, y, color[0], color[1], color[2]);
    }
}

// Function to draw a triangle with color interpolation.
void Renderer::drawTriangleColor(const std::array<float, 2>& v0,
                       const std::array<float, 2>& v1,
                       const std::array<float, 2>& v2,
                       const std::array<uint8_t, 3>& c0,
                       const std::array<uint8_t, 3>& c1,
                       const std::array<uint8_t, 3>& c2)
{
    // Compute the triangle's bounding box.
    float minX = std::min({v0[0], v1[0], v2[0]});
    float maxX = std::max({v0[0], v1[0], v2[0]});
    float minY = std::min({v0[1], v1[1], v2[1]});
    float maxY = std::max({v0[1], v1[1], v2[1]});

    // Compute twice the triangle area (denom) used in barycentrics.
    float denom = ((v1[1] - v2[1]) * (v0[0] - v2[0]) + (v2[0] - v1[0]) * (v0[1] - v2[1]));
    const float epsilon = 1e-6f;
    if (std::fabs(denom) < epsilon) {
        // Degenerate triangle: the vertices are nearly collinear.
        printf("Degenerate triangle: area is zero or nearly zero.\n");
        // Since all x-values (or y-values) may be the same or nearly the same,
        // round them to integer pixel positions.
        int x = static_cast<int>(std::round(v0[0]));  // Assuming vertical line for degenerate case.
        int y0 = static_cast<int>(std::round(v0[1]));
        int y1 = static_cast<int>(std::round(v1[1]));
        int y2 = static_cast<int>(std::round(v2[1]));
        int yMin = std::min({y0, y1, y2});
        int yMax = std::max({y0, y1, y2});
        // For the degenerate case, we can simply use one of the endpoint colors.
        drawVerticalLine(x, yMin, yMax, c0);
        return;
    }

    // Iterate over each pixel within the bounding box.
    for (int y = static_cast<int>(std::floor(minY)); y <= static_cast<int>(std::ceil(maxY)); ++y) {
        for (int x = static_cast<int>(std::floor(minX)); x <= static_cast<int>(std::ceil(maxX)); ++x) {
            // Compute barycentric coordinates for the current pixel.
            float w0 = ((v1[1] - v2[1]) * (x - v2[0]) + (v2[0] - v1[0]) * (y - v2[1])) / denom;
            float w1 = ((v2[1] - v0[1]) * (x - v2[0]) + (v0[0] - v2[0]) * (y - v2[1])) / denom;
            float w2 = 1.0f - w0 - w1;

            // If the point lies inside the triangle (including edges).
            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                // Interpolate the color using barycentric weights.
                uint8_t r = static_cast<uint8_t>(std::round(w0 * c0[0] + w1 * c1[0] + w2 * c2[0]));
                uint8_t g = static_cast<uint8_t>(std::round(w0 * c0[1] + w1 * c1[1] + w2 * c2[1]));
                uint8_t b = static_cast<uint8_t>(std::round(w0 * c0[2] + w1 * c1[2] + w2 * c2[2]));
                setPixel(x, y, r, g, b);
            }
        }
    }
}




// Handle window resizing
void Renderer::resize(int newWidth, int newHeight) {
    width = newWidth;
    height = newHeight;
    buffer.resize(width * height, 0x000000FF); // Default black
    SDL_DestroyTexture(texture);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
}

int Renderer::getWindowWidth() {
    return width;
}
int Renderer::getWindowHeight() {
    return height;
}

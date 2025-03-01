#ifndef SHAPE_H
#define SHAPE_H

#include <vector>
#include "../renderer/renderer.h"

class Shape {
protected:
    std::vector<float> pos; // Now includes z-index
    std::vector<float> orientation;
    std::vector<float> scale;
    uint8_t r, g, b; // Color

public:
    Shape(std::vector<float> pos,std::vector<float> orientation,std::vector<float> scale,  uint8_t r, uint8_t g, uint8_t b)
        : pos(pos),orientation(orientation),scale(scale), r(r), g(g), b(b) {}

    virtual ~Shape() = default;

    virtual void draw(Renderer& renderer) = 0; // Pure virtual function
    virtual void move(float dx, float dy, float dz) { 
        pos[0] += dx; 
        pos[1] += dy; 
        pos[2] += dz; 
    } // Move shape in 3D space
    virtual void move_to(float x, float y, float z) { 
        pos[0] = x; 
        pos[1]= y; 
        pos[2]= z; 
    } // Move shape in 3D space

    std::vector<float> get_coords() { 
        return pos; 
    }
};

#endif // SHAPE_H

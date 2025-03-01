#ifndef SHAPE_H
#define SHAPE_H

#include <SDL3/SDL.h>
#include <vector>
#include "../renderer/renderer.h"

class Shape {
protected:
    std::vector<int> pos; // Now includes z-index
    uint8_t r, g, b; // Color

public:
    Shape(std::vector<int> pos, uint8_t r, uint8_t g, uint8_t b)
        : pos(pos), r(r), g(g), b(b) {}

    virtual ~Shape() = default;

    virtual void draw(Renderer& renderer) = 0; // Pure virtual function
    virtual void move(int dx, int dy, int dz) { 
        pos[0] += dx; 
        pos[1] += dy; 
        pos[2] += dz; 
    } // Move shape in 3D space
    virtual void move_to(int x, int y, int z) { 
        pos[0] = x; 
        pos[1]= y; 
        pos[2]= z; 
    } // Move shape in 3D space

    std::vector<int> get_coords() { 
        return pos; 
    }
};

#endif // SHAPE_H

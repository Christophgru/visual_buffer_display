#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "shape.h"

class Triangle : public Shape {
private:
    int size;

public:
    Triangle(std::vector<int> pos, int size, uint8_t r, uint8_t g, uint8_t b)
        : Shape(pos, r, g, b), size(size) {}

    void draw(Renderer& renderer) override {
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j <= i; ++j) { // Draw right-angled triangle
                renderer.setPixel(pos[0] + j, pos[1] + i, r, g, b);
            }
        }
    }
};

#endif // TRIANGLE_H

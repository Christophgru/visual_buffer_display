#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "shape.h"

class Rectangle : public Shape {
private:
    int width, height;

public:
    Rectangle(std::vector<int> pos, int width, int height, uint8_t r, uint8_t g, uint8_t b)
        : Shape(pos, r, g, b), width(width), height(height) {}

    void draw(Renderer& renderer) override {
        for (int i = pos[0]; i < pos[0] + width; ++i) {
            for (int j = pos[1]; j < pos[1] + height; ++j) {
                renderer.setPixel(i, j, r, g, b);
            }
        }
    }
};

#endif // RECTANGLE_H

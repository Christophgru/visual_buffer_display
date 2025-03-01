#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "shape.h"

class Rectangle : public Shape {
private:
    float width, height;

public:
    Rectangle(std::vector<float> pos,std::vector<float> orientation,std::vector<float> scale, float width, float height, uint8_t r, uint8_t g, uint8_t b)
        : Shape(pos,orientation,scale, r, g, b), width(width), height(height) {}

    void draw(Renderer& renderer) override {
        for (float i = pos[0]; i < pos[0] + width; ++i) {
            for (float j = pos[1]; j < pos[1] + height; ++j) {
                renderer.setPixel(i, j, r, g, b);
            }
        }
    }
};

#endif // RECTANGLE_H

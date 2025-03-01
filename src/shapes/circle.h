#ifndef CIRCLE_H
#define CIRCLE_H

#include "shape.h"
#include <cmath>

class Circle : public Shape {
private:
    float radius;

public:
    Circle(std::vector<float> pos,std::vector<float> orientation,std::vector<float> scale, float radius, uint8_t r, uint8_t g, uint8_t b)
        : Shape(pos,orientation,scale, r, g, b), radius(radius) {}

    void draw(Renderer& renderer) override {
        for (float i = -radius; i <= radius; ++i) {
            for (float j = -radius; j <= radius; ++j) {
                if (i * i + j * j <= radius * radius) { // Check if inside circle
                    renderer.setPixel(pos[0] + i, pos[1] + j, r, g, b);
                }
            }
        }
    }
};

#endif // CIRCLE_H

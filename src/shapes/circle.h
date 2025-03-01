#ifndef CIRCLE_H
#define CIRCLE_H

#include "shape.h"
#include <cmath>

class Circle : public Shape {
private:
    float radius;
public:
    Circle(std::vector<float> pos,std::vector<float> orientation,std::vector<float> scale, float radius, uint8_t r, uint8_t g, uint8_t b)
        : Shape(pos,orientation,scale, r, g, b), radius(radius) {shape_type=CIRCLE;}

        float get_radius() {
            return radius;
        }
};

#endif // CIRCLE_H

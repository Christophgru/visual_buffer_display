#ifndef CIRCLE_H
#define CIRCLE_H

#include "object.h"
#include <cmath>

class Circle : public Object {
private:
    float radius;
public:
    Circle(std::vector<float> pos,std::vector<float> orientation,std::vector<float> scale, float radius, uint8_t r, uint8_t g, uint8_t b, std::string name="Circle")
        : Object(pos,orientation,scale, r, g, b,name), radius(radius) {shape_type=CIRCLE;}

        float get_radius() {
            return radius;
        }
};

#endif // CIRCLE_H

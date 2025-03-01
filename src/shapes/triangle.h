#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "shape.h"

class Triangle : public Shape {
private:
    float size;

public:
    Triangle(std::vector<float> pos,std::vector<float> orientation,std::vector<float> scale, float size, uint8_t r, uint8_t g, uint8_t b)
        : Shape(pos,orientation,scale, r, g, b), size(size) {shape_type=TRIANGLE;}

        float get_size() {
            return size;
        }
};

#endif // TRIANGLE_H

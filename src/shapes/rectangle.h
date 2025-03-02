#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "shape.h"

class Rect : public Shape {
private:
    float width, height;

public:
    Rect(std::vector<float> pos,std::vector<float> orientation,std::vector<float> scale, float width, float height, uint8_t r, uint8_t g, uint8_t b)
        : Shape(pos,orientation,scale, r, g, b), width(width), height(height) {shape_type=RECTANGLE;}

        float get_width() {
            return width;
        }
        float get_height() {
            return height;
        }
};

#endif // RECTANGLE_H

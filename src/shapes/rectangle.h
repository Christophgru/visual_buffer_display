#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "object.h"

class Rect : public Object {
private:
    float width, height;

public:
    Rect(std::vector<float> pos,std::vector<float> orientation,std::vector<float> scale, float width, float height, uint8_t r, uint8_t g, uint8_t b,std::string name="Rectangle")
        : Object(pos,orientation,scale, r, g, b,name), width(width), height(height) {shape_type=RECTANGLE;}

        float get_width() {
            return width;
        }
        float get_height() {
            return height;
        }
};

#endif // RECTANGLE_H

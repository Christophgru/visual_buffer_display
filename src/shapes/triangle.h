#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object.h"

class Triangle : public Object {
private:
    float size;

public:
    Triangle(std::vector<float> pos,std::vector<float> orientation,std::vector<float> scale, float size, uint8_t r, uint8_t g, uint8_t b,std::string name="Triangle")
        : Object(pos,orientation,scale, r, g, b,name), size(size) {shape_type=TRIANGLE;}

        float get_size() {
            return size;
        }
};

#endif // TRIANGLE_H

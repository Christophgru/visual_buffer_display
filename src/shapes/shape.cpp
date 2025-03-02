#include   "shape.h"
int Shape::next_id = 0;

Shape::Shape(std::vector<float> pos, std::vector<float> orientation, std::vector<float> scale, uint8_t r, uint8_t g, uint8_t b)
    : pos(pos), orientation(orientation), scale(scale), r(r), g(g), b(b), id(next_id++) {}
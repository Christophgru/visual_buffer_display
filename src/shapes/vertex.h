#include "shape.h"
#include <cmath>
#include <cstdint>

class Vertex : public Shape {



public:
    Vertex(std::vector<float> pos,std::vector<float> orientation,std::vector<float> scale, uint8_t r, uint8_t g, uint8_t b)
        : Shape(pos,orientation,scale, r, g, b)  {shape_type=VERTEX;}

};


#include "object.h"
#include <cmath>
#include <cstdint>

class Vertex : public Object {



public:
    Vertex(std::vector<float> pos,std::vector<float> orientation,std::vector<float> scale, uint8_t r, uint8_t g, uint8_t b,std::string name="Vertex")
        : Object(pos,orientation,scale, r, g, b,name)  {shape_type=VERTEX;}

};


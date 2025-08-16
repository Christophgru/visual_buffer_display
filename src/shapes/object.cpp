#include   "object.h"
int Object::next_id = 0;

Object::Object(std::vector<float> pos, std::vector<float> orientation, std::vector<float> scale, uint8_t r, uint8_t g, uint8_t b,std::string name)
    : pos(pos), orientation(orientation), scale(scale), r(r), g(g), b(b), id(next_id++),name(name) {}

   void Object::move(float dx, float dy, float dz)  { 
        pos[0] += dx; 
        pos[1] += dy; 
        pos[2] += dz; 
    } // Move shape in 3D space

    void Object::move_to(float x, float y, float z) { 
        pos[0] = x; 
        pos[1]= y; 
        pos[2]= z; 
    } // Move shape in 3D space
    std::vector<float> Object::get_coords() { 
        return pos; 
    }
    std::array<uint8_t,3> Object::get_color() { 
        return {r, g, b}; 
    }
    void Object::add_child(std::shared_ptr<Object> child) { 
        children->push_back(child); 
    } // Add a child object
    std::shared_ptr<std::vector<std::shared_ptr<Object>>> Object::get_children() { 
        return children; 
    } // Get children objects

ShapeType Object::get_shape_type() { return shape_type; }

std::string Object::get_name() { 
        return name; 
    } // Get the name of the object
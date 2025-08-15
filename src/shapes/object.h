#ifndef SHAPE_H
#define SHAPE_H

#include <vector>
#include <array>
#include <exception>
#include <iostream>
#include <memory>

enum ShapeType {
    CIRCLE = 1,
    RECTANGLE = 2,
    TRIANGLE = 3,
    VERTEX = 4
};

class Object {
    private :
    
    //generate a const id that is unique for each shape
    static int next_id;
    
protected:
    std::string name; // Name of the object for identification
    ShapeType shape_type;
    std::vector<float> pos; // Now includes z-index
    std::vector<float> orientation;
    std::vector<float> scale;
    uint8_t r, g, b; // Color
    std::vector<Object> children={}; // Children objects

public:
    const int id;
    Object(std::vector<float> pos,std::vector<float> orientation,std::vector<float> scale,  uint8_t r, uint8_t g, uint8_t b, std::string name);
    ShapeType get_shape_type();
    ~Object() {};
    void move(float dx, float dy, float dz);
    void move_to(float x, float y, float z);

    std::vector<float> get_coords() ;
    std::array<uint8_t,3> get_color();
    void add_child(Object child) ;

    std::shared_ptr<std::vector<Object>> get_children();
    std::string get_name() ;// Get the name of the object
    bool in_frame=true; // Flag to indicate if the object is in the frame

};

#endif // SHAPE_H

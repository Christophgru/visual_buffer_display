#ifndef CAMERA_H
#define CAMERA_H

#include <vector>
class Camera{
private:
    /* data */
public:
    std::vector<float> pos; // Now includes z-index
    std::vector<float> orientation;
    std::vector<float> velocity={0.0f,0.0f,0.0f}; // Velocity vector
    //1000 pixels = 1 unit
    float sensorwidth=36.0f;
    float sensorheight=24.0f;
    float fov_width_deg;
    float fov_height_deg;
    float zoom;
    Camera(std::vector<float> pos,std::vector<float> orientation,float zoom);
    ~Camera();
};
#endif // CAMERA_H


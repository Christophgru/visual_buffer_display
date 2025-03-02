#include "camera.h"
#include <cmath>
Camera::Camera(std::vector<float> pos,std::vector<float> orientation,float zoom)
    : pos(pos),orientation(orientation),zoom(zoom)
{
    fov_width_deg=2*std::atan(sensorwidth/(2*zoom*1000))*180/3.14159265359;
    fov_height_deg=2*std::atan(sensorheight/(2*zoom*1000))*180/3.14159265359;
}

Camera::~Camera()
{
}

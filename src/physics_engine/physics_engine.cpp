#include "physics_engine.h"
#include <iostream>


struct Vector3 {
    float x, y, z;
};

struct Matrix3 {
    float data[3][3];

    // Matrix multiplication (dot product of rows and columns)
    Matrix3 operator*(const Matrix3& other) const {
        Matrix3 result = {};  // Initialize all elements to zero
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                result.data[i][j] = 0.0f;
                for (int k = 0; k < 3; k++) {
                    result.data[i][j] += data[i][k] * other.data[k][j];
                }
            }
        }
        return result;
    }
};

// Dot product function for two 3-element vectors
float dotProduct(const float a[3], const float b[3]) {
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

// Function to multiply a 3x3 matrix by a 3D vector
Vector3 multiplyMatrixVector(const Matrix3& m, const Vector3& v) {
    Vector3 result;
    result.x = m.data[0][0]*v.x + m.data[0][1]*v.y + m.data[0][2]*v.z;
    result.y = m.data[1][0]*v.x + m.data[1][1]*v.y + m.data[1][2]*v.z;
    result.z = m.data[2][0]*v.x + m.data[2][1]*v.y + m.data[2][2]*v.z;
    return result;
}


// Creates a rotation matrix from Euler angles (in radians)
// The rotation order is: first pitch (X-axis), then yaw (Y-axis), then roll (Z-axis)
Matrix3 create_rot_matrix(float pitch, float yaw, float roll) {
    Matrix3 rx, ry, rz;

    // Rotation about X-axis (pitch)
    rx.data[0][0] = 1.0f; rx.data[0][1] = 0.0f;          rx.data[0][2] = 0.0f;
    rx.data[1][0] = 0.0f; rx.data[1][1] = cos(pitch);      rx.data[1][2] = -sin(pitch);
    rx.data[2][0] = 0.0f; rx.data[2][1] = sin(pitch);      rx.data[2][2] = cos(pitch);

    // Rotation about Y-axis (yaw)
    ry.data[0][0] = cos(yaw);  ry.data[0][1] = 0.0f; ry.data[0][2] = sin(yaw);
    ry.data[1][0] = 0.0f;      ry.data[1][1] = 1.0f; ry.data[1][2] = 0.0f;
    ry.data[2][0] = -sin(yaw); ry.data[2][1] = 0.0f; ry.data[2][2] = cos(yaw);

    // Rotation about Z-axis (roll)
    rz.data[0][0] = cos(roll); rz.data[0][1] = -sin(roll); rz.data[0][2] = 0.0f;
    rz.data[1][0] = sin(roll); rz.data[1][1] = cos(roll);  rz.data[1][2] = 0.0f;
    rz.data[2][0] = 0.0f;      rz.data[2][1] = 0.0f;       rz.data[2][2] = 1.0f;

    // Combine the rotations.
    // Note: The order of multiplication matters. Here we perform:
    // result = rz * (ry * rx)
    return rz * (ry * rx);
}



void PhysicsEngine::update() {
    try
    {
        auto time= SDL_GetTicks();
        auto deltaTime = (time - lastMoveTime) / 1000.0f*20.0f; // Time in seconds
        lastMoveTime = time;
        camera->orientation={camera->orientation.at(0),camera->orientation.at(1),camera->orientation.at(2)};
        for (auto shape : *shapes) {
            if(shape->get_shape_type()==VERTEX){
                shape->move(0, -deltaTime,0);
                if(shape->get_coords().at(1)<0){
                    shape->move_to(shape->get_coords().at(0), 100.0f,shape->get_coords().at(2));
                }
            }else{
                shape->move(5*deltaTime, 5*deltaTime,0);
            };
            auto pos = shape->get_coords();
            if (pos.at(0) > renderer.getWindowWidth()) {
                shape->move_to(0, pos.at(1), pos.at(2));
            }
            if (pos.at(1) > renderer.getWindowHeight()) {
                shape->move_to(pos.at(0), 0, pos.at(2));
            }
            
        }
        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    
    }

    std::tuple<detected_actions,std::vector<int>> PhysicsEngine::handleEvent(SDL_Event event) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
            printf("terminate detected\n");
                return {terminate,{0}};
                break;
            case SDL_EVENT_WINDOW_RESIZED:
                printf("window resize detected\n");
                renderer.resize(event.window.data1, event.window.data2);
                glViewport(0, 0, event.window.data1, event.window.data2); // Update the OpenGL viewport on resize
                return {window_resize,{event.window.data1,event.window.data2}};
                break;
            case SDL_EVENT_KEY_DOWN:
                // Example: WASD movement
                if (event.key.scancode == SDL_SCANCODE_W) {
                    // Move forward or update camera position
                    printf("w detected\n");
                } else if (event.key.scancode == SDL_SCANCODE_A) {
                    // Move left
                    printf("a detected\n");
                } else if (event.key.scancode == SDL_SCANCODE_S) {
                    // Move backward
                    printf("s detected\n");
                } else if (event.key.scancode == SDL_SCANCODE_D) {
                    // Move right
                    printf("d detected\n");	
                }
                break;
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    // Handle left mouse click (e.g., selecting an object)
                    printf("left mouseclick down detected\n");
                    printf("first coords: %f %f \n",event.motion.x,event.motion.y);
                    mouse_movement={event.motion.x,event.motion.y};
                    mouse_clicked=true;
                }
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    // Handle left mouse click (e.g., selecting an object)
                    printf("left mouseclick up detected\n");
                    printf("final coords: %f %f \n",event.motion.x,event.motion.y);
                    mouse_clicked=false;
                    mouse_movement={0.0,0.0};
                }
                break;
            case SDL_EVENT_MOUSE_MOTION:
                // event.motion.x and event.motion.y for absolute position,
                // event.motion.xrel and event.motion.yrel for relative movement
                // Update camera orientation or object interaction here.
                if(mouse_clicked){
                            float diff_x=event.motion.x-std::get<0>(mouse_movement);
                            float diff_y=event.motion.y-std::get<1>(mouse_movement);
                            printf("difference= %f %f \n",diff_x,diff_y);
                            printf("orientation_before: %f %f %f \n",camera->orientation[0],camera->orientation[1],camera->orientation[2]);
                            int display_width,display_height;
                            display_width=std::get<0>(display_dimensions);
                            display_height=std::get<1>(display_dimensions);
                            printf("window size: %d %d \n",display_width,display_height);
                            diff_x=diff_x/display_width;
                            diff_y=diff_y/display_height;
                            printf("difference= %f %f \n",diff_x,diff_y);
                            Vector3 orientation={ camera->orientation[0],
                                camera->orientation[1],
                                camera->orientation[2] };
                            Matrix3 rot_matrix = create_rot_matrix(diff_x, diff_y, 0.0f);
                            Vector3 new_orientation = multiplyMatrixVector(rot_matrix, orientation);

                            camera->orientation={new_orientation.x,new_orientation.y,new_orientation.z};
                            printf("orientation_after: %f %f %f \n",camera->orientation[0],camera->orientation[1],camera->orientation[2]);
                            
                }
                break;
            // Add more cases as needed to handle other SDL events.
            default:
                printf("unknown input detected\n");
                break;
        }
        
        return {no_action,{0}};
    }
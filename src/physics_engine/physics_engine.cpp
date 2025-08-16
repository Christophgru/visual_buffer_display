#include "physics_engine.h"
#include <iostream>
#include <math/own_math.h>
constexpr double pi = 3.14159265358979323846;



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
    try{   
        auto time= SDL_GetTicks();
        float deltaTime = (time - lastMoveTime) / 1000.0f*20.0f; // Time in seconds
        lastMoveTime = time;
        //float camera_decceleration=0.5f;
        //printf("camera pos: %f %f %f \n",camera->pos.at(0),camera->pos.at(1),camera->pos.at(2));
        //camera->pos={camera->pos.at(0)+deltaTime*camera->velocity.at(0),camera->pos.at(1),camera->pos.at(2)};
        //float camera_decceleration_resulting=(1-1/pow((deltaTime*camera_decceleration+1.0f),2.0f));
        //camera->velocity={camera->velocity.at(0)*camera_decceleration_resulting,camera->velocity.at(1)*camera_decceleration_resulting,camera->velocity.at(2)*camera_decceleration_resulting};
        for (auto shape : *shapes) {
            // If shape is a vertex, update the position until its behind origin, then reset it to 100
            if(shape->get_shape_type()==VERTEX){
              //use strcmp  if(shape->get_name()=="moving_over"){
              if(shape->get_name().find("moving_over") != std::string::npos) {
             
                shape->move(0, -deltaTime,0);
                if(shape->get_coords().at(1)<0){
                    shape->move_to(shape->get_coords().at(0), 100.0f,shape->get_coords().at(2));
                }}
            }else{
                shape->move(5*deltaTime, 5*deltaTime,0);
            };
            auto pos = shape->get_coords();
            // check if pos is behind the camera
            // If the point is behind the camera, make it invisible
            
            // Update the renderer with the new position of the shape
            
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
                    // Move forward or update camera position relative to caymera orientation
                    printf("w detected\n");
                    auto new_position= calculate_new_position(camera->pos,camera->orientation,{0,1,0},1.0f);
                   
                    camera->pos =new_position;
                    break;
                } else if (event.key.scancode == SDL_SCANCODE_A) {
                    // Move left
                    printf("a detected\n");
                    auto new_position= calculate_new_position(camera->pos,camera->orientation,{-1,0,0},1.0f);
                   
                    camera->pos =new_position;
                } else if (event.key.scancode == SDL_SCANCODE_S) {
                    // Move backward
                    printf("s detected\n");
                    auto new_position= calculate_new_position(camera->pos,camera->orientation,{0,-1,0},1.0f);
                   
                    camera->pos =new_position;
                } else if (event.key.scancode == SDL_SCANCODE_D) {
                    // Move right
                    printf("d detected\n");	
                    auto new_position= calculate_new_position(camera->pos,camera->orientation,{1,0,0},1.0f);
                   
                    camera->pos =new_position;
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
                    printf("camera orientation: %f %f %f \n",camera->orientation.at(0),camera->orientation.at(1),camera->orientation.at(2));
                    float sensityfity=3.0f; // Sensitivity factor for mouse movement
                    float diff_x=event.motion.x-std::get<0>(mouse_movement);
                    float diff_y=event.motion.y-std::get<1>(mouse_movement);
                    printf("difference= %f %f \n",diff_x,diff_y);
                    printf("orientation_before: %f %f %f \n",camera->orientation[0],camera->orientation[1],camera->orientation[2]);
                    int display_width,display_height;
                    display_width=std::get<0>(display_dimensions);
                    display_height=std::get<1>(display_dimensions);
                    printf("window size: %d %d \n",display_width,display_height);
                    diff_x=diff_x/display_width*sensityfity/2.0f*pi;
                    diff_y=-diff_y/display_height*sensityfity/2.0f*pi;
                    printf("difference= %f %f \n",diff_x,diff_y);
                    Vector3 orientation={ camera->orientation[0],
                    camera->orientation[1],
                    camera->orientation[2] };
                    Matrix3 rot_matrix = create_rot_matrix(diff_y, 0.0f, diff_x);
                    Vector3 new_orientation = multiplyMatrixVector(rot_matrix, orientation);

                    camera->orientation={new_orientation.x,new_orientation.y,new_orientation.z};
                    printf("orientation_after: %f %f %f \n",camera->orientation[0],camera->orientation[1],camera->orientation[2]);
                    mouse_movement={event.motion.x,event.motion.y};
                            
                }
                break;
            // Add more cases as needed to handle other SDL events.
            default:
                printf("unknown input detected\n");
                break;
        }
        
        return {no_action,{0}};
    }

    std::vector<float> PhysicsEngine::calculate_new_position(std::vector<float> pos, std::vector<float> orientation, std::vector<float> direction, float speed) {
        // Calculate the new position based on the current position, orientation, direction, and speed
        //get resulting delta dist from orientation and direction
        float heading = atan2(orientation[1], orientation[0]); // Calculate the heading angle in radians
        // Calculate the new direction based on the orientation
        
        float delta_direction_x =  direction[0] * sin(heading) - direction[1] * cos(heading);
        float delta_direction_y =  direction[0] * cos(heading) + direction[1] * sin(heading);
        float new_x = pos[0] + delta_direction_x * speed;
        float new_y = pos[1] + delta_direction_y * speed; 
        float new_z = pos[2]; // Assuming no change in z-axis for simplicity

        return {new_x, new_y, new_z};
    }
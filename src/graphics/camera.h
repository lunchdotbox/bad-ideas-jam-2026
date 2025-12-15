#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>
#include "window.h"

typedef struct CameraPushConstant {
    mat4 view_matrix, model_matrix;
} CameraPushConstant;

typedef struct Camera {
    double last_mouse_x, last_mouse_y;
    vec3 position, rotation;
    mat4 projection;
} Camera;

Camera createCamera();
CameraPushConstant getCameraPush(Camera camera);
void updateCamera(Camera* camera, Window window);

#endif

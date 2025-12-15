#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

#include <elc/core.h>
#include "graphics/model.h"

typedef struct BoundingBox {
    vec3 max, min;
} BoundingBox;

BoundingBox boundingBoxCube1x1x1();
void boundingBoxTranslate(BoundingBox* bounding_box, vec3 vector);
bool boundingBoxOverlapping(BoundingBox bounding_box, BoundingBox other);
void boundingBoxVertices(BoundingBox self, VulkanVertex* vertices);
void boundingBoxIndices(BoundingBox bounding_box, u32* indices);
Model boundingBoxModel(Device device, BoundingBox bounding_box);

#endif

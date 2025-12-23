#ifndef MESH_INERTIA_H
#define MESH_INERTIA_H

#include "../formats/wavefront.h"
#include <cglm/mat3.h>
#include <cglm/util.h>
#include <cglm/vec3.h>

void computeMeshInertia(HostMesh mesh, float density, mat3 inertia, vec3 com, float* m);

#endif

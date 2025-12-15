#include "bounding_box.h"

#include "graphics/model.h"

BoundingBox boundingBoxCube1x1x1() {
    return (BoundingBox){
        .max = {0.5f, 0.5f, 0.5f},
        .min = {-0.5f, -0.5f, -0.5f},
    };
}

void boundingBoxTranslate(BoundingBox* bounding_box, vec3 vector) {
    glm_vec3_add(bounding_box->max, vector, bounding_box->max);
    glm_vec3_add(bounding_box->min, vector, bounding_box->min);
}

bool boundingBoxOverlapping(BoundingBox bounding_box, BoundingBox other) {
    return bounding_box.min[0] <= other.max[0] &&
        bounding_box.max[0] >= other.min[0] &&
        bounding_box.min[1] <= other.max[1] &&
        bounding_box.max[1] >= other.min[1] &&
        bounding_box.min[2] <= other.max[2] &&
        bounding_box.max[2] >= other.min[2];
}

void boundingBoxVertices(BoundingBox self, VulkanVertex* vertices) {
    vec3 normals[] = {
        { -1.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f },
        { 0.0f, -1.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, -1.0f },
    };

    vec2 quad_uvs[] = {
        { 0.0f, 0.0f },
        { 1.0f, 1.0f },
        { 0.0f, 1.0f },
        { 1.0f, 0.0f },
    };

    vertices[0] = (VulkanVertex){ .position = { self.min[0], self.min[1], self.min[2] }, .normal = { normals[0][0], normals[0][1], normals[0][2] }, .uv = { quad_uvs[0][0], quad_uvs[0][1] } };
    vertices[1] = (VulkanVertex){ .position = { self.min[0], self.max[1], self.max[2] }, .normal = { normals[0][0], normals[0][1], normals[0][2] }, .uv = { quad_uvs[1][0], quad_uvs[1][1] } };
    vertices[2] = (VulkanVertex){ .position = { self.min[0], self.min[1], self.max[2] }, .normal = { normals[0][0], normals[0][1], normals[0][2] }, .uv = { quad_uvs[2][0], quad_uvs[2][1] } };
    vertices[3] = (VulkanVertex){ .position = { self.min[0], self.max[1], self.min[2] }, .normal = { normals[0][0], normals[0][1], normals[0][2] }, .uv = { quad_uvs[3][0], quad_uvs[3][1] } };

    vertices[4] = (VulkanVertex){ .position = { self.max[0], self.min[1], self.min[2] }, .normal = { normals[1][0], normals[1][1], normals[1][2] }, .uv = { quad_uvs[0][0], quad_uvs[0][1] } };
    vertices[5] = (VulkanVertex){ .position = { self.max[0], self.max[1], self.max[2] }, .normal = { normals[1][0], normals[1][1], normals[1][2] }, .uv = { quad_uvs[1][0], quad_uvs[1][1] } };
    vertices[6] = (VulkanVertex){ .position = { self.max[0], self.min[1], self.max[2] }, .normal = { normals[1][0], normals[1][1], normals[1][2] }, .uv = { quad_uvs[2][0], quad_uvs[2][1] } };
    vertices[7] = (VulkanVertex){ .position = { self.max[0], self.max[1], self.min[2] }, .normal = { normals[1][0], normals[1][1], normals[1][2] }, .uv = { quad_uvs[3][0], quad_uvs[3][1] } };

    vertices[8] = (VulkanVertex){ .position = { self.min[0], self.min[1], self.min[2] }, .normal = { normals[2][0], normals[2][1], normals[2][2] }, .uv = { quad_uvs[0][0], quad_uvs[0][1] } };
    vertices[9] = (VulkanVertex){ .position = { self.max[0], self.min[1], self.max[2] }, .normal = { normals[2][0], normals[2][1], normals[2][2] }, .uv = { quad_uvs[1][0], quad_uvs[1][1] } };
    vertices[10] = (VulkanVertex){ .position = { self.min[0], self.min[1], self.max[2] }, .normal = { normals[2][0], normals[2][1], normals[2][2] }, .uv = { quad_uvs[2][0], quad_uvs[2][1] } };
    vertices[11] = (VulkanVertex){ .position = { self.max[0], self.min[1], self.min[2] }, .normal = { normals[2][0], normals[2][1], normals[2][2] }, .uv = { quad_uvs[3][0], quad_uvs[3][1] } };

    vertices[12] = (VulkanVertex){ .position = { self.min[0], self.max[1], self.min[2] }, .normal = { normals[3][0], normals[3][1], normals[3][2] }, .uv = { quad_uvs[0][0], quad_uvs[0][1] } };
    vertices[13] = (VulkanVertex){ .position = { self.max[0], self.max[1], self.max[2] }, .normal = { normals[3][0], normals[3][1], normals[3][2] }, .uv = { quad_uvs[1][0], quad_uvs[1][1] } };
    vertices[14] = (VulkanVertex){ .position = { self.min[0], self.max[1], self.max[2] }, .normal = { normals[3][0], normals[3][1], normals[3][2] }, .uv = { quad_uvs[2][0], quad_uvs[2][1] } };
    vertices[15] = (VulkanVertex){ .position = { self.max[0], self.max[1], self.min[2] }, .normal = { normals[3][0], normals[3][1], normals[3][2] }, .uv = { quad_uvs[3][0], quad_uvs[3][1] } };

    vertices[16] = (VulkanVertex){ .position = { self.min[0], self.min[1], self.max[2] }, .normal = { normals[4][0], normals[4][1], normals[4][2] }, .uv = { quad_uvs[0][0], quad_uvs[0][1] } };
    vertices[17] = (VulkanVertex){ .position = { self.max[0], self.max[1], self.max[2] }, .normal = { normals[4][0], normals[4][1], normals[4][2] }, .uv = { quad_uvs[1][0], quad_uvs[1][1] } };
    vertices[18] = (VulkanVertex){ .position = { self.min[0], self.max[1], self.max[2] }, .normal = { normals[4][0], normals[4][1], normals[4][2] }, .uv = { quad_uvs[2][0], quad_uvs[2][1] } };
    vertices[19] = (VulkanVertex){ .position = { self.max[0], self.min[1], self.max[2] }, .normal = { normals[4][0], normals[4][1], normals[4][2] }, .uv = { quad_uvs[3][0], quad_uvs[3][1] } };

    vertices[20] = (VulkanVertex){ .position = { self.min[0], self.min[1], self.min[2] }, .normal = { normals[5][0], normals[5][1], normals[5][2] }, .uv = { quad_uvs[0][0], quad_uvs[0][1] } };
    vertices[21] = (VulkanVertex){ .position = { self.max[0], self.max[1], self.min[2] }, .normal = { normals[5][0], normals[5][1], normals[5][2] }, .uv = { quad_uvs[1][0], quad_uvs[1][1] } };
    vertices[22] = (VulkanVertex){ .position = { self.min[0], self.max[1], self.min[2] }, .normal = { normals[5][0], normals[5][1], normals[5][2] }, .uv = { quad_uvs[2][0], quad_uvs[2][1] } };
    vertices[23] = (VulkanVertex){ .position = { self.max[0], self.min[1], self.min[2] }, .normal = { normals[5][0], normals[5][1], normals[5][2] }, .uv = { quad_uvs[3][0], quad_uvs[3][1] } };
}

void boundingBoxIndices(BoundingBox bounding_box, u32* indices) {
    memcpy(indices, (u32[]){0, 1, 2, 0, 3, 1, 4, 5, 6, 4, 7, 5, 8, 9, 10, 8, 11, 9, 12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21}, sizeof(u32) * 36);
}

Model boundingBoxModel(Device device, BoundingBox bounding_box) {
    void* vertex_mapped, *index_mapped;
    Model model = createModel(device, sizeof(VulkanVertex), 24, 36, &vertex_mapped, &index_mapped);

    boundingBoxVertices(bounding_box, vertex_mapped);
    boundingBoxIndices(bounding_box, index_mapped);

    finishModelUpload(device, &model, sizeof(VulkanVertex));

    return model;
}

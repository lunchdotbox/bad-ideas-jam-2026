#define GLM_FORCE_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_RADIANS 1

#include "graphics/instance.h"
#include "graphics/device.h"
#include "graphics/window.h"
#include "graphics/graphics_pipeline.h"
#include "graphics/model.h"
#include "graphics/texture.h"
#include "graphics/descriptor.h"
#include "graphics/camera.h"

#include "bounding_box.h"

int main() {
    glfwInit();
    VkInstance instance = createInstance();
    Device device = createDevice(instance);
    Window window = createWindow(device, instance, 800, 600, "vulkan renderer");

    glfwSetInputMode(window.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    PipelineConfig pipeline_config;
    MODEL_PIPELINE_CONFIG(pipeline_config);
    setPipelineWindow(&pipeline_config, window);
    setPipelineVertexShader(&pipeline_config, createShaderModule(device, "spv/terrain.vert.spv"));
    setPipelineFragmentShader(&pipeline_config, createShaderModule(device, "spv/diffuse.frag.spv"));
    VkPipeline pipeline = createPipeline(device, pipeline_config);

    Model model = boundingBoxModel(device, boundingBoxCube1x1x1());

    Texture texture = loadTexture(device, &window.device_loop, QUEUE_TYPE_GRAPHICS, "images/cat.png");
    addDescriptorTexture(device, &window.device_loop, texture);

    Camera camera = createCamera();

    while (!glfwWindowShouldClose(window.window)) {
        updateCamera(&camera, window);

        glfwPollEvents();

        u32 image = beginWindowFrame(&window, device, (VkClearColorValue){0, 20, 255, 0});

        CameraPushConstant push = getCameraPush(camera);

        vkCmdBindPipeline(currentCommand(window), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        vkCmdBindVertexBuffers(currentCommand(window), 0, 1, &model.vertex_buffer, (u64[]){0});
        vkCmdBindIndexBuffer(currentCommand(window), model.index_buffer, 0, VK_INDEX_TYPE_UINT32);
        vkCmdPushConstants(currentCommand(window), device.pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, 128, &push);
        vkCmdDrawIndexed(currentCommand(window), 36, 1, 0, 0, 0);

        endWindowFrame(&window, device, image);
    }

    vkDeviceWaitIdle(device.logical);

    destroyTexture(device, texture);
    destroyModel(device, model);
    vkDestroyPipeline(device.logical, pipeline, NULL);
    destroyWindow(window, device, instance);
    destroyDevice(device, instance);
    vkDestroyInstance(instance, NULL);
    glfwTerminate();
    return 0;
}

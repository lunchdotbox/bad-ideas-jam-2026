#include "graphics/descriptor.h"
#include <vulkan/vulkan_core.h>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_RADIANS 1

#include "graphics/instance.h"
#include "graphics/device.h"
#include "graphics/window.h"
#include "graphics/graphics_pipeline.h"
#include "graphics/model.h"
#include "graphics/texture.h"
#include "graphics/camera.h"

int main() {
    glfwInit();
    VkInstance instance = createInstance();
    Device device = createDevice(instance);
    Window window = createWindow(device, instance, 800, 600, "vulkan renderer");

    PipelineConfig pipeline_config;
    MODEL_PIPELINE_CONFIG(pipeline_config);
    pipeline_config.attributes = NULL;
    pipeline_config.n_attributes = 0;
    pipeline_config.bindings = NULL;
    pipeline_config.n_bindings = 0;
    pipeline_config.color_attachments->blendEnable = VK_TRUE;
    pipeline_config.color_attachments->srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    pipeline_config.color_attachments->dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    pipeline_config.color_attachments->colorBlendOp = VK_BLEND_OP_ADD;
    setPipelineWindow(&pipeline_config, window);
    setPipelineVertexShader(&pipeline_config, createShaderModule(device, "spv/text.vert.spv"));
    setPipelineFragmentShader(&pipeline_config, createShaderModule(device, "spv/text.frag.spv"));
    VkPipeline pipeline = createPipeline(device, pipeline_config);

    Texture font_atlas = loadTexture(device, &window.device_loop, QUEUE_TYPE_GRAPHICS, "images/fonts/minogram_6x10.png");
    u32 font_id = addDescriptorTexture(device, &window.device_loop, 0, font_atlas);

    while (!glfwWindowShouldClose(window.window)) {
        glfwPollEvents();

        u32 image = beginWindowFrame(&window, device, (VkClearColorValue){0, 20, 255, 0});

        vkCmdBindPipeline(currentCommand(window), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        vkCmdPushConstants(currentCommand(window), device.pipeline_layout, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(u32), &font_id);
        vkCmdDraw(currentCommand(window), 6, 1, 0, 0);

        endWindowFrame(&window, device, image);
    }

    vkDeviceWaitIdle(device.logical);

    vkDestroyPipeline(device.logical, pipeline, NULL);
    destroyWindow(window, device, instance);
    destroyDevice(device, instance);
    vkDestroyInstance(instance, NULL);
    glfwTerminate();
    return 0;
}

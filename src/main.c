#include "graphics/buffer.h"
#include "graphics/descriptor.h"
#include <GLFW/glfw3.h>
#include <cglm/affine2d.h>
#include <cglm/types.h>
#include <elc/core.h>
#include <vulkan/vulkan_core.h>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_RADIANS 1

#include "graphics/instance.h"
#include "graphics/device.h"
#include "graphics/window.h"
#include "graphics/graphics_pipeline.h"
#include "graphics/model.h"
#include "graphics/texture.h"
#include "graphics/text.h"
#include "graphics/render_target.h"

int main() {
    glfwInit();
    VkInstance instance = createInstance();
    Device device = createDevice(instance);
    Window window = createWindow(device, instance, 800, 600, "vulkan renderer");

    TextRenderer text_renderer = createTextRenderer(device, windowPipelineConfig(window));
    TextFont text_font = createTextFont(device, &window.device_loop, ELC_KILOBYTE);

    mat3 transform;
    glm_rotate2d_make(transform, GLM_PI / 8);
    glm_scale2d(transform, (vec2){0.5f / 13.0f, 0.5f / 7.0f});

    while (!glfwWindowShouldClose(window.window)) {
        glfwPollEvents();

        u32 image = beginWindowFrame(&window, device);
        beginRenderPass(window.render_pass, window.framebuffers[image], window.extent, currentCommand(window), &(VkClearColorValue){0, 20, 255, 0}, 1.0f);

        addFontText(&text_font, transform, "hi there");
        drawTextFont(currentCommand(window), device, text_renderer, &text_font);

        endWindowFrame(&window, device, image);
    }

    vkDeviceWaitIdle(device.logical);

    destroyTextFont(device, text_font);
    destroyTextRenderer(device, text_renderer);
    destroyWindow(window, device, instance);
    destroyDevice(device, instance);
    vkDestroyInstance(instance, NULL);
    glfwTerminate();
    return 0;
}

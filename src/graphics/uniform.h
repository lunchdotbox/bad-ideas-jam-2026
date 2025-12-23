#ifndef UNIFORM_H
#define UNIFORM_H

#include "device.h"
#include "device_loop.h"
#include <elc/core.h>
#include <vulkan/vulkan_core.h>

typedef struct UniformBuffer {
    VkDeviceMemory memory;
    VkBuffer buffers[FRAMES_IN_FLIGHT]; // TODO: make a macro to generate structs with multiple buffers per memory
    void* mapped;
} UniformBuffer;

UniformBuffer createUniformBuffer(Device device, VkDeviceSize size);
void destroyUniformBuffer(Device device, UniformBuffer uniform);
void* getUniformMapped(UniformBuffer uniform, VkDeviceSize size, u32 frame);
void flushUniformBuffer(Device device, UniformBuffer uniform, VkDeviceSize size, u32 frame);

#endif

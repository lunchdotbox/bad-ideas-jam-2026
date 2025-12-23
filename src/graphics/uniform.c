#include "uniform.h"

#include "buffer.h"

UniformBuffer createUniformBuffer(Device device, VkDeviceSize size) {
    UniformBuffer uniform;
    for (u32 i = 0; i < FRAMES_IN_FLIGHT; i++) uniform.buffers[i] = createBuffer(device, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, false);
    uniform.memory = createBuffersMemory(device, uniform.buffers, FRAMES_IN_FLIGHT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    uniform.mapped = mapDeviceMemory(device, uniform.memory, 0, size * FRAMES_IN_FLIGHT);
    return uniform;
}

void destroyUniformBuffer(Device device, UniformBuffer uniform) {
    vkUnmapMemory(device.logical, uniform.memory);
    vkFreeMemory(device.logical, uniform.memory, NULL);
    for (u32 i = 0; i < FRAMES_IN_FLIGHT; i++) vkDestroyBuffer(device.logical, uniform.buffers[i], NULL);
}

void* getUniformMapped(UniformBuffer uniform, VkDeviceSize size, u32 frame) {
    return uniform.mapped + (size * frame);
}

void flushUniformBuffer(Device device, UniformBuffer uniform, VkDeviceSize size, u32 frame) {
    flushDeviceMemory(device, uniform.memory, size * frame, size);
}

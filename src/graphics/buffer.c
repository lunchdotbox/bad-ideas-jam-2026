#include "buffer.h"

#include "command.h"

VkBuffer createBuffer(Device device, VkDeviceSize size, VkBufferUsageFlags usage, bool concurrent) {
    VkBufferCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = usage,
        .sharingMode = concurrent ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE,
    };

    VkBuffer buffer;
    vkCreateBuffer(device.logical, &create_info, NULL, &buffer);

    return buffer;
}

VkDeviceMemory createBufferMemory(Device device, VkBuffer buffer, VkMemoryPropertyFlags properties) {
    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(device.logical, buffer, &requirements);

    VkDeviceMemory memory = allocateDeviceMemory(device, properties, requirements);
    vkBindBufferMemory(device.logical, buffer, memory, 0);

    return memory;
}

void copyBuffer(Device device, QueueType type, VkBuffer src_buffer, VkBuffer dst_buffer, u64 copy_size) {
    VkCommandBuffer command = beginSingleTimeCommands(device, type);

    commandCopyBuffer(command, src_buffer, dst_buffer, copy_size);

    endSingleTimeCommands(device, type, command);
}

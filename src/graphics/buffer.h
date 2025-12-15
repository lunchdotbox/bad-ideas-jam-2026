#ifndef BUFFER_H
#define BUFFER_H

#include "device.h"

VkBuffer createBuffer(Device device, VkDeviceSize size, VkBufferUsageFlags usage, bool concurrent);
VkDeviceMemory createBufferMemory(Device device, VkBuffer buffer, VkMemoryPropertyFlags properties);
void copyBuffer(Device device, QueueType type, VkBuffer src_buffer, VkBuffer dst_buffer, u64 copy_size);

#endif

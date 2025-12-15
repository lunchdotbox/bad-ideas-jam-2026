#ifndef SYNCHRONIZATION_H
#define SYNCHRONIZATION_H

#include "device.h"

VkSemaphore createSemaphore(Device device);
VkFence createFence(Device device);

#endif

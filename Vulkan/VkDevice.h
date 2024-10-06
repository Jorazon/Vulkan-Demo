#pragma once

#include "VkFunctionPointers.h"

typedef struct {
  uint32_t graphicsQueueFamilyIndex; // Index of queue family supporting graphics
  uint32_t presentQueueFamilyIndex;  // Index of queue family supporting presentation
} QueueFamilyIndices;

VkResult GetPhysicalDevice(VkInstance instance, VkPhysicalDevice* physicalDevice);
VkResult GetLogicalDevice(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice* logicalDevice);
QueueFamilyIndices findQueueFamilies(VkInstance instance, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
void GetQueue(VkInstance instance, VkDevice device, uint32_t queueFamilyIndex, VkQueue* queue);

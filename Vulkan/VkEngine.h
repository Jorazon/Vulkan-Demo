#pragma once

#include "VkIncludes.h"

#include "VkGraphicsPipeline.h"
#include "VkSwapChain.h" 

typedef struct {
	VkInstance _instance;//                      Vulkan library handle
	VkSurfaceKHR _surface;//                     Vulkan window surface
	VkPhysicalDevice _chosenGPU;//               GPU chosen as the default device
	VkDevice _device; //                         Vulkan device for commands
	VkDebugUtilsMessengerEXT _debug_messenger;// Vulkan debug output handle
} VkEngine;

int CreateInstance(VkInstance* instance);
void clean(VkEngine engine, VkSwapChain swapChain, VkGraphicsPipeline pipeline);

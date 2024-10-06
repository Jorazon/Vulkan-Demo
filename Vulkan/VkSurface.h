#pragma once

#include "VkIncludes.h"

VkResult CreateVulkanSurface(VkInstance instance, void* windowHandle, void* platformHandle, VkSurfaceKHR* out_surface);

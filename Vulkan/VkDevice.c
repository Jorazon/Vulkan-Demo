#include <stdint.h>
#include <stdio.h>

#include "VkDevice.h"

VkResult GetPhysicalDevice(VkInstance instance, VkPhysicalDevice* physicalDevice) {

  PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices)vkGetInstanceProcAddr(instance, "vkEnumeratePhysicalDevices");
  if (!vkEnumeratePhysicalDevices) {
    fprintf(stderr, "Failed to get pointer to vkEnumeratePhysicalDevices\n");
  }
  PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceProperties");
  if (!vkGetPhysicalDeviceProperties) {
    fprintf(stderr, "Failed to get pointer to vkGetPhysicalDeviceProperties\n");
  }
  PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures = (PFN_vkGetPhysicalDeviceFeatures)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFeatures");
  if (!vkGetPhysicalDeviceFeatures) {
    fprintf(stderr, "Failed to get pointer to vkGetPhysicalDeviceFeatures\n");
  }

  if (!vkEnumeratePhysicalDevices || !vkGetPhysicalDeviceProperties || !vkGetPhysicalDeviceFeatures) {
    return VK_ERROR_UNKNOWN;
  }

  // Get the count of available physical devices
  uint32_t physicalDeviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, NULL);

  if (physicalDeviceCount == 0) {
    fprintf(stderr, "Failed to find GPUs with Vulkan support\n");
    return VK_ERROR_UNKNOWN;
  }

  VkPhysicalDevice* physicalDevices = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * physicalDeviceCount);

  if (!physicalDevices) {
    fprintf(stderr, "Failed to allocate memory for physical devices\n");
    return VK_ERROR_OUT_OF_HOST_MEMORY;
  }

  vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices);

  *physicalDevice = VK_NULL_HANDLE;

  fprintf(stdout, "Found the following GPUs with Vulkan support:\n");

  for (uint32_t i = 0; i < physicalDeviceCount; i++) {
    VkPhysicalDeviceProperties deviceProperties = {0};
    vkGetPhysicalDeviceProperties(physicalDevices[i], &deviceProperties);

    // Check if the device supports graphics
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(physicalDevices[i], &deviceFeatures);

    // Print device properties (currently name and driver version)
    // conversion stolen from https://github.com/SaschaWillems/vulkan.gpuinfo.org/blob/1e6ca6e3c0763daabd6a101b860ab4354a07f5d3/functions.php#L294
    // Vendor ID lookup https://pcisig.com/membership/member-companies

    uint32_t driverVersion = deviceProperties.driverVersion;
    uint32_t variant = 0, major = 0, minor = 0, patch = 0;

    switch (deviceProperties.vendorID) {
    case 0x10DEU: // NVIDIA
      variant = (driverVersion >> 22) & 0x3ff;
      major = (driverVersion >> 14) & 0x0ff;
      minor = (driverVersion >> 6) & 0x0ff;
      patch = (driverVersion) & 0x003f;
      break;
    case 0x8086U: // INTEL
      variant = (driverVersion >> 14);
      major = (driverVersion) & 0x3fff;
      break;
    case 0x1022U: // AMD
    default:
      variant = (driverVersion >> 22);
      major = (driverVersion >> 12) & 0x3ff;
      minor = driverVersion & 0xfff;
      break;
    }

    fprintf(stdout, "[%d] %s (%d.%d.%d.%d)\n", i, deviceProperties.deviceName, variant, major, minor, patch);

    // You can add more checks for specific properties and features here
    // Example check: discrete GPU with geometry shader support
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader) {
      *physicalDevice = physicalDevices[i];
    }
  }

  free(physicalDevices); // Free the list of physical devices

  if (*physicalDevice == VK_NULL_HANDLE) {
    fprintf(stderr, "Failed to find a suitable physical device\n");
    return VK_ERROR_UNKNOWN;
  }

  return VK_SUCCESS;
}

/*
typedef struct VkPhysicalDeviceProperties {
    uint32_t                            apiVersion;
    uint32_t                            driverVersion;
    uint32_t                            vendorID;
    uint32_t                            deviceID;
    VkPhysicalDeviceType                deviceType;
    char                                deviceName[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE];
    uint8_t                             pipelineCacheUUID[VK_UUID_SIZE];
    VkPhysicalDeviceLimits              limits;
    VkPhysicalDeviceSparseProperties    sparseProperties;
} VkPhysicalDeviceProperties;
*/

VkResult GetLogicalDevice(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice* logicalDevice) {
  PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceQueueFamilyProperties");
  if (!vkGetPhysicalDeviceQueueFamilyProperties) {
    fprintf(stderr, "Failed to get pointer to vkGetPhysicalDeviceQueueFamilyProperties\n");
    return VK_ERROR_UNKNOWN;
  }

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, NULL); // Get the number of queue families

  //TODO use findQueueFamilies
  VkQueueFamilyProperties* queueFamilies = malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);

  if (!queueFamilies) {
    fprintf(stderr, "Failed to allocate memory for queue families\n");
    return VK_ERROR_OUT_OF_HOST_MEMORY;
  }

  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies); // Get queue family properties

  // Find a queue family that supports VK_QUEUE_GRAPHICS_BIT
  int graphicsQueueFamilyIndex = -1;
  for (uint32_t i = 0; i < queueFamilyCount; i++) {
    if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      graphicsQueueFamilyIndex = i;
      break;
    }
  }
  free(queueFamilies);

  if (graphicsQueueFamilyIndex == -1) {
    fprintf(stderr, "Failed to find a queue family that supports graphics operations\n");
    return VK_ERROR_UNKNOWN;
  }

  // Specify the queue creation info for the logical device
  float queuePriority = 1.0f; // Queue priority must be between 0.0 and 1.0
  VkDeviceQueueCreateInfo queueCreateInfo = { 0 };
  queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
  queueCreateInfo.queueCount = 1; // Number of queues to create in this family
  queueCreateInfo.pQueuePriorities = &queuePriority; // Specify the priority

  // Specify the device features you want to enable (empty for now)
  VkPhysicalDeviceFeatures deviceFeatures = { 0 };

  // Specify the device creation info
  VkDeviceCreateInfo createInfo = { 0 };
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.pQueueCreateInfos = &queueCreateInfo; // Point to queue creation info
  createInfo.queueCreateInfoCount = 1;             // Number of queue creation infos
  createInfo.pEnabledFeatures = &deviceFeatures;   // Point to device features

  // Optional: specify device extensions
  const char* deviceExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME }; // For surface presentation
  createInfo.enabledExtensionCount = 1;
  createInfo.ppEnabledExtensionNames = deviceExtensions;

  // Create the logical device
  if (vkCreateDevice(physicalDevice, &createInfo, NULL, logicalDevice) != VK_SUCCESS) {
    fprintf(stderr, "Failed to create logical Vulkan device\n");
    return VK_ERROR_UNKNOWN;
  }

  fprintf(stdout, "Logical Vulkan device created succesfully\n");

  return VK_SUCCESS;
}

// Function to find queue family indices
QueueFamilyIndices findQueueFamilies(VkInstance instance, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
  QueueFamilyIndices indices = { -1, -1 };

  PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceQueueFamilyProperties");
  if (!vkGetPhysicalDeviceQueueFamilyProperties) {
    fprintf(stderr, "Failed to get pointer to vkGetPhysicalDeviceQueueFamilyProperties\n");
  }
  PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceSupportKHR");
  if (!vkGetPhysicalDeviceSurfaceSupportKHR) {
    fprintf(stderr, "Failed to get pointer to vkGetPhysicalDeviceSurfaceSupportKHR\n");
  }

  if (!vkGetPhysicalDeviceQueueFamilyProperties || !vkGetPhysicalDeviceSurfaceSupportKHR) {
    return indices;
  }

  // Get the number of queue families
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, NULL);

  // Get the properties of all queue families
  VkQueueFamilyProperties* queueFamilies = malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
  if (!queueFamilies) {
    fprintf(stderr, "Failed to allocate memory for present modes\n");
    // TODO refactor to return vkresult VK_ERROR_OUT_OF_HOST_MEMORY and output parameter for indecies
    return indices;
  }
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies);

  // Iterate through each queue family and check its capabilities
  for (uint32_t i = 0; i < queueFamilyCount; i++) {
    // Check if the queue family supports graphics operations
    if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsQueueFamilyIndex = i;
    }

    // Check if the queue family supports presentation to the given surface
    VkBool32 presentSupport = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
    if (presentSupport) {
      indices.presentQueueFamilyIndex = i;
    }

    // If we have found both a graphics and a present queue family, we can stop searching
    if (indices.graphicsQueueFamilyIndex != -1 && indices.presentQueueFamilyIndex != -1) {
      break;
    }
  }

  free(queueFamilies);
  return indices;
}

void GetQueue(VkInstance instance, VkDevice device, uint32_t queueFamilyIndex, VkQueue* queue) {
  PFN_vkGetDeviceQueue vkGetDeviceQueue = (PFN_vkGetDeviceQueue)vkGetInstanceProcAddr(instance, "vkGetDeviceQueue");
  if (!vkGetDeviceQueue) {
    fprintf(stderr, "Failed to get pointer to vkGetDeviceQueue\n");
    return;
  }

  vkGetDeviceQueue(device, queueFamilyIndex, 0, queue);
}

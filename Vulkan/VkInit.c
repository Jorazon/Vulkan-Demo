#include <stdio.h>

#include "VkFunctionPointers.h"
#include "VkEngine.h"
#include "VkGraphicsPipeline.h"
#include "MakeWindow.h"

// https://vkguide.dev/docs/new_chapter_1/vulkan_init_code/

const char* instanceExtensions[] = {
    VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef _WIN32
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif __linux__
    VK_KHR_XLIB_SURFACE_EXTENSION_NAME,  // Or VK_KHR_XCB_SURFACE_EXTENSION_NAME
#elif __APPLE__
    VK_MVK_MACOS_SURFACE_EXTENSION_NAME,  // Or VK_EXT_METAL_SURFACE_EXTENSION_NAME
#endif
};

#ifdef _WIN32
const char* LIBRARY_PATH = "vulkan-1.dll";
#endif
#ifdef __linux__
const char* LIBRARY_PATH = "vulkan-1.so";
#endif

// Cross-platform function to load shared library
void* load_library(const char* library_name) {
#ifdef _WIN32
  return (void*)LoadLibraryA(library_name);
#elif __linux__
  return dlopen(library_name, RTLD_NOW);
#else
  return NULL; // Unsupported platform
#endif
}

// Cross-platform function to get a function pointer from a shared library
void* load_function(void* library, const char* function_name) {
#ifdef _WIN32
  return (void*)GetProcAddress((HMODULE)library, function_name);
#elif __linux__
  return dlsym(library, function_name);
#else
  return NULL; // Unsupported platform
#endif
}

// Cross-platform function to unload shared library
void close_library(void* library) {
#ifdef _WIN32
  FreeLibrary(library);
#elif __linux__
  dlclose(library);
#else
  return -1; // Unsupported platform
#endif
}

PFN_vkCreateInstance vkCreateInstance = NULL;
PFN_vkDestroyInstance vkDestroyInstance = NULL;
PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = NULL;
PFN_vkCreateDevice vkCreateDevice = NULL;
PFN_vkDestroyDevice vkDestroyDevice = NULL;
PFN_vkDestroyPipeline vkDestroyPipeline = NULL;
PFN_vkDestroyPipelineLayout vkDestroyPipelineLayout = NULL;

void* vulkanLibrary;

int CreateInstance(VkInstance* instance) {
  //Load library
  vulkanLibrary = load_library(LIBRARY_PATH);

  // print on error
	if (!vulkanLibrary) {
    fprintf(stderr, "Failed to load %s\n", LIBRARY_PATH);
		return -1;
	}

  // Get the function pointers
  // TODO can this be extracted into a function?
  // Get the address of vkCreateInstance from the library
  vkCreateInstance = (PFN_vkCreateInstance)load_function(vulkanLibrary, "vkCreateInstance");
  if (!vkCreateInstance) {
    fprintf(stderr, "Failed to load vkCreateInstance from %s\n", LIBRARY_PATH);
    close_library(vulkanLibrary);
    return -1;
  }

  // Get the address of vkDestroyInstance from the library
  vkDestroyInstance = (PFN_vkDestroyInstance)load_function(vulkanLibrary, "vkDestroyInstance");
  if (!vkDestroyInstance) {
    fprintf(stderr, "Failed to load vkDestroyInstance from %s\n", LIBRARY_PATH);
    close_library(vulkanLibrary);
    return -1;
  }

  // Get the address of vkGetInstanceProcAddr from the library
  vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)load_function(vulkanLibrary, "vkGetInstanceProcAddr");
  if (!vkGetInstanceProcAddr) {
    fprintf(stderr, "Failed to load vkGetInstanceProcAddr from %s\n", LIBRARY_PATH);
    close_library(vulkanLibrary);
    return -1;
  }

  // Get the address of vkCreateDevice from the library
  vkCreateDevice = (PFN_vkCreateDevice)load_function(vulkanLibrary, "vkCreateDevice");
  if (!vkCreateDevice) {
    fprintf(stderr, "Failed to load vkCreateDevice from %s\n", LIBRARY_PATH);
    close_library(vulkanLibrary);
    return -1;
  }

  // Get the address of vkDestroyDevice from the library
  vkDestroyDevice = (PFN_vkDestroyDevice)load_function(vulkanLibrary, "vkDestroyDevice");
  if (!vkDestroyDevice) {
    fprintf(stderr, "Failed to load vkDestroyDevice from %s\n", LIBRARY_PATH);
    close_library(vulkanLibrary);
    return -1;
  }

  // Get the address of vkDestroyPipeline from the library
  vkDestroyPipeline = (PFN_vkDestroyPipeline)load_function(vulkanLibrary, "vkDestroyPipeline");
  if (!vkDestroyPipeline) {
    fprintf(stderr, "Failed to load vkDestroyPipeline from %s\n", LIBRARY_PATH);
    close_library(vulkanLibrary);
    return -1;
  }

  // Get the address of vkDestroyPipelineLayout from the library
  vkDestroyPipelineLayout = (PFN_vkDestroyPipelineLayout)load_function(vulkanLibrary, "vkDestroyPipelineLayout");
  if (!vkDestroyPipelineLayout) {
    fprintf(stderr, "Failed to load vkDestroyPipelineLayout from %s\n", LIBRARY_PATH);
    close_library(vulkanLibrary);
    return -1;
  }

  // Now create a Vulkan instance
  VkApplicationInfo appInfo = {0};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Vulkan Test App";
  appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
  appInfo.pEngineName = "Oskari's Super Cool Custom Engine (WIP)";
  appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
  appInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 0, 0); // patch (last) always 0

  VkInstanceCreateInfo createInfo = {0};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;
  createInfo.enabledExtensionCount = sizeof(instanceExtensions) / sizeof(instanceExtensions[0]);
  createInfo.ppEnabledExtensionNames = instanceExtensions;

  VkResult result = vkCreateInstance(&createInfo, NULL, instance);

  if (result != VK_SUCCESS) {
    fprintf(stderr, "Failed to create Vulkan instance. Error code: %d\n", result);
    close_library(vulkanLibrary);
    return -1;
  }

  fprintf(stdout, "Vulkan instance created successfully\n");

  return 0;
}

// Clean up vulkan instance and free library
void clean(VkEngine engine, VkSwapChain swapChain, VkGraphicsPipeline pipeline) {
  //vkDestroySemaphore(device, imageAvailableSemaphore, NULL);
  //vkDestroyFence(device, renderFence, NULL);
  //vkDestroyCommandPool(device, commandPool, NULL);
  vkDestroyPipeline(engine._device, pipeline.graphicsPipeline, NULL);
  vkDestroyPipelineLayout(engine._device, pipeline.pipelineLayout, NULL);
  // destroy vulkan instance
  vkDestroyDevice(engine._device, NULL);
  vkDestroyInstance(engine._instance, NULL);
  // free memory
  free(pipeline.commandBuffers);
  free(swapChain.swapChainImages);
  free(swapChain.swapChainImageViews);
  free(swapChain.frameBuffers);
  //vkDestroyBuffer(device, vertexBuffer, NULL);
  //vkFreeMemory(device, vertexBufferMemory, NULL);
  // free library
  close_library(vulkanLibrary);

  fprintf(stdout, "Cleanup successful\n");
}

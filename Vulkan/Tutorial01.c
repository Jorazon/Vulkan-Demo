#include <stdio.h>
#include "vulkan/vulkan.h"

#if defined(VK_USE_PLATFORM_WIN32_KHR)
#include <windows.h>
#elif defined(VK_USE_PLATFORM_XCB_KHR) || defined(VK_USE_PLATFORM_XLIB_KHR)
#include <dlfcn.h>
#endif

#define true 1
#define false 0

int LoadVulkanLibrary() {
  #if defined(VK_USE_PLATFORM_WIN32_KHR)

    HINSTANCE VulkanLibrary = LoadLibrary(L"vulkan-1.dll");

  #elif defined(VK_USE_PLATFORM_XCB_KHR) || defined(VK_USE_PLATFORM_XLIB_KHR)

    void* VulkanLibrary = dlopen(L"libvulkan.so", RTLD_NOW);

  #endif
  
  if (VulkanLibrary == NULL) {
    printf("Could not load Vulkan library!\n");
    return false;
  }

  return true;
}

int LoadExportedEntryPoints() {
  #if defined(VK_USE_PLATFORM_WIN32_KHR)
  #define LoadProcAddress GetProcAddress
  #elif defined(VK_USE_PLATFORM_XCB_KHR) || defined(VK_USE_PLATFORM_XLIB_KHR)
  #define LoadProcAddress dlsym
  #endif

  #define VK_EXPORTED_FUNCTION( fun )                                              \
  if( !(fun = (PFN_##fun)LoadProcAddress( VulkanLibrary, #fun )) ) {               \
    std::cout << "Could not load exported function: " << #fun << "!" << std::endl; \
    return false;                                                                  \
  }

  #include "ListOfFunctions.inl"

  return true;
}

int LoadGlobalLevelEntryPoints() {
  #define VK_GLOBAL_LEVEL_FUNCTION( fun )                                              \
  if( !(fun = (PFN_##fun)vkGetInstanceProcAddr( NULL, #fun )) ) {                   \
    std::cout << "Could not load global level function: " << #fun << "!" << std::endl; \
    return false;                                                                      \
  }

  #include "ListOfFunctions.inl"

  return true;
}

int CreateInstance() {
  VkApplicationInfo application_info = {
  VK_STRUCTURE_TYPE_APPLICATION_INFO,             // VkStructureType            sType             
  NULL,                                           // const void                *pNext             
  "API without Secrets: Introduction to Vulkan",  // const char                *pApplicationName  
  VK_MAKE_VERSION(1, 0, 0),                       // uint32_t                   applicationVersion
  "Vulkan Tutorial by Intel",                     // const char                *pEngineName       
  VK_MAKE_VERSION(1, 0, 0),                       // uint32_t                   engineVersion     
  VK_MAKE_VERSION(1, 0, 0)                        // uint32_t                   apiVersion        
  };

  VkInstanceCreateInfo instance_create_info = {
    VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,         // VkStructureType            sType                  
    NULL,                                           // const void*                pNext                  
    0,                                              // VkInstanceCreateFlags      flags                  
    &application_info,                              // const VkApplicationInfo   *pApplicationInfo       
    0,                                              // uint32_t                   enabledLayerCount      
    NULL,                                           // const char * const        *ppEnabledLayerNames    
    0,                                              // uint32_t                   enabledExtensionCount  
    NULL                                            // const char * const        *ppEnabledExtensionNames
  };

  if (vkCreateInstance(&instance_create_info, NULL, &Vulkan.Instance) != VK_SUCCESS) {
    printf("Could not create Vulkan instance!\n");
    return false;
  }
  return true;
}
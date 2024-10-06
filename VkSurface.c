#include <stdio.h>

#include "VkFunctionPointers.h"
#include "VkEngine.h"
#include "VkSurface.h"

VkResult CreateVulkanSurface(VkInstance instance, void* windowHandle, void* platformHandle, VkSurfaceKHR* out_surface) {
#ifdef _WIN32
  PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR)vkGetInstanceProcAddr(instance, "vkCreateWin32SurfaceKHR");
  if (!vkCreateWin32SurfaceKHR) {
    fprintf(stderr, "Failed to get pointer to vkCreateWin32SurfaceKHR\n");
    return VK_ERROR_UNKNOWN;
  }

  VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = { 0 };
  surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
  surfaceCreateInfo.hwnd = (HWND)windowHandle;
  surfaceCreateInfo.hinstance = (HINSTANCE)platformHandle;
  return vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, NULL, out_surface);

#elif defined(__linux__)
  PFN_VkXlibSurfaceCreateInfoKHR VkXlibSurfaceCreateInfoKHR = (PFN_VkXlibSurfaceCreateInfoKHR)vkGetInstanceProcAddr(instance, "VkXlibSurfaceCreateInfoKHR");
  if (!VkXlibSurfaceCreateInfoKHR) {
    fprintf(stderr, "Failed to get pointer to VkXlibSurfaceCreateInfoKHR\n");
    return VK_ERROR_UNKNOWN;
  }

  VkXlibSurfaceCreateInfoKHR surfaceCreateInfo = { 0 };
  surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
  surfaceCreateInfo.window = *(Window*)windowHandle; // X11 Window
  surfaceCreateInfo.dpy = (Display*)platformHandle;
  return vkCreateXlibSurfaceKHR(instance, &surfaceCreateInfo, NULL, out_surface);

  // For XCB, use VkXcbSurfaceCreateInfoKHR instead
  /*
  PFN_VkXcbSurfaceCreateInfoKHR VkXcbSurfaceCreateInfoKHR = (PFN_VkXcbSurfaceCreateInfoKHR)vkGetInstanceProcAddr(instance, "VkXcbSurfaceCreateInfoKHR");
  if (!VkXcbSurfaceCreateInfoKHR) {
    fprintf(stderr, "Failed to get pointer to VkXcbSurfaceCreateInfoKHR\n");
    return VK_ERROR_UNKNOWN;
  }

  VkXcbSurfaceCreateInfoKHR surfaceCreateInfo = { 0 };
  surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
  surfaceCreateInfo.connection = (xcb_connection_t*)platformHandle;
  surfaceCreateInfo.window = (xcb_window_t)windowHandle;
  return vkCreateXcbSurfaceKHR(instance, &surfaceCreateInfo, NULL, out_surface);
  */

#elif defined(__APPLE__)
  PFN_VkMacOSSurfaceCreateInfoMVK VkMacOSSurfaceCreateInfoMVK = (PFN_VkMacOSSurfaceCreateInfoMVK)vkGetInstanceProcAddr(instance, "VkMacOSSurfaceCreateInfoMVK");
  if (!VkMacOSSurfaceCreateInfoMVK) {
    fprintf(stderr, "Failed to get pointer to VkMacOSSurfaceCreateInfoMVK\n");
    return VK_ERROR_UNKNOWN;
  }

  VkMacOSSurfaceCreateInfoMVK surfaceCreateInfo = {0};
  surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
  surfaceCreateInfo.pView = windowHandle;  // NSView pointer (void*)
  return vkCreateMacOSSurfaceMVK(instance, &surfaceCreateInfo, NULL, out_surface);

#else
  return VK_ERROR_EXTENSION_NOT_PRESENT;  // Unsupported platform
#endif
}

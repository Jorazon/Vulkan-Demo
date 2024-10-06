#pragma once

#define VK_NO_PROTOTYPES

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#include <windows.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h> // For VK_KHR_win32_surface
#elif defined(__linux__)
#include <dlfcn.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_xlib.h>  // For VK_KHR_xlib_surface
#include <vulkan/vulkan_xcb.h>   // For VK_KHR_xcb_surface
#elif defined(__APPLE__)
//TODO library loading for apple
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_macos.h> // For VK_MVK_macos_surface or VK_EXT_metal_surface
#endif

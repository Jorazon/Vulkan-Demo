#pragma once

#include <stdio.h>

#include "VkIncludes.h"
#include "VkFunctionPointers.h"

typedef struct {
  VkSwapchainKHR swapChain;
  VkSurfaceFormatKHR surfaceFormat;
  VkExtent2D swapChainExtent;
  uint32_t swapChainImageCount;
  VkImage* swapChainImages;
  VkImageView* swapChainImageViews;
  VkRenderPass renderPass;
  VkFramebuffer* frameBuffers;
} VkSwapChain;

VkResult CreateSwapChain(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, VkSwapchainKHR* swapChain, VkSurfaceFormatKHR* surfaceFormat, VkExtent2D* swapChainExtent, uint32_t graphicsQueueFamilyIndex, uint32_t presentQueueFamilyIndex, uint32_t width, uint32_t height);
VkResult GetSwapChainImages(VkInstance instance, VkDevice device, VkSwapchainKHR swapChain, uint32_t* swapChainImageCount, VkImage** swapChainImages);
VkResult CreateImageViews(VkInstance instance, VkDevice device, uint32_t swapChainImageCount, VkImage* swapChainImages, VkFormat swapChainImageFormat, VkImageView** swapChainImageViews);
VkResult CreateRenderPass(VkInstance instance, VkDevice device, VkFormat swapChainImageFormat, VkRenderPass* renderPass);
VkResult CreateFramebuffers(VkInstance instance, VkDevice device, uint32_t swapChainImageCount, VkRenderPass renderPass, VkImageView* swapChainImageViews, VkExtent2D swapChainExtent, VkFramebuffer** frameBuffers);

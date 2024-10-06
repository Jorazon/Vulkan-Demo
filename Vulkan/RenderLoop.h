#pragma once

#include <stdio.h>

#include "VkIncludes.h"
#include "VkFunctionPointers.h"

VkResult InitRendering(
  VkInstance instance,
  VkDevice device,
  VkSwapchainKHR swapChain
);
void recordCommandBuffer(
  VkCommandBuffer commandBuffer,
  VkRenderPass renderPass,
  VkFramebuffer framebuffer,
  VkPipeline graphicsPipeline,
  VkExtent2D extent,
  VkBuffer vertexBuffer
);
void RenderTriangle(
  VkDevice device,
  VkSwapchainKHR swapChain,
  VkSemaphore imageAvailableSemaphore,
  VkSemaphore renderFinishedSemaphore,
  VkCommandBuffer* commandBuffers,
  uint32_t commandBuffersCount,
  VkQueue graphicsQueue,
  VkQueue presentQueue
);

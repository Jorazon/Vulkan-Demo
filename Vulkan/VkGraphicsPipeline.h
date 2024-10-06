#pragma once

#include <stdio.h>

#include "VkIncludes.h"
#include "VkFunctionPointers.h"
#include "Vertex.h"

typedef struct {
  VkPipelineLayout pipelineLayout;
  VkPipeline graphicsPipeline;
  VkCommandPool commandPool;
  VkCommandBuffer* commandBuffers;
  VkSemaphore imageAvailableSemaphore;
  VkSemaphore renderFinishedSemaphore;
  VkFence renderFence;
} VkGraphicsPipeline;

VkShaderModule createShaderModule(VkInstance instance,
  VkDevice device,
  const char* filePath
);
VkResult createGraphicsPipeline(VkInstance instance,
  VkDevice device,
  VkRenderPass renderPass,
  uint32_t width,
  uint32_t height,
  VkPipelineLayout* pipelineLayout,
  VkPipeline* graphicsPipeline
);
VkResult createCommandPool(VkInstance instance,
  VkDevice device,
  uint32_t queueFamilyIndex,
  VkCommandPool* commandPool
);
VkResult createCommandBuffer(VkInstance instance,
  VkDevice device,
  VkCommandPool commandPool,
  uint32_t commandBufferCount,
  VkCommandBuffer** commandBuffers
);
VkResult createSemaphore(VkInstance instance,
  VkDevice device,
  VkSemaphore* semaphore
);
VkResult createFence(VkInstance instance,
  VkDevice device,
  VkFence* fence,
  VkFenceCreateFlags flags
);

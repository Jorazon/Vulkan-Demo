#include <stdio.h>

#include "MakeWindow.h"
#include "VkEngine.h"
#include "VkDevice.h"
#include "VkSurface.h"
#include "VkSwapChain.h"
#include "VkGraphicsPipeline.h"

// tested on windows

#define WINDOW_W 800
#define WINDOW_H 600

int main()
{
  printf("Hello, world!\n");

  VkEngine engine;
  
  void* window_handle = make_window(WINDOW_W, WINDOW_H);

  CreateInstance(&(engine._instance));

  fprintf(stdout, "Instance pointer 0x%p\n", engine._instance);

  GetPhysicalDevice(engine._instance, &engine._chosenGPU);

  GetLogicalDevice(
    engine._instance,
    engine._chosenGPU,
    &engine._device
  );

  void* platformHandle;

#ifdef _WIN32
  platformHandle = GetModuleHandle(NULL);
#endif

  CreateVulkanSurface(
    engine._instance,
    window_handle,
    platformHandle,
    &engine._surface
  );

  QueueFamilyIndices queueFamilyIndices = 
    findQueueFamilies(
      engine._instance,
      engine._chosenGPU,
      engine._surface
    );

  VkSwapChain swapChain = { 0 };

  CreateSwapChain(
    engine._instance,
    engine._chosenGPU,
    engine._device,
    engine._surface,
    &swapChain.swapChain,
    &swapChain.surfaceFormat,
    &swapChain.swapChainExtent,
    queueFamilyIndices.graphicsQueueFamilyIndex,
    queueFamilyIndices.presentQueueFamilyIndex,
    WINDOW_W,
    WINDOW_H
  );
  
  GetSwapChainImages(
    engine._instance,
    engine._device,
    swapChain.swapChain,
    &swapChain.swapChainImageCount,
    &swapChain.swapChainImages
  );

  CreateImageViews(
    engine._instance,
    engine._device,
    swapChain.swapChainImageCount,
    swapChain.swapChainImages,
    swapChain.surfaceFormat.format,
    &swapChain.swapChainImageViews
  );

  CreateRenderPass(
    engine._instance,
    engine._device,
    swapChain.surfaceFormat.format,
    &swapChain.renderPass
  );

  CreateFramebuffers(
    engine._instance,
    engine._device,
    swapChain.swapChainImageCount,
    swapChain.renderPass,
    swapChain.swapChainImageViews,
    swapChain.swapChainExtent,
    &swapChain.frameBuffers
  );
  
  VkGraphicsPipeline pipeline = { NULL };

  createGraphicsPipeline(
    engine._instance,
    engine._device,
    swapChain.renderPass,
    WINDOW_W,
    WINDOW_H,
    &pipeline.pipelineLayout,
    &pipeline.graphicsPipeline
  );

  createCommandPool(
    engine._instance,
    engine._device,
    queueFamilyIndices.graphicsQueueFamilyIndex,
    &pipeline.commandPool
  );

  createSemaphore(
    engine._instance,
    engine._device,
    &pipeline.imageAvailableSemaphore
  );

  createSemaphore(
    engine._instance,
    engine._device,
    &pipeline.renderFinishedSemaphore
  );

  createFence(
    engine._instance,
    engine._device,
    &pipeline.renderFence,
    0
  );

  createCommandBuffer(
    engine._instance,
    engine._device,
    pipeline.commandPool,
    swapChain.swapChainImageCount,
    &pipeline.commandBuffers
  );

  InitRendering(engine._instance, engine._device, swapChain.swapChain);

  VkBuffer vertexBuffer;

  createVertexBuffer(
    engine._instance,
    engine._chosenGPU,
    engine._device,
    &vertexBuffer
  );

  for (size_t i = 0; i < swapChain.swapChainImageCount; i++) {
    recordCommandBuffer(
      pipeline.commandBuffers[i],
      swapChain.renderPass,
      swapChain.frameBuffers[i],
      pipeline.graphicsPipeline,
      swapChain.swapChainExtent,
      vertexBuffer
    );
  }

  VkQueue graphicsQueue = { 0 };
  VkQueue presentQueue = { 0 };

  GetQueue(engine._instance, engine._device, queueFamilyIndices.graphicsQueueFamilyIndex, &graphicsQueue);
  GetQueue(engine._instance, engine._device, queueFamilyIndices.presentQueueFamilyIndex, &presentQueue);

  window_loop(
    engine,
    swapChain,
    pipeline,
    graphicsQueue,
    presentQueue
  );

  clean(engine, swapChain, pipeline);

  return 0;
}

/*
1. Setup Before the Render Loop
Before entering the render loop, you should have the following objects set up:

x Logical Device (VkDevice)
x Swap Chain and Swap Chain Images
x Image Views for Swap Chain Images
x Render Pass
  Graphics Pipeline (including shaders, pipeline layout, etc.)
  Framebuffers (one per swap chain image)
  Command Pool (for managing command buffers)
  Command Buffers (one for each framebuffer)
*/
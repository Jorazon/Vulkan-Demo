#pragma once

#include <stdint.h>
#include <stdio.h>

#include "VkIncludes.h"
#include "VkEngine.h"
#include "VkSwapChain.h"
#include "VkGraphicsPipeline.h"
#include "RenderLoop.h"

void* make_window(uint32_t width, uint32_t height);
void window_loop(
  VkEngine engine,
  VkSwapChain swapChain,
  VkGraphicsPipeline pipeline,
  VkQueue graphicsQueue,
  VkQueue presentQueue
);

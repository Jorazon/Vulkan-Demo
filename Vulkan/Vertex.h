#pragma once

#include <stdio.h>

#include "VkIncludes.h"
#include "VkFunctionPointers.h"

typedef struct Vertex {
  float pos[3];   // X, Y, Z position
  float uv[2];    // U, V texture coordinate
  float color[3]; // R, G, B color
} Vertex;

#define VERTEX_ATTRIBUTE_COUNT 3U

void getBindingDescription(VkVertexInputBindingDescription* bindingDescription);
void getAttributeDescriptions(VkVertexInputAttributeDescription* attributeDescription);
void createVertexBuffer(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, VkBuffer* vertexBuffer);
uint32_t findMemoryType(VkInstance instance, VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

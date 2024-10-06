#include "Vertex.h"

const float h = 0.866f * (800.0f / 600.0f);

//Hard coded triangle
Vertex vertices[] = {
  {{ 0.0f, -0.655f, 0.0f}, {0.0f,  0.0f}, {1.0f, 0.0f, 0.0f}},
  {{ 0.5f,  0.500f, 0.0f}, {0.5f,  1.0f}, {0.0f, 1.0f, 0.0f}},
  {{-0.5f,  0.500f, 0.0f}, {0.0f,  1.0f}, {0.0f, 0.0f, 1.0f}}
};

void getBindingDescription(VkVertexInputBindingDescription* bindingDescriptions) {
  bindingDescriptions[0].binding = 0;
  bindingDescriptions[0].stride = sizeof(Vertex);
  bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
}

void getAttributeDescriptions(VkVertexInputAttributeDescription* attributeDescriptions) {
  // position
  attributeDescriptions[0].binding = 0;
  attributeDescriptions[0].location = 0;
  attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  attributeDescriptions[0].offset = offsetof(Vertex, pos);
  // uv
  attributeDescriptions[1].binding = 0;
  attributeDescriptions[1].location = 1;
  attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
  attributeDescriptions[1].offset = offsetof(Vertex, uv);
  // color
  attributeDescriptions[2].binding = 0;
  attributeDescriptions[2].location = 2;
  attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
  attributeDescriptions[2].offset = offsetof(Vertex, color);
}

void createVertexBuffer(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, VkBuffer* vertexBuffer) {
  PFN_vkCreateBuffer vkCreateBuffer = (PFN_vkCreateBuffer)vkGetInstanceProcAddr(instance, "vkCreateBuffer");
  if (!vkCreateBuffer) {
    fprintf(stderr, "Failed to get pointer to vkCreateBuffer\n");
  }
  PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements = (PFN_vkGetBufferMemoryRequirements)vkGetInstanceProcAddr(instance, "vkGetBufferMemoryRequirements");
  if (!vkGetBufferMemoryRequirements) {
    fprintf(stderr, "Failed to get pointer to vkGetBufferMemoryRequirements\n");
  }
  PFN_vkBindBufferMemory vkBindBufferMemory = (PFN_vkBindBufferMemory)vkGetInstanceProcAddr(instance, "vkBindBufferMemory");
  if (!vkBindBufferMemory) {
    fprintf(stderr, "Failed to get pointer to vkBindBufferMemory\n");
  }
  PFN_vkAllocateMemory vkAllocateMemory = (PFN_vkAllocateMemory)vkGetInstanceProcAddr(instance, "vkAllocateMemory");
  if (!vkAllocateMemory) {
    fprintf(stderr, "Failed to get pointer to vkAllocateMemory\n");
  }
  PFN_vkMapMemory vkMapMemory = (PFN_vkMapMemory)vkGetInstanceProcAddr(instance, "vkMapMemory");
  if (!vkMapMemory) {
    fprintf(stderr, "Failed to get pointer to vkMapMemory\n");
  }
  PFN_vkUnmapMemory vkUnmapMemory = (PFN_vkUnmapMemory)vkGetInstanceProcAddr(instance, "vkUnmapMemory");
  if (!vkUnmapMemory) {
    fprintf(stderr, "Failed to get pointer to vkUnmapMemory\n");
  }
  if (!vkCreateBuffer || !vkGetBufferMemoryRequirements || !vkBindBufferMemory || !vkAllocateMemory || !vkMapMemory || !vkUnmapMemory) {
    return;
  }

  VkBufferCreateInfo bufferInfo = { 0 };
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = sizeof(vertices);
  bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(device, &bufferInfo, NULL, vertexBuffer) != VK_SUCCESS) {
    fprintf(stderr, "Failed to create vertex buffer");
  }
  
  VkMemoryRequirements memRequirements = { 0 };
  vkGetBufferMemoryRequirements(device, *vertexBuffer, &memRequirements);

  VkMemoryAllocateInfo allocInfo = { 0 };
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = findMemoryType(instance, physicalDevice, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  VkDeviceMemory vertexBufferMemory = { 0 };

  VkResult result = vkAllocateMemory(device, &allocInfo, NULL, &vertexBufferMemory);

  if (result != VK_SUCCESS) {
    fprintf(stderr, "Failed to allocate vertex buffer memory\n");
    return;
  }

  vkBindBufferMemory(device, *vertexBuffer, vertexBufferMemory, 0);

  void* data = NULL;
  vkMapMemory(device, vertexBufferMemory, 0, bufferInfo.size, 0, &data);

  if (!data) {
    fprintf(stderr, "Failed to get map vertex buffer memory\n");
    return;
  }

  memcpy(data, vertices, (size_t)bufferInfo.size);

  vkUnmapMemory(device, vertexBufferMemory);
}

uint32_t findMemoryType(VkInstance instance, VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
  PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceMemoryProperties");
  if (!vkGetPhysicalDeviceMemoryProperties) {
    fprintf(stderr, "Failed to get pointer to vkGetPhysicalDeviceMemoryProperties\n");
    return VK_ERROR_UNKNOWN;
  }

  VkPhysicalDeviceMemoryProperties memProperties = { 0 };
  vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
      return i;
    }
  }

  fprintf(stderr, "Failed to find suitable memory type");

  return VK_ERROR_OUT_OF_DEVICE_MEMORY;
}

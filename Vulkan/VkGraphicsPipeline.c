#include "VkGraphicsPipeline.h"

// Helper function to read SPIR-V binary files
VkShaderModule createShaderModule(VkInstance instance, VkDevice device, const char* filePath) {
  PFN_vkCreateShaderModule vkCreateShaderModule = (PFN_vkCreateShaderModule)vkGetInstanceProcAddr(instance, "vkCreateShaderModule");
  if (!vkCreateShaderModule) {
    fprintf(stderr, "Failed to get pointer to vkCreateShaderModule\n");
    return VK_NULL_HANDLE;
  }
  
  FILE* file = fopen(filePath, "rb");
  if (!file) {
    fprintf(stderr, "Failed to open shader file: %s\n", filePath);
    return VK_NULL_HANDLE;
  }

  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);
  fseek(file, 0, SEEK_SET);

  uint32_t* buffer = (uint32_t*)malloc(size);
  if (!buffer) {
    fprintf(stderr, "Failed to allocate memory for shader module buffer\n");
    return VK_NULL_HANDLE;
  }
  fread(buffer, 1, size, file);
  fclose(file);

  VkShaderModuleCreateInfo createInfo = { 0 };
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = size;
  createInfo.pCode = (const uint32_t*)buffer;

  VkShaderModule shaderModule = NULL;
  if (vkCreateShaderModule(device, &createInfo, NULL, &shaderModule) != VK_SUCCESS) {
    fprintf(stderr, "Failed to create shader module: %s\n", filePath);
    free(buffer);
    return VK_NULL_HANDLE;
  }

  free(buffer);

  fprintf(stdout, "Shader module created successfully: %s\n", filePath);

  return shaderModule;
}

// Function to create the graphics pipeline
VkResult createGraphicsPipeline(VkInstance instance, VkDevice device, VkRenderPass renderPass, uint32_t width, uint32_t height, VkPipelineLayout* pipelineLayout, VkPipeline* graphicsPipeline) {
  // Get function pointers
  PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelines = (PFN_vkCreateGraphicsPipelines)vkGetInstanceProcAddr(instance, "vkCreateGraphicsPipelines");
  if (!vkCreateGraphicsPipelines) {
    fprintf(stderr, "Failed to get pointer to vkCreateGraphicsPipelines\n");
  }
  PFN_vkCreatePipelineLayout vkCreatePipelineLayout = (PFN_vkCreatePipelineLayout)vkGetInstanceProcAddr(instance, "vkCreatePipelineLayout");
  if (!vkCreatePipelineLayout) {
    fprintf(stderr, "Failed to get pointer to vkCreatePipelineLayout\n");
  }
  PFN_vkDestroyShaderModule vkDestroyShaderModule = (PFN_vkDestroyShaderModule)vkGetInstanceProcAddr(instance, "vkDestroyShaderModule");
  if (!vkDestroyShaderModule) {
    fprintf(stderr, "Failed to get pointer to vkDestroyShaderModule\n");
  }

  if (!vkCreateGraphicsPipelines || !vkCreatePipelineLayout || !vkDestroyShaderModule) {
    return VK_ERROR_UNKNOWN;
  }

  // Load shaders
  VkShaderModule vertShaderModule = createShaderModule(instance, device, "triangle.vert.spv");
  VkShaderModule fragShaderModule = createShaderModule(instance, device, "triangle.frag.spv");

  // Shader stage creation
  VkPipelineShaderStageCreateInfo vertShaderStageInfo = { 0 };
  vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = vertShaderModule;
  vertShaderStageInfo.pName = "main";  // Entry point in the shader

  VkPipelineShaderStageCreateInfo fragShaderStageInfo = { 0 };
  fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module = fragShaderModule;
  fragShaderStageInfo.pName = "main";  // Entry point in the shader

  VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

  // Vertex input state
  VkVertexInputBindingDescription bindingDescriptions[1] = { 0 };
  getBindingDescription(bindingDescriptions);
  VkVertexInputAttributeDescription attributeDescriptions[VERTEX_ATTRIBUTE_COUNT] = { { 0 } };
  getAttributeDescriptions(attributeDescriptions);
  VkPipelineVertexInputStateCreateInfo vertexInputInfo = { 0 };
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = 1;
  vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions;
  vertexInputInfo.vertexAttributeDescriptionCount = VERTEX_ATTRIBUTE_COUNT;
  vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions;

  // Input assembly state
  VkPipelineInputAssemblyStateCreateInfo inputAssembly = { 0 };
  inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; // Draw triangles
  inputAssembly.primitiveRestartEnable = VK_FALSE; // No primitive restart

  // Viewport and scissor
  VkViewport viewport = { 0 };
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)width; // Replace with your window width
  viewport.height = (float)height; // Replace with your window height
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor = { 0 };
  scissor.offset = (VkOffset2D){ 0, 0 };
  scissor.extent = (VkExtent2D){ width, height }; // Replace with your window size

  VkPipelineViewportStateCreateInfo viewportState = { 0 };
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.pViewports = &viewport;
  viewportState.scissorCount = 1;
  viewportState.pScissors = &scissor;

  // Rasterization state
  VkPipelineRasterizationStateCreateInfo rasterizer = { 0 };
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL; // Fill polygons
  rasterizer.lineWidth = 1.0f; // Line width
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT; // Cull back faces
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE; // Front face
  rasterizer.depthBiasEnable = VK_FALSE; // No depth bias

  // Multisampling
  VkPipelineMultisampleStateCreateInfo multisampling = { 0 };
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE; // No sample shading
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; // 1 sample per pixel

  // Color blending
  VkPipelineColorBlendAttachmentState colorBlendAttachment = { 0 };
  colorBlendAttachment.blendEnable = VK_FALSE; // No blending
  colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

  VkPipelineColorBlendStateCreateInfo colorBlending = { 0 };
  colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY; // Default logic operation
  colorBlending.attachmentCount = 1; // Number of attachments
  colorBlending.pAttachments = &colorBlendAttachment; // Attachments

  // Pipeline layout (no descriptor sets or push constants for now)
  VkPipelineLayoutCreateInfo pipelineLayoutInfo = { 0 };
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0; // No descriptor sets
  pipelineLayoutInfo.pSetLayouts = NULL; // No descriptor sets
  pipelineLayoutInfo.pushConstantRangeCount = 0; // No push constants
  pipelineLayoutInfo.pPushConstantRanges = NULL; // No push constants

  *pipelineLayout = VK_NULL_HANDLE;
  if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, NULL, pipelineLayout) != VK_SUCCESS) {
    fprintf(stderr, "Failed to create pipeline layout\n");
    return VK_ERROR_UNKNOWN;
  }

  // Graphics pipeline creation
  VkGraphicsPipelineCreateInfo pipelineInfo = { 0 };
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2; // Vertex and fragment shaders
  pipelineInfo.pStages = shaderStages;
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDepthStencilState = NULL; // No depth/stencil state
  pipelineInfo.pDynamicState = NULL; // No dynamic states
  pipelineInfo.layout = *pipelineLayout; // Pipeline layout
  pipelineInfo.renderPass = renderPass; // Render pass
  pipelineInfo.subpass = 0; // Subpass index

  *graphicsPipeline = VK_NULL_HANDLE;
  if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, graphicsPipeline) != VK_SUCCESS) {
    fprintf(stderr, "Failed to create graphics pipeline\n");
    return VK_ERROR_UNKNOWN;
  }

  // Cleanup shader modules after creating the pipeline
  vkDestroyShaderModule(device, fragShaderModule, NULL);
  vkDestroyShaderModule(device, vertShaderModule, NULL);

  fprintf(stdout, "Graphics pipeline created successfully\n");

  return VK_SUCCESS;
}

VkResult createCommandPool(VkInstance instance, VkDevice device, uint32_t queueFamilyIndex, VkCommandPool* commandPool) {
  PFN_vkCreateCommandPool vkCreateCommandPool = (PFN_vkCreateCommandPool)vkGetInstanceProcAddr(instance, "vkCreateCommandPool");
  if (!vkCreateCommandPool) {
    fprintf(stderr, "Failed to get pointer to vkCreateCommandPool\n");
    return VK_ERROR_UNKNOWN;
  }
  
  VkCommandPoolCreateInfo poolInfo = { 0 };
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.queueFamilyIndex = queueFamilyIndex;
  poolInfo.flags = 0; //One of: 0, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT or VK_COMMAND_POOL_CREATE_TRANSIENT_BIT

  if (vkCreateCommandPool(device, &poolInfo, NULL, commandPool) != VK_SUCCESS) {
    fprintf(stderr, "Failed to create command pool\n");
    return VK_ERROR_UNKNOWN;
  }

  return VK_SUCCESS;
}

VkResult createCommandBuffer(VkInstance instance, VkDevice device, VkCommandPool commandPool, uint32_t commandBufferCount, VkCommandBuffer** commandBuffers) {
  PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers = (PFN_vkAllocateCommandBuffers)vkGetInstanceProcAddr(instance, "vkAllocateCommandBuffers");
  if (!vkAllocateCommandBuffers) {
    fprintf(stderr, "Failed to get pointer to vkAllocateCommandBuffers\n");
    return VK_ERROR_UNKNOWN;
  }

  *commandBuffers = malloc(sizeof(VkSurfaceFormatKHR) * commandBufferCount);
  if (!(*commandBuffers)) {
    fprintf(stderr, "Failed to allocate memory for command buffers\n");
    return VK_ERROR_OUT_OF_HOST_MEMORY;
  }

  VkCommandBufferAllocateInfo allocInfo = { 0 };
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = commandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // VK_COMMAND_BUFFER_LEVEL_SECONDARY for secondary command buffers
  allocInfo.commandBufferCount = commandBufferCount; // Number of command buffers to allocate.

  if (vkAllocateCommandBuffers(device, &allocInfo, *commandBuffers) != VK_SUCCESS) {
    fprintf(stderr, "Failed to allocate command buffer\n");
    return VK_ERROR_UNKNOWN;
  }

  return VK_SUCCESS;
}

VkResult createSemaphore(VkInstance instance, VkDevice device, VkSemaphore* semaphore) {
  PFN_vkCreateSemaphore vkCreateSemaphore = (PFN_vkCreateSemaphore)vkGetInstanceProcAddr(instance, "vkCreateSemaphore");
  if (!vkCreateSemaphore) {
    fprintf(stderr, "Failed to get pointer to vkCreateSemaphore\n");
    return VK_ERROR_UNKNOWN;
  }
  
  // Define the semaphore creation info struct
  VkSemaphoreCreateInfo semaphoreInfo = { 0 };
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  semaphoreInfo.pNext = NULL;
  semaphoreInfo.flags = 0; // No special flags for the semaphore

  // Create the semaphore and check for success
  VkResult result = vkCreateSemaphore(device, &semaphoreInfo, NULL, semaphore);
  if (result != VK_SUCCESS) {
    fprintf(stderr, "Failed to create semaphore. Error: %d\n", result);
  }
  return result;
}

VkResult createFence(VkInstance instance, VkDevice device, VkFence* fence, VkFenceCreateFlags flags) {
  PFN_vkCreateFence vkCreateFence = (PFN_vkCreateFence)vkGetInstanceProcAddr(instance, "vkCreateFence");
  if (!vkCreateFence) {
    fprintf(stderr, "Failed to get pointer to vkCreateFence\n");
    return VK_ERROR_UNKNOWN;
  }
  
  // Define the fence creation info struct
  VkFenceCreateInfo fenceInfo = { 0 };
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.pNext = NULL;
  fenceInfo.flags = flags; // Optional: Set to VK_FENCE_CREATE_SIGNALED_BIT if needed

  // Create the fence and check for success
  VkResult result = vkCreateFence(device, &fenceInfo, NULL, fence);
  if (result != VK_SUCCESS) {
    fprintf(stderr, "Failed to create fence. Error: %d\n", result);
  }
  return result;
}

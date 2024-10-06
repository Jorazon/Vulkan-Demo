#include "VkSwapChain.h"

VkResult CreateSwapChain(
  VkInstance instance,
  VkPhysicalDevice physicalDevice,
  VkDevice device,
  VkSurfaceKHR surface,
  VkSwapchainKHR* swapChain,         // Output parameter for the created swap chain
  VkSurfaceFormatKHR* surfaceFormat, // Output parameter for the selected surface format
  VkExtent2D* swapChainExtent,       // Output parameter for the selected swap chain extent
  uint32_t graphicsQueueFamilyIndex, // Index of the graphics queue family
  uint32_t presentQueueFamilyIndex,  // Index of the presentation queue family
  uint32_t width,                    // Desired width of the swap chain
  uint32_t height                    // Desired height of the swap chain
) {
  PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
  if (!vkGetPhysicalDeviceSurfaceCapabilitiesKHR) {
    fprintf(stderr, "Failed to get pointer to vkGetPhysicalDeviceSurfaceCapabilitiesKHR\n");
  }
  PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
  if (!vkGetPhysicalDeviceSurfaceFormatsKHR) {
    fprintf(stderr, "Failed to get pointer to vkGetPhysicalDeviceSurfaceFormatsKHR\n");
  }
  PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfacePresentModesKHR");
  if (!vkGetPhysicalDeviceSurfacePresentModesKHR) {
    fprintf(stderr, "Failed to get pointer to vkGetPhysicalDeviceSurfacePresentModesKHR\n");
  }
  PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)vkGetInstanceProcAddr(instance, "vkCreateSwapchainKHR");
  if (!vkCreateSwapchainKHR) {
    fprintf(stderr, "Failed to get pointer to vkCreateSwapchainKHR\n");
  }

  if (!vkGetPhysicalDeviceSurfaceCapabilitiesKHR || !vkGetPhysicalDeviceSurfaceFormatsKHR || !vkGetPhysicalDeviceSurfacePresentModesKHR || !vkCreateSwapchainKHR) {
    return VK_ERROR_UNKNOWN;
  }

  // 1. Query Swap Chain Support Details
  VkSurfaceCapabilitiesKHR surfaceCapabilities;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);

  uint32_t formatCount = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, NULL);

  VkSurfaceFormatKHR* formats = malloc(sizeof(VkSurfaceFormatKHR) * formatCount);
  if (!formats) {
    fprintf(stderr, "Failed to allocate memory for formats\n");
    return VK_ERROR_OUT_OF_HOST_MEMORY;
  }
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats);

  uint32_t presentModeCount = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, NULL);

  VkPresentModeKHR* presentModes = malloc(sizeof(VkPresentModeKHR) * presentModeCount);
  if (!presentModes) {
    fprintf(stderr, "Failed to allocate memory for present modes\n");
    return VK_ERROR_OUT_OF_HOST_MEMORY;
  }
  vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes);

  // 2. Choose Surface Format
  *surfaceFormat = formats[0]; // Default to the first format if none are better
  for (uint32_t i = 0; i < formatCount; i++) {
    if (formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      *surfaceFormat = formats[i];
      break;
    }
  }
  free(formats);

  // 3. Choose Present Mode
  VkPresentModeKHR chosenPresentMode = VK_PRESENT_MODE_FIFO_KHR; // Use FIFO (standard mode)
  for (uint32_t i = 0; i < presentModeCount; i++) {
    if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
      chosenPresentMode = presentModes[i];
      break;
    }
  }
  free(presentModes);

  // 4. Choose Swap Extent (resolution)
  *swapChainExtent = surfaceCapabilities.currentExtent;
  if (surfaceCapabilities.currentExtent.width == (uint32_t)-1) {
    // Use specified width and height if the extent is undefined
    *swapChainExtent = (VkExtent2D){ width, height };
  }

  // Clamp the width and height to the supported limits
  swapChainExtent->width = (swapChainExtent->width > surfaceCapabilities.maxImageExtent.width)
    ? surfaceCapabilities.maxImageExtent.width
    : swapChainExtent->width;
  swapChainExtent->height = (swapChainExtent->height > surfaceCapabilities.maxImageExtent.height)
    ? surfaceCapabilities.maxImageExtent.height
    : swapChainExtent->height;

  // 5. Specify the number of images in the swap chain
  uint32_t imageCount = surfaceCapabilities.minImageCount + 1; // Request one more than minimum
  if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount) {
    imageCount = surfaceCapabilities.maxImageCount;
  }

  // 6. Create the Swap Chain
  VkSwapchainCreateInfoKHR createInfo = { 0 };
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = surface;
  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat->format;
  createInfo.imageColorSpace = surfaceFormat->colorSpace;
  createInfo.imageExtent = *swapChainExtent;
  createInfo.imageArrayLayers = 1; // Single-layered image
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  // If the graphics and present queues are the same, use exclusive mode
  uint32_t queueFamilyIndices[] = { graphicsQueueFamilyIndex, presentQueueFamilyIndex };
  if (graphicsQueueFamilyIndex != presentQueueFamilyIndex) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT; // Images can be used across multiple queues
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  }
  else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0; // Optional
    createInfo.pQueueFamilyIndices = NULL; // Optional
  }

  createInfo.preTransform = surfaceCapabilities.currentTransform; // No transform applied
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // Ignore alpha blending
  createInfo.presentMode = chosenPresentMode;
  createInfo.clipped = VK_TRUE; // Clip pixels outside the visible region
  createInfo.oldSwapchain = VK_NULL_HANDLE; // No previous swap chain to replace

  // Create the swap chain
  VkResult result = vkCreateSwapchainKHR(device, &createInfo, NULL, swapChain);

  if (result != VK_SUCCESS) {
    fprintf(stderr, "Failed to create swap chain\n");
  }
  
  fprintf(stderr, "Swap chain created succesfully\n");

  return result;
}

VkResult GetSwapChainImages(VkInstance instance, VkDevice device, VkSwapchainKHR swapChain, uint32_t* swapChainImageCount, VkImage** swapChainImages) {
  PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)vkGetInstanceProcAddr(instance, "vkGetSwapchainImagesKHR");
  if (!vkGetSwapchainImagesKHR) {
    fprintf(stderr, "Failed to get pointer to vkGetSwapchainImagesKHR\n");
    return VK_ERROR_UNKNOWN;
  }

  VkResult result;

  result = vkGetSwapchainImagesKHR(device, swapChain, swapChainImageCount, NULL);
  if (result != VK_SUCCESS) {
    fprintf(stderr, "Failed to get swap chain image count! Error code: %d\n", result);
    return result;
  }

  fprintf(stdout, "Swap chain image count: %d\n", *swapChainImageCount);

  // Step 2: Allocate memory for the swap chain images.
  *swapChainImages = (VkImage*)malloc(*swapChainImageCount * sizeof(VkImage));
  if (!(*swapChainImages)) {
    fprintf(stderr, "Failed to allocate memory for swap chain images\n");
    return VK_ERROR_OUT_OF_HOST_MEMORY;
  }

  // Step 3: Get the swap chain images.
  result = vkGetSwapchainImagesKHR(device, swapChain, swapChainImageCount, *swapChainImages);
  if (result != VK_SUCCESS) {
    fprintf(stderr, "Failed to retrieved swap chain images! Error code: %d\n", result);
    free(*swapChainImages);
    return result;
  }

  fprintf(stdout, "Swap chain images retrieved successfully\n");

  return result;
}

VkResult CreateImageViews(VkInstance instance, VkDevice device, uint32_t swapChainImageCount, VkImage* swapChainImages, VkFormat swapChainImageFormat, VkImageView** swapChainImageViews) {
  PFN_vkCreateImageView vkCreateImageView = (PFN_vkCreateImageView)vkGetInstanceProcAddr(instance, "vkCreateImageView");
  if (!vkCreateImageView) {
    fprintf(stderr, "Failed to get pointer to vkCreateImageView\n");
    return VK_ERROR_UNKNOWN;
  }

  (*swapChainImageViews) = (VkImageView*)malloc(swapChainImageCount * sizeof(VkImageView));
  if (!(*swapChainImageViews)) {
    fprintf(stderr, "Failed to allocate memory for swap chain image views\n");
    return VK_ERROR_OUT_OF_HOST_MEMORY;
  }

  VkResult result = VK_ERROR_UNKNOWN;

  for (uint32_t i = 0; i < swapChainImageCount; i++) {
    VkImageViewCreateInfo viewInfo = { 0 };
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = swapChainImages[i];
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = swapChainImageFormat;
    viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    result = vkCreateImageView(device, &viewInfo, NULL, &(*swapChainImageViews)[i]);

    if (result != VK_SUCCESS) {
      fprintf(stderr, "Failed to create image views\n");
    }
  }

  if (result == VK_SUCCESS) fprintf(stdout, "Image views created successfully\n");

  return result;
}

VkResult CreateRenderPass(VkInstance instance, VkDevice device, VkFormat swapChainImageFormat, VkRenderPass* renderPass)
{
  PFN_vkCreateRenderPass vkCreateRenderPass = (PFN_vkCreateRenderPass)vkGetInstanceProcAddr(instance, "vkCreateRenderPass");
  if (!vkCreateRenderPass) {
    fprintf(stderr, "Failed to get pointer to vkCreateRenderPass\n");
    return VK_ERROR_UNKNOWN;
  }

  VkResult result = VK_ERROR_UNKNOWN;

  VkAttachmentDescription colorAttachment = { 0 };
  colorAttachment.format = swapChainImageFormat;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef = { 0 };
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass = { 0 };
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;

  VkRenderPassCreateInfo renderPassInfo = { 0 };
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments = &colorAttachment;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;

  result = vkCreateRenderPass(device, &renderPassInfo, NULL, renderPass);

  if (result != VK_SUCCESS) {
    fprintf(stderr, "Failed to create render pass\n");
  } else {
    fprintf(stdout, "Render pass created successfully\n");
  }

  return result;
}

VkResult CreateFramebuffers(VkInstance instance, VkDevice device, uint32_t swapChainImageCount, VkRenderPass renderPass, VkImageView* swapChainImageViews, VkExtent2D swapChainExtent, VkFramebuffer** frameBuffers)
{
  PFN_vkCreateFramebuffer vkCreateFramebuffer = (PFN_vkCreateFramebuffer)vkGetInstanceProcAddr(instance, "vkCreateFramebuffer");
  if (!vkCreateFramebuffer) {
    fprintf(stderr, "Failed to get pointer to vkCreateFramebuffer\n");
    return VK_ERROR_UNKNOWN;
  }

  *frameBuffers = (VkFramebuffer*)malloc(swapChainImageCount * sizeof(VkFramebuffer));
  if (!(*frameBuffers)) {
    fprintf(stderr, "Failed to allocate memory for swap chain image views\n");
    return VK_ERROR_OUT_OF_HOST_MEMORY;
  }

  VkResult result = VK_ERROR_UNKNOWN;

  for (uint32_t i = 0; i < swapChainImageCount; i++) {
    VkFramebufferCreateInfo framebufferInfo = { 0 };
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = &swapChainImageViews[i];
    framebufferInfo.width = swapChainExtent.width;
    framebufferInfo.height = swapChainExtent.height;
    framebufferInfo.layers = 1;

    result = vkCreateFramebuffer(device, &framebufferInfo, NULL, &(*frameBuffers)[i]);

    if (result != VK_SUCCESS) {
      fprintf(stderr, "Failed to create framebuffer\n");
    }
  }
  
  if (result == VK_SUCCESS) fprintf(stdout, "Framebuffers created successfully\n");

  return result;
}

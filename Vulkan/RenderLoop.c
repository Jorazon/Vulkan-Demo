#include "RenderLoop.h"

PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR = NULL;
PFN_vkQueueSubmit vkQueueSubmit = NULL;
PFN_vkQueuePresentKHR vkQueuePresentKHR = NULL;

PFN_vkBeginCommandBuffer vkBeginCommandBuffer = NULL;
PFN_vkEndCommandBuffer vkEndCommandBuffer = NULL;
PFN_vkCmdBeginRenderPass vkCmdBeginRenderPass = NULL;
PFN_vkCmdEndRenderPass vkCmdEndRenderPass = NULL;

PFN_vkCmdBindPipeline vkCmdBindPipeline = NULL;
PFN_vkCmdBindVertexBuffers vkCmdBindVertexBuffers = NULL;
PFN_vkCmdDraw vkCmdDraw = NULL;

VkResult InitRendering(VkInstance instance, VkDevice device, VkSwapchainKHR swapChain) {
  vkAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR)vkGetInstanceProcAddr(instance, "vkAcquireNextImageKHR");
  if (!vkAcquireNextImageKHR) {
    fprintf(stderr, "Failed to get pointer to vkAcquireNextImageKHR\n");
  }
  vkQueueSubmit = (PFN_vkQueueSubmit)vkGetInstanceProcAddr(instance, "vkQueueSubmit");
  if (!vkQueueSubmit) {
    fprintf(stderr, "Failed to get pointer to vkQueueSubmit\n");
  }
  vkQueuePresentKHR = (PFN_vkQueuePresentKHR)vkGetInstanceProcAddr(instance, "vkQueuePresentKHR");
  if (!vkQueuePresentKHR) {
    fprintf(stderr, "Failed to get pointer to vkQueuePresentKHR\n");
  }

  vkBeginCommandBuffer = (PFN_vkBeginCommandBuffer)vkGetInstanceProcAddr(instance, "vkBeginCommandBuffer");
  if (!vkBeginCommandBuffer) {
    fprintf(stderr, "Failed to get pointer to vkBeginCommandBuffer\n");
  }
  vkEndCommandBuffer = (PFN_vkEndCommandBuffer)vkGetInstanceProcAddr(instance, "vkEndCommandBuffer");
  if (!vkEndCommandBuffer) {
    fprintf(stderr, "Failed to get pointer to vkEndCommandBuffer\n");
  }
  vkCmdBeginRenderPass = (PFN_vkCmdBeginRenderPass)vkGetInstanceProcAddr(instance, "vkCmdBeginRenderPass");
  if (!vkCmdBeginRenderPass) {
    fprintf(stderr, "Failed to get pointer to vkCmdBeginRenderPass\n");
  }
  vkCmdEndRenderPass = (PFN_vkCmdEndRenderPass)vkGetInstanceProcAddr(instance, "vkCmdEndRenderPass");
  if (!vkCmdEndRenderPass) {
    fprintf(stderr, "Failed to get pointer to vkCmdEndRenderPass\n");
  }

  vkCmdBindPipeline = (PFN_vkCmdBindPipeline)vkGetInstanceProcAddr(instance, "vkCmdBindPipeline");
  if (!vkCmdBindPipeline) {
    fprintf(stderr, "Failed to get pointer to vkCmdBindPipeline\n");
  }
  vkCmdBindVertexBuffers = (PFN_vkCmdBindVertexBuffers)vkGetInstanceProcAddr(instance, "vkCmdBindVertexBuffers");
  if (!vkCmdBindVertexBuffers) {
    fprintf(stderr, "Failed to get pointer to vkCmdBindVertexBuffers\n");
  }
  vkCmdDraw = (PFN_vkCmdDraw)vkGetInstanceProcAddr(instance, "vkCmdDraw");
  if (!vkCmdDraw) {
    fprintf(stderr, "Failed to get pointer to vkCmdDraw\n");
  }

  if (
    !vkAcquireNextImageKHR || !vkQueueSubmit || !vkQueuePresentKHR ||
    !vkBeginCommandBuffer || !vkEndCommandBuffer || !vkCmdBeginRenderPass || !vkCmdEndRenderPass ||
    !vkCmdBindPipeline || !vkCmdBindVertexBuffers || !vkCmdDraw
  ) {
    return VK_ERROR_UNKNOWN;
  }
  
  VkResult result = VK_ERROR_UNKNOWN;

  return result;
}

void recordCommandBuffer(VkCommandBuffer commandBuffer, VkRenderPass renderPass, VkFramebuffer framebuffer, VkPipeline graphicsPipeline, VkExtent2D extent, VkBuffer vertexBuffer) {
  // Begin the command buffer recording
  VkCommandBufferBeginInfo beginInfo = { 0 };
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = 0;  // Optional flags like VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
  beginInfo.pInheritanceInfo = NULL;  // Only used for secondary command buffers

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
    printf("Failed to begin recording command buffer!\n");
    return;
  }

  // Define the clear color for the render pass
  VkClearValue clearColor = { { 1.0f, 0.0f, 1.0f, 1.0f } };  // Pink with full opacity

  // Begin the render pass
  VkRenderPassBeginInfo renderPassInfo = { 0 };
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = renderPass;
  renderPassInfo.framebuffer = framebuffer;
  renderPassInfo.renderArea.offset = (VkOffset2D){ 0, 0 };
  renderPassInfo.renderArea.extent = extent;
  renderPassInfo.clearValueCount = 1;
  renderPassInfo.pClearValues = &clearColor;

  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

  // Bind the graphics pipeline
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

  // Bind the vertex buffer (ensure you have vertex data uploaded)
  VkBuffer vertexBuffers[] = { vertexBuffer };
  VkDeviceSize offsets[] = { 0 };
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

  // Draw the triangle (no vertex buffer needed if hardcoded in the shader)
  vkCmdDraw(commandBuffer, 3, 1, 0, 0);  // Draw 3 vertices to form a triangle

  // End the render pass
  vkCmdEndRenderPass(commandBuffer);

  // Finish recording the command buffer
  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    printf("Failed to record command buffer!\n");
  }
}

void RenderTriangle(
  VkDevice device,
  VkSwapchainKHR swapChain,
  VkSemaphore imageAvailableSemaphore,
  VkSemaphore renderFinishedSemaphore,
  VkCommandBuffer* commandBuffers,
  uint32_t commandBuffersCount,
  VkQueue graphicsQueue,
  VkQueue presentQueue
) {
  uint32_t imageIndex = 0;
  VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    // Recreate swap chain if window size changed or other issue.
    //RecreateSwapChain(); //TODO
    return;
  }
  else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    fprintf(stderr, "Failed to acquire swap chain image.\n");
    return;
  }

  // Submit command buffer
  VkSubmitInfo submitInfo = { 0 };
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
  VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = commandBuffersCount;
  submitInfo.pCommandBuffers = commandBuffers;

  VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
    fprintf(stderr, "Failed to submit draw command buffer.\n");
    return;
  }

  // Present swap chain image
  VkPresentInfoKHR presentInfo = { 0 };
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = { swapChain };
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &imageIndex;

  result = vkQueuePresentKHR(presentQueue, &presentInfo);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
    //RecreateSwapChain(); //TODO
  }
  else if (result != VK_SUCCESS) {
    fprintf(stderr, "Failed to present swap chain image.\n");
  }
}

#pragma once

#define VK_USE_PLATFORM_WIN32_KHR

#include <string>
#include <vector>
#include <map>
#include <Windows.h>
#include <tchar.h>
#include <vulkan\vulkan.h>

#include "Logger.h"
#include "VulkanWindow.h"

class VulkanSample
{
private:
	
	FileLogger								mLogger;
	VkInstance								mVulkanInstance;
	VkPhysicalDevice						mPhysicalDevice;
	VkPhysicalDeviceFeatures				mPhysicalDeviceFeatures;
	VkPhysicalDeviceProperties				mPhysicalDeviceProperties;
	VkPhysicalDeviceMemoryProperties		mPhysicalDeviceMemoryProperties;
	uint32_t								mQueueFamilyIndex;
	VkDevice								mDevice;
	VulkanWindow							mWindow;
	VkSurfaceKHR							mPresentationSurface;
	VkPresentModeKHR						mPresentMode;
	VkSurfaceCapabilitiesKHR				mPresentationSurfaceCapabilities;
	uint32_t								mNumSwapChainImages;
	VkExtent2D								mSwapChainImageSize;
	VkSurfaceFormatKHR						mPresentationSurfaceFormat;
	VkSwapchainKHR							mOldSwapChain;
	VkSwapchainKHR							mSwapChain;
	VkCommandPool							mCommandPool;
	
	std::vector<VkPhysicalDevice>			mDevices;
	std::vector<VkExtensionProperties>		mInstanceExtensions;	
	std::vector<VkExtensionProperties>		mDeviceExtensions;
	std::vector<VkQueueFamilyProperties>	mQueueFamilyProperties;
	std::vector<VkQueue>					mQueues;
	std::vector<VkPresentModeKHR>			mPresentModes;
	std::vector<VkSurfaceFormatKHR>			mPresentationSurfaceFormats;
	std::vector<VkImage>					mSwapChainImages;

public:

	VulkanSample();
	~VulkanSample();

	bool Initialize();
	void Destroy();

	bool CreateVulkanInstance(const std::vector<char*> &desiredExtensions);
	void DestroyVulkanInstance();

	bool PopulateInstanceExtensions();
	void LogInstanceExtensions();

	bool PopulatePhysicalDevices();
	void PopulatePhysicalDeviceFeaturesAndProperties();
	void LogPhysicalDeviceProperties();

	bool PopulateDeviceExtensions();
	void LogDeviceExtensions();

	bool PopulateQueueFamilyProperties();
	bool SelectQueueFamily(VkQueueFlags desiredType, bool supportPresentation = true);

	bool CreateDevice(const std::vector<char*> &desiredExtensions, 
		const std::vector<float> &desiredQueuePriorities);

	void DestroyDevice();
	bool GetQueues(uint32_t queueCount);

	bool CreateVulkanWindow(uint32_t width, uint32_t height, 
		const std::string &title = "Vulkan Window");

	void ShowVulkanWindow();
	void DestroyVulkanWindow();

	bool CreatePresentationSurface();
	void DestroyPresentationSurface();

	bool PopulatePresentModes();
	bool SelectPresentMode(VkPresentModeKHR presentMode);

	bool PopulatePresentationSurfaceFormats();
	bool SelectPresentationSurfaceFormat(VkSurfaceFormatKHR surfaceFormat);

	bool CreateSwapChain();
	void DestroySwapChain();

	bool CreateCommandPool(VkCommandPoolCreateFlags createFlags);
	void DestroyCommandPool();
	bool ResetCommandPool(bool releaseResources);

	bool AllocateCommandBuffers(uint32_t count, VkCommandBufferLevel level, 
		std::vector<VkCommandBuffer> &buffers);

	void FreeCommandBuffers(const std::vector<VkCommandBuffer> &buffers);

	bool BeginCommandBuffer(VkCommandBuffer buffer, 
		VkCommandBufferLevel level,
		VkCommandBufferUsageFlags usage,
		VkCommandBufferInheritanceInfo *inheritenceInfo);

	bool EndCommandBuffer(VkCommandBuffer buffer);
	bool ResetCommandBuffer(VkCommandBuffer buffer, bool releaseResources);

	bool CreateVulkanSemaphore(VkSemaphore *semaphore);
	void DestroyVulkanSemaphore(VkSemaphore semaphore);

	bool CreateFence(VkFence *fence, bool isSignaled);
	void DestroyFence(VkFence fence);

	bool ResetFences(const std::vector<VkFence> &fences);
	bool WaitForFences(const std::vector<VkFence> &fences,
		bool waitForAll, uint64_t timeout);

	bool SubmitCommandBuffers(uint32_t queueIndex, 
		const std::vector<VkCommandBuffer> &buffers,
		const std::vector<VkSemaphore> &waitSemaphores,
		const std::vector<VkPipelineStageFlags> &waitStates,
		const std::vector<VkSemaphore> &signaledSemaphores,
		VkFence fence);

	bool CreateBuffer(VkBuffer *buffer, 
		VkDeviceMemory * memory, 
		VkBufferUsageFlags usage,
		VkDeviceSize size, 
		VkMemoryPropertyFlags propertyFlags);

	void DestroyBuffer(VkBuffer buffer, VkDeviceMemory memory);

	void SetBuffersMemoryBarrier(VkCommandBuffer commandBuffer, 
		const std::vector<VkBuffer> &buffers,
		const std::vector<VkAccessFlags> &currentAccess,
		const std::vector<VkAccessFlags> &newAccess, 
		VkPipelineStageFlags generatngStages,
		VkPipelineStageFlags consumingStages);

	bool CreateBufferView(VkBuffer buffer, VkBufferView *view, 
		VkFormat format, VkDeviceSize offset, VkDeviceSize size);

	bool CreateImage(VkImageType type, 
		bool cubemap, 
		VkFormat format,
		VkExtent3D size, 
		uint32_t numMipMaps, 
		uint32_t numLayers, 
		VkSampleCountFlagBits samples,
		VkImageUsageFlags usage, 
		VkMemoryPropertyFlags propertyFlags,
		VkDeviceMemory *memory,
		VkImage *image);

	void SetImagesMemoryBarrier(VkCommandBuffer commandBuffer,
		const std::vector<VkImage> &images,
		const std::vector<VkAccessFlags> &currentAccess,
		const std::vector<VkAccessFlags> &newAccess,
		const std::vector<VkImageLayout> &currentLayout,
		const std::vector<VkImageLayout> &newLayout,
		const std::vector<VkImageAspectFlags> &aspectFlags,
		VkPipelineStageFlags generatngStages,
		VkPipelineStageFlags consumingStages);



private:

	bool IsInstanceExtensionSupported(const std::string &extension);
	bool IsDeviceExtensionSupported(const std::string &extension);
	bool IsQueueFamilySupportsPresentation(uint32_t index);
};
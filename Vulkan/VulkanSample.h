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

struct BufferMemoryTransition
{
	VkBuffer Buffer;
	VkAccessFlags CurrentAccess;
	VkAccessFlags NewAccess;
};

struct ImageMemoryTransition
{
	VkImage Image;
	VkAccessFlags CurrentAccess;
	VkAccessFlags NewAccess;
	VkImageLayout CurrentLayout;
	VkImageLayout NewLayout;
	VkImageAspectFlags AspectFlags;
};

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
	std::vector<VkFormat>					mFormats;

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
	bool SelectQueueFamily(VkQueueFlags desiredType, 
		bool supportPresentation = true);

	bool CreateDevice(const std::vector<char*> &desiredExtensions, 
		const std::vector<float> &desiredQueuePriorities);

	void DestroyDevice();
	bool GetQueues(uint32_t queueCount);

	bool CreateVulkanWindow(uint32_t width, 
		uint32_t height, 
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

	bool AllocateCommandBuffers(uint32_t count, 
		VkCommandBufferLevel level, 
		std::vector<VkCommandBuffer> &buffers);

	void FreeCommandBuffers(const std::vector<VkCommandBuffer> &buffers);

	bool BeginCommandBuffer(VkCommandBuffer buffer, 
		VkCommandBufferLevel level,
		VkCommandBufferUsageFlags usage,
		VkCommandBufferInheritanceInfo *inheritenceInfo);

	bool EndCommandBuffer(VkCommandBuffer buffer);
	bool ResetCommandBuffer(VkCommandBuffer buffer, 
		bool releaseResources);

	bool CreateVulkanSemaphore(VkSemaphore *semaphore);
	void DestroyVulkanSemaphore(VkSemaphore semaphore);

	bool CreateFence(VkFence *fence, bool isSignaled);
	void DestroyFence(VkFence fence);

	bool ResetFences(const std::vector<VkFence> &fences);
	bool WaitForFences(const std::vector<VkFence> &fences,
		bool waitForAll, 
		uint64_t timeout);

	bool SubmitCommandBuffers(uint32_t queueIndex, 
		const std::vector<VkCommandBuffer> &buffers,
		const std::vector<VkSemaphore> &waitSemaphores,
		const std::vector<VkPipelineStageFlags> &waitStates,
		const std::vector<VkSemaphore> &signaledSemaphores,
		VkFence fence);

	bool CreateBuffer(VkBufferUsageFlags usage,
		VkDeviceSize size, 
		VkMemoryPropertyFlags propertyFlags,
		VkBuffer *buffer,
		VkDeviceMemory * memory);

	void DestroyBuffer(VkBuffer buffer, VkDeviceMemory memory);

	void SetBuffersMemoryBarrier(VkCommandBuffer commandBuffer,
		const std::vector<BufferMemoryTransition> &bufferTransitions,
		VkPipelineStageFlags generatingStages,
		VkPipelineStageFlags consumingStages);

	bool CreateBufferView(VkBuffer buffer, 		 
		VkFormat format, 
		VkDeviceSize offset,
		VkDeviceSize size, 
		VkBufferView *view);

	void DestroyBufferView(VkBufferView view);

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

	void DestroyImage(VkImage image, VkDeviceMemory memory);

	void SetImagesMemoryBarrier(VkCommandBuffer commandBuffer,
		const std::vector<ImageMemoryTransition> &imageTransitions,
		VkPipelineStageFlags generatingStages,
		VkPipelineStageFlags consumingStages);

	bool CreateImageView(VkImage image, 
		VkImageViewType type, 
		VkFormat format, 
		VkImageAspectFlags flags, 
		VkImageView *view);

	void DestroyImageView(VkImageView view);

	bool MapMemory(VkDeviceMemory memory, VkDeviceSize offset, 
		VkDeviceSize size, void **localData);

	bool UnmapMemory(VkDeviceMemory memory, VkDeviceSize offset,
		VkDeviceSize size);

	bool CreateSampler(VkFilter magFilter,
		VkFilter minFilter,
		VkSamplerMipmapMode mipMapMode,
		VkSamplerAddressMode addressModeU,
		VkSamplerAddressMode addressModeV,
		VkSamplerAddressMode addressModeW,
		float lodBias,
		VkBool32 enableAnisotropy,
		float maxAnisotropy,
		VkBool32 enableCompare,
		VkCompareOp compareOp,
		float minLod,
		float maxLod,
		VkBorderColor borderColor,
		VkBool32 unnormalizedCoords,
		VkSampler *sampler);

	bool CreateSampledImage(VkImageType type,
		bool cubemap,
		bool linearFiltering,
		VkFormat format,
		VkExtent3D size,
		uint32_t numMipMaps,
		uint32_t numLayers,
		VkImageUsageFlags usage,
		VkImageViewType viewType, 
		VkImageAspectFlags aspectFlags,
		VkDeviceMemory *memory,
		VkImage *image,
		VkImageView *view);

	bool CreateUniformTexelBuffer(VkBufferUsageFlags usage,
		VkFormat format,
		VkDeviceSize size,
		VkBuffer *buffer,
		VkDeviceMemory *memory,
		VkBufferView *view);

	bool CreateStorageTexelBuffer(VkBufferUsageFlags usage,
		VkFormat format,
		VkDeviceSize size,
		VkBuffer *buffer,
		VkDeviceMemory *memory,
		VkBufferView *view);

	bool CreateUniformBuffer(VkBufferUsageFlags usage,
		VkDeviceSize size,
		VkBuffer *buffer,
		VkDeviceMemory *memory);

	bool CreateStorageBuffer(VkBufferUsageFlags usage,
		VkDeviceSize size,
		VkBuffer *buffer,
		VkDeviceMemory *memory);

	bool CreateInputAttachment(VkImageType type,
		VkFormat format,
		VkExtent3D size,
		VkImageUsageFlags usage,
		VkMemoryPropertyFlags propertyFlags,
		VkImageViewType viewType,
		VkImageAspectFlags flags,
		VkImage *image,
		VkDeviceMemory *memory,
		VkImageView *view);

private:

	bool IsInstanceExtensionSupported(const std::string &extension);
	bool IsDeviceExtensionSupported(const std::string &extension);
	bool IsQueueFamilySupportsPresentation(uint32_t index);
};
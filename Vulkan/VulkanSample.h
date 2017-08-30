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

	bool CreateVulkanWindow(uint32_t width, uint32_t height, const std::string &title = "Vulkan Window");
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

private:

	bool IsInstanceExtensionSupported(const std::string &extension);
	bool IsDeviceExtensionSupported(const std::string &extension);
	bool IsQueueFamilySupportsPresentation(uint32_t index);
};
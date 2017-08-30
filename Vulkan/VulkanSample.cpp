#include "VulkanSample.h"

VulkanSample::VulkanSample()
	: mLogger("VulkanSample.log"),
	mVulkanInstance(nullptr),
	mPhysicalDevice(nullptr),
	mDevice(nullptr),
	mPresentationSurface(nullptr),
	mOldSwapChain(VK_NULL_HANDLE),
	mSwapChain(VK_NULL_HANDLE)
{
}

VulkanSample::~VulkanSample()
{
	Destroy();
}

bool VulkanSample::Initialize()
{
	if (!mLogger.Open())
		return false;

	return true;
}

void VulkanSample::Destroy()
{
	mLogger.Close();
}

bool VulkanSample::PopulateInstanceExtensions()
{
	uint32_t extensionCount = 0;
	VkResult result = VK_SUCCESS;

	result = vkEnumerateInstanceExtensionProperties(
		nullptr,
		&extensionCount,
		nullptr);

	if (result != VK_SUCCESS || extensionCount == 0)
	{
		LOG_ERROR(_T("Unable to get instance extension properties count"));
		return false;
	}

	mInstanceExtensions.resize(extensionCount);

	result = vkEnumerateInstanceExtensionProperties(
		nullptr,
		&extensionCount,
		&mInstanceExtensions[0]
	);

	if (result != VK_SUCCESS || extensionCount == 0)
	{
		LOG_ERROR(_T("Unable to enumerate instance extension properties"));
		return false;
	}

	return true;
}

void VulkanSample::LogInstanceExtensions()
{
	if (mInstanceExtensions.size() == 0)
		PopulateInstanceExtensions();

	LOG_INFO(_T("Instance Extension Properties:"));
	
	for (const auto& extensionProperty : mInstanceExtensions)
	{
		LOG_INFO(_T("Name: %s, Version: %d"), extensionProperty.extensionName, 
			extensionProperty.specVersion);
	}
}

bool VulkanSample::PopulatePhysicalDevices()
{
	VkResult result = VK_SUCCESS;
	uint32_t deviceCount = 0;

	result = vkEnumeratePhysicalDevices(
		mVulkanInstance, 
		&deviceCount, 
		nullptr);

	if (result != VK_SUCCESS || deviceCount == 0)
	{
		LOG_ERROR("Unable to get Vulkan physical devices count");
		return false;
	}

	mDevices.resize(deviceCount);

	result = vkEnumeratePhysicalDevices(
		mVulkanInstance,
		&deviceCount,
		&mDevices[0]);

	if (result != VK_SUCCESS || deviceCount == 0)
	{
		LOG_ERROR("Unable to get Vulkan physical devices");
		return false;
	}

	mPhysicalDevice = mDevices[0];

	return true;
}

bool VulkanSample::IsInstanceExtensionSupported(const std::string &extension)
{
	for (const auto& supportedExtension : mInstanceExtensions)
	{
		if (strcmp(extension.c_str(), supportedExtension.extensionName) == 0)
			return true;
	}

	return false;
}

bool VulkanSample::CreateVulkanInstance(const std::vector<char*> &desiredExtensions)
{
	VkResult result = VK_SUCCESS;

	if (mInstanceExtensions.size() == 0)
		PopulateInstanceExtensions();

	for (const auto& extension : desiredExtensions)
	{
		if (!IsInstanceExtensionSupported(extension))
		{
			LOG_ERROR("Extension %s is not supported", extension);
			return false;
		}
	}

	VkApplicationInfo applInfo =
	{
		VK_STRUCTURE_TYPE_APPLICATION_INFO,
		nullptr,
		"Vulkan Sample",
		VK_MAKE_VERSION(1, 0, 0),
		"Vulkan Engine",
		VK_MAKE_VERSION(1, 0, 0),
		VK_MAKE_VERSION(1, 0, 0)
	};

	VkInstanceCreateInfo instanceInfo =
	{
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		nullptr,
		0,
		&applInfo,
		0,
		nullptr,
		static_cast<uint32_t>(desiredExtensions.size()),
		desiredExtensions.size() > 0 ? &desiredExtensions[0] : nullptr
	};

	result = vkCreateInstance(&instanceInfo, nullptr, &mVulkanInstance);
	
	if (result != VK_SUCCESS || mVulkanInstance == nullptr)
	{
		LOG_ERROR(_T("Unable to create Vulkan instance"));
		return false;
	}

	return true;
}

void VulkanSample::DestroyVulkanInstance()
{
	if (mVulkanInstance)	
		vkDestroyInstance(mVulkanInstance, nullptr);
	
	mVulkanInstance = nullptr;
}

bool VulkanSample::PopulateDeviceExtensions()
{
	uint32_t extensionCount = 0;
	VkResult result = VK_SUCCESS;

	result = vkEnumerateDeviceExtensionProperties(
		mPhysicalDevice,
		nullptr,
		&extensionCount,
		nullptr);

	if (result != VK_SUCCESS || extensionCount == 0)
	{
		LOG_ERROR(_T("Unable to get device extension properties count"));
		return false;
	}

	mDeviceExtensions.resize(extensionCount);

	result = vkEnumerateDeviceExtensionProperties(
		mPhysicalDevice,
		nullptr,
		&extensionCount,
		&mDeviceExtensions[0]
	);

	if (result != VK_SUCCESS || extensionCount == 0)
	{
		LOG_ERROR(_T("Unable to enumerate device extension properties"));
		return false;
	}

	return true;
}

void VulkanSample::LogDeviceExtensions()
{
	if (mDeviceExtensions.size() == 0)
		PopulateDeviceExtensions();

	LOG_INFO(_T("Device Extension Properties:"));

	for (const auto& extensionProperty : mDeviceExtensions)
	{
		LOG_INFO(_T("Name: %s, Version: %d"), extensionProperty.extensionName,
			extensionProperty.specVersion);
	}
}

void VulkanSample::PopulatePhysicalDeviceFeaturesAndProperties()
{
	vkGetPhysicalDeviceFeatures(mPhysicalDevice, &mPhysicalDeviceFeatures);
	vkGetPhysicalDeviceProperties(mPhysicalDevice, &mPhysicalDeviceProperties);
}

void VulkanSample::LogPhysicalDeviceProperties()
{
	LOG_INFO("Physical device name: %s", mPhysicalDeviceProperties.deviceName);
}

bool VulkanSample::PopulateQueueFamilyProperties()
{
	VkResult result = VK_SUCCESS;
	uint32_t queueFamilyCount = 0;

	vkGetPhysicalDeviceQueueFamilyProperties(
		mPhysicalDevice,
		&queueFamilyCount,
		nullptr
	);

	if (queueFamilyCount == 0)
	{
		LOG_ERROR("Unable to get queue family properties count");
		return false;
	}

	mQueueFamilyProperties.resize(queueFamilyCount);

	vkGetPhysicalDeviceQueueFamilyProperties(
		mPhysicalDevice,
		&queueFamilyCount,
		&mQueueFamilyProperties[0]
	);

	if (queueFamilyCount == 0 || mQueueFamilyProperties.size() == 0)
	{
		LOG_ERROR("Unable to get queue family properties");
		return false;
	}

	return true;
}

bool VulkanSample::SelectQueueFamily(VkQueueFlags desiredType, bool supportPresentation)
{
	if (mQueueFamilyProperties.size() == 0)
		PopulateQueueFamilyProperties();

	for (uint32_t index = 0; index < static_cast<uint32_t>(mQueueFamilyProperties.size()); index++)
	{
		if (mQueueFamilyProperties[index].queueCount > 0 &&
			(mQueueFamilyProperties[index].queueFlags & desiredType) != 0)
		{
			if (supportPresentation && !IsQueueFamilySupportsPresentation(index))
				continue;

			mQueueFamilyIndex = index;
			return true;
		}
	}

	return false;
}

bool VulkanSample::IsDeviceExtensionSupported(const std::string &extension)
{
	for (const auto& supportedExtension : mDeviceExtensions)
	{
		if (strcmp(extension.c_str(), supportedExtension.extensionName) == 0)
			return true;
	}

	return false;
}

bool VulkanSample::CreateDevice(const std::vector<char*>& desiredExtensions,
	const std::vector<float>& desiredQueuePriorities)
{
	for (const auto& extension : desiredExtensions)
	{
		if (!IsDeviceExtensionSupported(extension))
		{
			LOG_ERROR("Device extension %s is not supported", extension);
			return false;
		}
	}

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos =
	{
		VkDeviceQueueCreateInfo
		{
			VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			nullptr,
			0,
			mQueueFamilyIndex,
			static_cast<uint32_t>(desiredQueuePriorities.size()),
			desiredQueuePriorities.size() > 0 ? &desiredQueuePriorities[0] : nullptr
		}
	};

	VkDeviceCreateInfo deviceCreateInfo =
	{
		VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		nullptr,
		0,
		static_cast<uint32_t>(queueCreateInfos.size()),
		queueCreateInfos.size() > 0 ? &queueCreateInfos[0] : nullptr,
		0,
		nullptr,
		static_cast<uint32_t>(desiredExtensions.size()),
		desiredExtensions.size() > 0 ? &desiredExtensions[0] : nullptr,
		&mPhysicalDeviceFeatures
	};

	VkResult result = vkCreateDevice(
		mPhysicalDevice, 
		&deviceCreateInfo, 
		nullptr, 
		&mDevice);
	 
	if (result != VK_SUCCESS || mDevice == nullptr)
	{
		LOG_ERROR(_T("Unable to create device"));
		return false;
	}

	return true;
}

void VulkanSample::DestroyDevice()
{
	if (mDevice)
		vkDestroyDevice(mDevice, nullptr);

	mDevice = nullptr;
}

bool VulkanSample::GetQueues(uint32_t queueCount)
{	
	mQueues.resize(queueCount);

	for (uint32_t index = 0; index < queueCount; index++)
	{
		vkGetDeviceQueue(mDevice, mQueueFamilyIndex, 
			index, &mQueues[index]);

		if (mQueues[index] == nullptr)
		{
			LOG_ERROR("Unable to get queue[%d]", index);
			return false;
		}
	}

	return true;
}

bool VulkanSample::CreateVulkanWindow(uint32_t width, uint32_t height, const std::string & title)
{
	return mWindow.Create(width, height, title);
}

void VulkanSample::ShowVulkanWindow()
{
	mWindow.Show();
}

void VulkanSample::DestroyVulkanWindow()
{
	mWindow.Destroy();
}

bool VulkanSample::CreatePresentationSurface()
{
	VkResult result = VK_SUCCESS;

	VkWin32SurfaceCreateInfoKHR surfaceInfo =
	{
		VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
		nullptr,
		0,
		GetModuleHandle(nullptr),
		mWindow.GetHandle()
	};

	result = vkCreateWin32SurfaceKHR(
		mVulkanInstance,
		&surfaceInfo,
		nullptr,
		&mPresentationSurface);

	if (result != VK_SUCCESS || mPresentationSurface == nullptr)
	{
		LOG_ERROR("Unable to create presentation surface");
		return false;
	}

	result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
		mPhysicalDevice,
		mPresentationSurface,
		&mPresentationSurfaceCapabilities
	);

	if (result != VK_SUCCESS)
	{
		LOG_ERROR("Unable to get presentation surface capabilities");
		return false;
	}

	mNumSwapChainImages = mPresentationSurfaceCapabilities.minImageCount + 1;
	if (mPresentationSurfaceCapabilities.maxImageCount > 0 &&
		mNumSwapChainImages > mPresentationSurfaceCapabilities.maxImageCount)
	{
		mNumSwapChainImages = mPresentationSurfaceCapabilities.maxImageCount;
	}

	if (mPresentationSurfaceCapabilities.currentExtent.width == 0xFFFFFFFF)
	{
		mSwapChainImageSize =
		{
			mWindow.GetWidth(),
			mWindow.GetHeight()
		};

		if (mSwapChainImageSize.width < mPresentationSurfaceCapabilities.minImageExtent.width)
			mSwapChainImageSize.width = mPresentationSurfaceCapabilities.minImageExtent.width;

		if (mSwapChainImageSize.width > mPresentationSurfaceCapabilities.maxImageExtent.width)
			mSwapChainImageSize.width = mPresentationSurfaceCapabilities.maxImageExtent.width;

		if (mSwapChainImageSize.height < mPresentationSurfaceCapabilities.minImageExtent.height)
			mSwapChainImageSize.height = mPresentationSurfaceCapabilities.minImageExtent.height;

		if (mSwapChainImageSize.height > mPresentationSurfaceCapabilities.maxImageExtent.height)
			mSwapChainImageSize.height = mPresentationSurfaceCapabilities.maxImageExtent.height;
	}
	else
	{
		mSwapChainImageSize = mPresentationSurfaceCapabilities.currentExtent;
	}

	return true;
}

void VulkanSample::DestroyPresentationSurface()
{
	if (mPresentationSurface)
		vkDestroySurfaceKHR(mVulkanInstance, mPresentationSurface, nullptr);

	mPresentationSurface = nullptr;
}

bool VulkanSample::IsQueueFamilySupportsPresentation(uint32_t index)
{
	VkBool32 supported = VK_FALSE;

	VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(
		mPhysicalDevice,
		index,
		mPresentationSurface,
		&supported
	);

	if (result == VK_SUCCESS && supported == VK_TRUE)
		return true;

	return false;
}

bool VulkanSample::PopulatePresentModes()
{
	VkResult result = VK_SUCCESS;
	uint32_t presentModeCount = 0;

	result = vkGetPhysicalDeviceSurfacePresentModesKHR(
		mPhysicalDevice,
		mPresentationSurface,
		&presentModeCount,
		nullptr
	);

	if (result != VK_SUCCESS || presentModeCount == 0)
	{
		LOG_ERROR("Unable to get present mode count");
		return false;
	}

	mPresentModes.resize(presentModeCount);

	result = vkGetPhysicalDeviceSurfacePresentModesKHR(
		mPhysicalDevice,
		mPresentationSurface,
		&presentModeCount,
		&mPresentModes[0]
	);

	if (result != VK_SUCCESS || presentModeCount == 0)
	{
		LOG_ERROR("Unable to get present modes");
		return false;
	}

	return true;
}

bool VulkanSample::SelectPresentMode(VkPresentModeKHR presentMode)
{
	if (mPresentModes.size() == 0)
		PopulatePresentModes();

	for (const auto& supportedMode : mPresentModes)
	{
		if (supportedMode == presentMode)
		{
			mPresentMode = presentMode;
			return true;
		}
	}

	return false;
}

bool VulkanSample::PopulatePresentationSurfaceFormats()
{
	VkResult result = VK_SUCCESS;
	uint32_t surfaceFormatCount = 0;

	result = vkGetPhysicalDeviceSurfaceFormatsKHR(
		mPhysicalDevice,
		mPresentationSurface,
		&surfaceFormatCount,
		nullptr
	);

	if (result != VK_SUCCESS || surfaceFormatCount == 0)
	{
		LOG_ERROR("Unable to get presentation surface format count");
		return false;
	}

	mPresentationSurfaceFormats.resize(surfaceFormatCount);

	result = vkGetPhysicalDeviceSurfaceFormatsKHR(
		mPhysicalDevice,
		mPresentationSurface,
		&surfaceFormatCount,
		&mPresentationSurfaceFormats[0]
	);

	if (result != VK_SUCCESS || surfaceFormatCount == 0)
	{
		LOG_ERROR("Unable to get presentation surface formats");
		return false;
	}

	return true;
}

bool VulkanSample::SelectPresentationSurfaceFormat(VkSurfaceFormatKHR surfaceFormat)
{
	if (mPresentationSurfaceFormats.size() == 0)
		PopulatePresentationSurfaceFormats();

	if (mPresentationSurfaceFormats.size() == 1 &&
		mPresentationSurfaceFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		mPresentationSurfaceFormat = mPresentationSurfaceFormats[0];
		return true;
	}

	for (const auto& supportedFormat : mPresentationSurfaceFormats)
	{
		if (supportedFormat.format == surfaceFormat.format &&
			supportedFormat.colorSpace == surfaceFormat.colorSpace)
		{
			mPresentationSurfaceFormat = surfaceFormat;
			return true;
		}
	}

	return false;
}

bool VulkanSample::CreateSwapChain()
{
	VkResult result = VK_SUCCESS;

	VkSwapchainCreateInfoKHR swapChainCreateInfo =
	{
		VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		nullptr,
		0,
		mPresentationSurface,
		mNumSwapChainImages,
		mPresentationSurfaceFormat.format,
		mPresentationSurfaceFormat.colorSpace,
		mSwapChainImageSize,
		1,
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_SHARING_MODE_EXCLUSIVE,
		0,
		nullptr,
		VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		mPresentMode,
		VK_TRUE,
		mOldSwapChain
	};

	result = vkCreateSwapchainKHR(
		mDevice,
		&swapChainCreateInfo,
		nullptr,
		&mSwapChain
	);

	if (result != VK_SUCCESS || mSwapChain == nullptr)
	{
		LOG_ERROR("Unable to create Swapchain");
		return false;
	}

	if (mOldSwapChain != VK_NULL_HANDLE)
	{
		vkDestroySwapchainKHR(mDevice, mOldSwapChain, nullptr);
		mOldSwapChain = nullptr;
	}
	
	uint32_t swapChainImageCount = 0;

	result = vkGetSwapchainImagesKHR(
		mDevice,
		mSwapChain,
		&swapChainImageCount,
		nullptr
	);

	if (result != VK_SUCCESS || swapChainImageCount == 0)
	{
		LOG_ERROR("Unable to get Swapchain image count");
		return false;
	}

	mSwapChainImages.resize(swapChainImageCount);

	result = vkGetSwapchainImagesKHR(
		mDevice,
		mSwapChain,
		&swapChainImageCount,
		&mSwapChainImages[0]
	);

	if (result != VK_SUCCESS || swapChainImageCount == 0)
	{
		LOG_ERROR("Unable to get Swapchain images");
		return false;
	}

	return true;
}

void VulkanSample::DestroySwapChain()
{
	if (mSwapChain)
		vkDestroySwapchainKHR(mDevice, mSwapChain, nullptr);

	mSwapChain = nullptr;
}

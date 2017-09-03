#include "VulkanSample.h"

VulkanSample::VulkanSample()
	: mLogger("VulkanSample.log"),
	mVulkanInstance(nullptr),
	mPhysicalDevice(nullptr),
	mDevice(nullptr),
	mPresentationSurface(nullptr),
	mOldSwapChain(VK_NULL_HANDLE),
	mSwapChain(VK_NULL_HANDLE),
	mCommandPool(nullptr)
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
	vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &mPhysicalDeviceMemoryProperties);
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

bool VulkanSample::CreateCommandPool(VkCommandPoolCreateFlags createFlags)
{
	VkResult result = VK_SUCCESS;

	VkCommandPoolCreateInfo createInfo =
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		nullptr,
		createFlags,
		mQueueFamilyIndex
	};

	result = vkCreateCommandPool(
		mDevice,
		&createInfo,
		nullptr,
		&mCommandPool
	);

	if (result != VK_SUCCESS || mCommandPool == nullptr)
	{
		LOG_ERROR("Unable to create Command pool");
		return false;
	}

	return true;
}

void VulkanSample::DestroyCommandPool()
{
	if (mCommandPool)
		vkDestroyCommandPool(mDevice, mCommandPool, nullptr);

	mCommandPool = nullptr;
}

bool VulkanSample::ResetCommandPool(bool releaseResources)
{
	VkResult result = vkResetCommandPool(
		mDevice,
		mCommandPool,
		releaseResources ? VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT : 0
	);

	if (result != VK_SUCCESS)
	{
		LOG_ERROR("Unable to reset Command pool");
		return false;
	}

	return true;
}

bool VulkanSample::AllocateCommandBuffers(uint32_t count, VkCommandBufferLevel level, std::vector<VkCommandBuffer> &buffers)
{
	VkResult result = VK_SUCCESS;	

	VkCommandBufferAllocateInfo allocateInfo =
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		nullptr,
		mCommandPool,
		level,
		count
	};

	buffers.resize(count);

	result = vkAllocateCommandBuffers(
		mDevice,
		&allocateInfo,
		&buffers[0]
	);

	if (result != VK_SUCCESS)
	{
		LOG_ERROR("Unable to create command buffers");
		return false;
	}

	return true;
}

void VulkanSample::FreeCommandBuffers(const std::vector<VkCommandBuffer>& buffers)
{
	vkFreeCommandBuffers(
		mDevice,
		mCommandPool,
		static_cast<uint32_t>(buffers.size()),
		&buffers[0]
	);
}

bool VulkanSample::BeginCommandBuffer(VkCommandBuffer buffer, VkCommandBufferLevel level, 
	VkCommandBufferUsageFlags usage, VkCommandBufferInheritanceInfo *inheritenceInfo)
{
	VkResult result = VK_SUCCESS;

	VkCommandBufferBeginInfo beginInfo =
	{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		nullptr,
		usage,
		inheritenceInfo
	};

	result = vkBeginCommandBuffer(
		buffer,
		&beginInfo
	);

	if (result != VK_SUCCESS)
	{
		LOG_ERROR("Unable to begin Command buffer");
		return false;
	}

	return true;
}

bool VulkanSample::EndCommandBuffer(VkCommandBuffer buffer)
{
	VkResult result = vkEndCommandBuffer(buffer);

	if (result != VK_SUCCESS)
	{
		LOG_ERROR("Unable to end Command buffer");
		return false;
	}

	return true;
}

bool VulkanSample::ResetCommandBuffer(VkCommandBuffer buffer, bool releaseResources)
{
	VkResult result = vkResetCommandBuffer(
		buffer,
		releaseResources ? VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT : 0
	);

	if (result != VK_SUCCESS)
	{
		LOG_ERROR("Unable to reset Command buffer");
		return false;
	}

	return true;
}

bool VulkanSample::CreateVulkanSemaphore(VkSemaphore * semaphore)
{
	VkResult result = VK_SUCCESS;

	VkSemaphoreCreateInfo createInfo =
	{
		VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		nullptr,
		0
	};

	result = vkCreateSemaphore(
		mDevice,
		&createInfo,
		nullptr,
		semaphore
	);

	if (result != VK_SUCCESS || *semaphore == nullptr)
	{
		LOG_ERROR("Unable to create Semaphore");
		return false;
	}

	return true;
}

void VulkanSample::DestroyVulkanSemaphore(VkSemaphore semaphore)
{
	vkDestroySemaphore(
		mDevice,
		semaphore,
		nullptr
	);
}

bool VulkanSample::CreateFence(VkFence * fence, bool isSignaled)
{
	VkResult result = VK_SUCCESS;

	VkFenceCreateInfo createInfo =
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		nullptr,
		isSignaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0ul
	};

	result = vkCreateFence(
		mDevice,
		&createInfo,
		nullptr,
		fence
	);

	if (result != VK_SUCCESS || *fence == nullptr)
	{
		LOG_ERROR("Unable to create Fence");
		return false;
	}

	return true;
}

void VulkanSample::DestroyFence(VkFence fence)
{
	vkDestroyFence(
		mDevice,
		fence,
		nullptr
	);
}

bool VulkanSample::ResetFences(const std::vector<VkFence>& fences)
{
	if (fences.size() > 0)
	{
		VkResult result = vkResetFences(
			mDevice,
			static_cast<uint32_t>(fences.size()),
			&fences[0]
		);

		if (result != VK_SUCCESS)
		{
			LOG_ERROR("Unable to reset fences");
			return false;
		}

		return true;
	}

	return false;
}

bool VulkanSample::WaitForFences(const std::vector<VkFence>& fences, bool waitForAll, uint64_t timeout)
{
	if (fences.size() > 0)
	{
		VkResult result = vkWaitForFences(
			mDevice,
			static_cast<uint32_t>(fences.size()),
			&fences[0],
			waitForAll ? VK_TRUE : VK_FALSE,
			timeout
		);

		if (result != VK_SUCCESS)
		{
			LOG_ERROR("Waiting for fences failed");
			return false;
		}

		return true;
	}

	return false;
}

bool VulkanSample::SubmitCommandBuffers(uint32_t queueIndex,
	const std::vector<VkCommandBuffer>& buffers,
	const std::vector<VkSemaphore>& waitSemaphores, 
	const std::vector<VkPipelineStageFlags>& waitStates, 
	const std::vector<VkSemaphore>& signaledSemaphores, 
	VkFence fence)
{
	VkResult result = VK_SUCCESS;

	VkSubmitInfo submitInfo =
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,
		nullptr,
		static_cast<uint32_t>(waitSemaphores.size()),
		waitSemaphores.size() > 0 ? &waitSemaphores[0] : nullptr,
		waitStates.size() > 0 ? &waitStates[0] : nullptr,
		static_cast<uint32_t>(buffers.size()),
		buffers.size() > 0 ? &buffers[0] : nullptr,
		static_cast<uint32_t>(signaledSemaphores.size()),
		signaledSemaphores.size() > 0 ? &signaledSemaphores[0] : nullptr
	};

	result = vkQueueSubmit(
		mQueues[queueIndex],
		1,
		&submitInfo,
		fence
	);

	if (result != VK_SUCCESS)
	{
		LOG_ERROR("Unable to submit Command buffers");
		return false;
	}

	return true;
}

bool VulkanSample::CreateBuffer( 
	VkBufferUsageFlags usage, 
	VkDeviceSize size, 
	VkMemoryPropertyFlags propertyFlags, 
	VkBuffer * buffer,
	VkDeviceMemory * memory)
{
	VkResult result = VK_SUCCESS;

	VkBufferCreateInfo createInfo =
	{
		VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		nullptr,
		0,
		size,
		usage,
		VK_SHARING_MODE_EXCLUSIVE,
		0,
		nullptr
	};

	result = vkCreateBuffer(
		mDevice,
		&createInfo,
		nullptr,
		buffer
	);

	if (result != VK_SUCCESS || *buffer == nullptr)
	{
		LOG_ERROR("Unable to create buffer");
		return false;
	}

	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(
		mDevice,
		*buffer,
		&memoryRequirements
	);
		
	for (uint32_t type = 0; type < mPhysicalDeviceMemoryProperties.memoryTypeCount; type++)
	{
		if (memoryRequirements.memoryTypeBits & (1 << type))
		{
			if (mPhysicalDeviceMemoryProperties.memoryTypes[type].propertyFlags & propertyFlags)
			{
				VkMemoryAllocateInfo allocateInfo =
				{
					VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
					nullptr,
					memoryRequirements.size,
					type
				};

				result = vkAllocateMemory(
					mDevice,
					&allocateInfo,
					nullptr,
					memory
				);

				if (result == VK_SUCCESS)
					break;
			}
		}
	}

	if (*memory == nullptr)
	{
		LOG_ERROR("Unable to allocate memory for buffer");
		return false;
	}

	result = vkBindBufferMemory(
		mDevice,
		*buffer,
		*memory,
		0
	);

	if (result != VK_SUCCESS)
	{
		LOG_ERROR("Unable to bind memory to buffer");
		return false;
	}

	return true;
}

void VulkanSample::DestroyBuffer(VkBuffer buffer, VkDeviceMemory memory)
{
	if (memory)
	{
		vkFreeMemory(
			mDevice,
			memory,
			nullptr
		);
	}		

	if (buffer)
	{
		vkDestroyBuffer(
			mDevice,
			buffer,
			nullptr
		);
	}
}

void VulkanSample::SetBuffersMemoryBarrier(VkCommandBuffer commandBuffer, 
	const std::vector<BufferMemoryTransition> &bufferTransitions,
	VkPipelineStageFlags generatingStages, 
	VkPipelineStageFlags consumingStages)
{
	std::vector<VkBufferMemoryBarrier> memoryBarriers;

	for (uint32_t index = 0; index < static_cast<uint32_t>(bufferTransitions.size()); index++)
	{
		memoryBarriers.push_back({
			VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
			nullptr,
			bufferTransitions[index].CurrentAccess,
			bufferTransitions[index].NewAccess,
			VK_QUEUE_FAMILY_IGNORED,
			VK_QUEUE_FAMILY_IGNORED,
			bufferTransitions[index].Buffer,
			0,
			VK_WHOLE_SIZE
		});
	}

	vkCmdPipelineBarrier(
		commandBuffer,
		generatingStages,
		consumingStages,
		0,
		0,
		nullptr,
		static_cast<uint32_t>(memoryBarriers.size()),
		memoryBarriers.size() > 0 ? &memoryBarriers[0] : nullptr,
		0, nullptr
	);
}

bool VulkanSample::CreateBufferView(VkBuffer buffer, 
	VkFormat format, 
	VkDeviceSize offset, 
	VkDeviceSize size,
	VkBufferView * view)
{
	VkResult result = VK_SUCCESS;

	VkBufferViewCreateInfo createInfo =
	{
		VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO,
		nullptr,
		0,
		buffer,
		format,
		offset,
		size
	};

	result = vkCreateBufferView(
		mDevice,
		&createInfo,
		nullptr,
		view
	);

	if (result != VK_SUCCESS || *view == nullptr)
	{
		LOG_ERROR("Unable to create Buffer view");
		return false;
	}

	return true;
}

void VulkanSample::DestroyBufferView(VkBufferView view)
{
	if (view)
		vkDestroyBufferView(mDevice, view, nullptr);
}

bool VulkanSample::CreateImage(VkImageType type, 
	bool cubemap, 
	VkFormat format, 
	VkExtent3D size, 
	uint32_t numMipMaps, 
	uint32_t numLayers, 
	VkSampleCountFlagBits samples, 
	VkImageUsageFlags usage,
	VkMemoryPropertyFlags propertyFlags,
	VkDeviceMemory *memory, 
	VkImage * image)
{
	VkResult result = VK_SUCCESS;

	VkImageCreateInfo createInfo =
	{
		VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		nullptr,
		cubemap ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0ul,
		type,
		format,
		size,
		numMipMaps,
		cubemap ? numLayers * 6 : numLayers,
		samples,
		VK_IMAGE_TILING_OPTIMAL,
		usage,
		VK_SHARING_MODE_EXCLUSIVE,
		0,
		nullptr,
		VK_IMAGE_LAYOUT_UNDEFINED
	};

	result = vkCreateImage(
		mDevice,
		&createInfo,
		nullptr,
		image
	);

	if (result != VK_SUCCESS || *image == nullptr)
	{
		LOG_ERROR("Unable to create image");
		return false;
	}

	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements(
		mDevice,
		*image,
		&memoryRequirements
	);

	for (uint32_t type = 0; type < mPhysicalDeviceMemoryProperties.memoryTypeCount; type++)
	{
		if (memoryRequirements.memoryTypeBits & (1 << type))
		{
			if (mPhysicalDeviceMemoryProperties.memoryTypes[type].propertyFlags & propertyFlags)
			{
				VkMemoryAllocateInfo allocateInfo =
				{
					VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
					nullptr,
					memoryRequirements.size,
					type
				};

				result = vkAllocateMemory(
					mDevice,
					&allocateInfo,
					nullptr,
					memory
				);

				if (result == VK_SUCCESS)
					break;
			}
		}
	}

	if (*memory == nullptr)
	{
		LOG_ERROR("Unable to allocate memory for image");
		return false;
	}

	result = vkBindImageMemory(
		mDevice,
		*image,
		*memory,
		0
	);

	if (result != VK_SUCCESS)
	{
		LOG_ERROR("Unable to bind memory to image");
		return false;
	}

	return true;
}

void VulkanSample::DestroyImage(VkImage image, VkDeviceMemory memory)
{
	if (memory)
	{
		vkFreeMemory(
			mDevice,
			memory,
			nullptr
		);
	}

	if (image)
	{
		vkDestroyImage(
			mDevice, 
			image, 
			nullptr
		);
	}
}

void VulkanSample::SetImagesMemoryBarrier(VkCommandBuffer commandBuffer, 
	const std::vector<ImageMemoryTransition> &imageTransitions,
	VkPipelineStageFlags generatingStages, 
	VkPipelineStageFlags consumingStages)
{
	std::vector<VkImageMemoryBarrier> memoryBarriers;

	for (uint32_t index = 0; index < static_cast<uint32_t>(imageTransitions.size()); index++)
	{
		memoryBarriers.push_back({
			VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			nullptr,
			imageTransitions[index].CurrentAccess,
			imageTransitions[index].NewAccess,
			imageTransitions[index].CurrentLayout,
			imageTransitions[index].NewLayout,
			VK_QUEUE_FAMILY_IGNORED,
			VK_QUEUE_FAMILY_IGNORED,
			imageTransitions[index].Image, 
			{
				imageTransitions[index].AspectFlags,
				0,
				VK_REMAINING_MIP_LEVELS,
				0,
				VK_REMAINING_ARRAY_LAYERS
			}
		});
	}

	vkCmdPipelineBarrier(
		commandBuffer,
		generatingStages,
		consumingStages,
		0,
		0,
		nullptr,
		0,
		nullptr,
		static_cast<uint32_t>(memoryBarriers.size()), 
		memoryBarriers.size() > 0 ? &memoryBarriers[0] : nullptr
	);
}

bool VulkanSample::CreateImageView(VkImage image, VkImageViewType type, VkFormat format, VkImageAspectFlags flags, VkImageView * view)
{
	VkResult result = VK_SUCCESS;

	VkImageViewCreateInfo createInfo =
	{
		VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		nullptr,
		0,
		image,
		type,
		format, 
		{
			VK_COMPONENT_SWIZZLE_R,
			VK_COMPONENT_SWIZZLE_G,
			VK_COMPONENT_SWIZZLE_B,
			VK_COMPONENT_SWIZZLE_A
		},
		{
			flags,
			0,
			VK_REMAINING_MIP_LEVELS,
			0,
			VK_REMAINING_ARRAY_LAYERS
		}
	};

	result = vkCreateImageView(
		mDevice,
		&createInfo,
		nullptr,
		view
	);

	if (result != VK_SUCCESS)
	{
		LOG_ERROR("Unable to create image view");
		return false;
	}

	return true;
}

void VulkanSample::DestroyImageView(VkImageView view)
{
	if (view)
	{
		vkDestroyImageView(
			mDevice,
			view,
			nullptr
		);
	}
}

bool VulkanSample::MapMemory(VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, void ** localData)
{
	VkResult result = vkMapMemory(
		mDevice,
		memory,
		offset,
		size,
		0,
		localData
	);

	if (result != VK_SUCCESS)
	{
		LOG_ERROR("Unable to Map memory");
		return false;
	}

	return true;
}

bool VulkanSample::UnmapMemory(VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size)
{
	VkResult result = VK_SUCCESS;

	VkMappedMemoryRange mappedRange =
	{
		VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
		nullptr,
		memory,
		offset,
		size
	};

	result = vkFlushMappedMemoryRanges(
		mDevice,
		1,
		&mappedRange
	);

	if (result != VK_SUCCESS)
	{
		LOG_ERROR("Unable to flush mapped memory");
		return false;
	}

	return true;
}

bool VulkanSample::CreateSampler(VkFilter magFilter, 
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
	VkSampler *sampler)
{
	VkResult result = VK_SUCCESS;

	VkSamplerCreateInfo createInfo =
	{
		VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		nullptr,
		0,
		magFilter,
		minFilter,
		mipMapMode,
		addressModeU,
		addressModeV,
		addressModeW,
		lodBias,
		enableAnisotropy,
		maxAnisotropy,
		enableCompare,
		compareOp,
		minLod,
		maxLod,
		borderColor,
		unnormalizedCoords
	};

	result = vkCreateSampler(
		mDevice,
		&createInfo,
		nullptr,
		sampler
	);

	if (result != VK_SUCCESS)
	{
		LOG_ERROR("Unable to create Sampler");
		return false;
	}

	return true;
}

bool VulkanSample::CreateSampledImage(VkImageType type, 
	bool cubemap, 
	bool linearFiltering,
	VkFormat format, 
	VkExtent3D size, 
	uint32_t numMipMaps, 
	uint32_t numLayers, 
	VkImageUsageFlags usage,
	VkImageViewType viewType,
	VkImageAspectFlags aspectFlags,
	VkDeviceMemory * memory, 
	VkImage * image, 
	VkImageView * view)
{
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(
		mPhysicalDevice,
		format,
		&formatProperties
	);

	if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT))
	{
		LOG_ERROR("Provided format is not supported for sampled image");
		return false;
	}

	if (linearFiltering)
	{
		if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
		{
			LOG_ERROR("Provided format is not supported for linear sampled image");
			return false;
		}
	}

	bool result = CreateImage(
		type,
		cubemap,
		format,
		size,
		numMipMaps,
		numLayers,
		VK_SAMPLE_COUNT_1_BIT,
		usage | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		memory,
		image
	);

	if (!result)
	{
		LOG_ERROR("Unable to create sampled image");
		return false;
	}

	result = CreateImageView(
		*image,
		viewType,
		format,
		aspectFlags,
		view
	);

	if (!result)
	{
		LOG_ERROR("Unable to create Sampled image view");
		DestroyImage(*image, *memory);

		return false;
	}

	return true;
}

bool VulkanSample::CreateUniformTexelBuffer(VkBufferUsageFlags usage,
	VkFormat format,
	VkDeviceSize size,
	VkBuffer * buffer,
	VkDeviceMemory * memory,
	VkBufferView * view)
{
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(
		mPhysicalDevice,
		format,
		&formatProperties
	);

	if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT))
	{
		LOG_ERROR("Provided format is not supported for Uniform Texel buffer");
		return false;
	}

	bool result = CreateBuffer(
		usage | VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT,
		size,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		buffer,
		memory
	);

	if (!result)
	{
		LOG_ERROR("Unable to create Uniform Texel buffer");
		return false;
	}

	result = CreateBufferView(
		*buffer,
		format,
		0,
		VK_WHOLE_SIZE,
		view
	);

	if (!result)
	{
		LOG_ERROR("Unable to create Uniform Texel buffer view");
		DestroyBuffer(*buffer, *memory);
		return false;
	}

	return true;
}

bool VulkanSample::CreateStorageTexelBuffer(VkBufferUsageFlags usage,
	VkFormat format,
	VkDeviceSize size,
	VkBuffer * buffer,
	VkDeviceMemory * memory,
	VkBufferView * view)
{
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(
		mPhysicalDevice,
		format,
		&formatProperties
	);

	if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT))
	{
		LOG_ERROR("Provided format is not supported for Storage Texel buffer");
		return false;
	}

	bool result = CreateBuffer(
		usage | VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT,
		size,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		buffer,
		memory
	);

	if (!result)
	{
		LOG_ERROR("Unable to create Storage Texel buffer");
		return false;
	}

	result = CreateBufferView(
		*buffer,
		format,
		0,
		VK_WHOLE_SIZE,
		view
	);

	if (!result)
	{
		LOG_ERROR("Unable to create Uniform Texel buffer view");
		DestroyBuffer(*buffer, *memory);
		return false;
	}

	return true;
}

bool VulkanSample::CreateUniformBuffer(VkBufferUsageFlags usage,
	VkDeviceSize size,
	VkBuffer * buffer,
	VkDeviceMemory * memory)
{	
	bool result = CreateBuffer(
		usage | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		size,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		buffer,
		memory
	);

	if (!result)
	{
		LOG_ERROR("Unable to create Uniform buffer");
		return false;
	}

	return true;
}

bool VulkanSample::CreateStorageBuffer(VkBufferUsageFlags usage,
	VkDeviceSize size,
	VkBuffer * buffer,
	VkDeviceMemory * memory)
{
	bool result = CreateBuffer(
		usage | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
		size,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		buffer,
		memory
	);

	if (!result)
	{
		LOG_ERROR("Unable to create Storage buffer");
		return false;
	}

	return true;
}

bool VulkanSample::CreateInputAttachment(VkImageType type, 
	VkFormat format, VkExtent3D size, 
	VkImageUsageFlags usage, 
	VkMemoryPropertyFlags propertyFlags, 
	VkImageViewType viewType, 
	VkImageAspectFlags aspectFlags, 
	VkImage * image, 
	VkDeviceMemory * memory,
	VkImageView * view)
{
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(
		mPhysicalDevice,
		format,
		&formatProperties
	);

	if (aspectFlags & VK_IMAGE_ASPECT_COLOR_BIT)
	{
		if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT))
		{
			LOG_ERROR("Provided format is not supported for Color input attachment");
			return false;
		}
	}

	if ((aspectFlags & VK_IMAGE_ASPECT_DEPTH_BIT) || (aspectFlags & VK_IMAGE_ASPECT_STENCIL_BIT))
	{
		if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT))
		{
			LOG_ERROR("Provided format is not supported for Depth/Stencil input attachment");
			return false;
		}
	}

	bool result = CreateImage(
		type,
		false,
		format,
		size,
		1,
		1,
		VK_SAMPLE_COUNT_1_BIT,
		usage | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		memory,
		image
	);

	if (!result)
	{
		LOG_ERROR("Unable to Create input attachment image");
		return false;
	}

	result = CreateImageView(
		*image,
		viewType,
		format,
		aspectFlags,
		view
	);

	if (!result)
	{
		LOG_ERROR("Unable to Create input attachment image view");
		DestroyImage(*image, *memory);
		return false;
	}

	return true;
}

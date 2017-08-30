#include "VulkanSample.h"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow)
{
	VulkanSample sample;

	if (sample.Initialize())
	{
		sample.PopulateInstanceExtensions();
		sample.LogInstanceExtensions();

		std::vector<char*> desiredInstanceExtensions = 
		{
			VK_KHR_SURFACE_EXTENSION_NAME,
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME
		};

		sample.CreateVulkanInstance(desiredInstanceExtensions);

		sample.PopulatePhysicalDevices();
		sample.PopulatePhysicalDeviceFeaturesAndProperties();
		sample.LogPhysicalDeviceProperties();

		sample.PopulateDeviceExtensions();
		sample.LogDeviceExtensions();
		
		sample.CreateVulkanWindow(800, 600);
		sample.CreatePresentationSurface();

		sample.PopulateQueueFamilyProperties();
		sample.SelectQueueFamily(VK_QUEUE_GRAPHICS_BIT);

		sample.PopulatePresentModes();
		sample.SelectPresentMode(VK_PRESENT_MODE_MAILBOX_KHR);

		VkSurfaceFormatKHR desiredSurfaceFormat =
		{
			VK_FORMAT_B8G8R8A8_UNORM,
			VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
		};

		sample.PopulatePresentationSurfaceFormats();
		sample.SelectPresentationSurfaceFormat(desiredSurfaceFormat);

		std::vector<char*> desiredDeviceExtensions =
		{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME
		};

		std::vector<float> desiredQueuePriorities =
		{
			1.0f,
			0.8f
		};

		sample.CreateDevice(desiredDeviceExtensions, desiredQueuePriorities);
		sample.GetQueues(static_cast<uint32_t>(desiredQueuePriorities.size()));
		sample.CreateSwapChain();

		sample.ShowVulkanWindow();
		while (VulkanWindow::ProcessEvents());

		sample.DestroySwapChain();
		sample.DestroyPresentationSurface();
		sample.DestroyVulkanWindow();
		sample.DestroyDevice();
		sample.DestroyVulkanInstance();
	}

	return 0;
}
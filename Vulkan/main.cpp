#include "VulkanSample.h"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow)
{
	VulkanSample sample;

	if (sample.Initialize())
	{
		sample.PopulateInstanceExtensions();
		sample.LogInstanceExtensions();

		sample.CreateVulkanInstance({
			VK_KHR_SURFACE_EXTENSION_NAME,
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME
		});

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

		sample.PopulatePresentationSurfaceFormats();
		sample.SelectPresentationSurfaceFormat({
			VK_FORMAT_B8G8R8A8_UNORM,
			VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
		});

		sample.CreateDevice({
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME
		}, {1.0f, 0.8f});

		sample.GetQueues(2);

		sample.CreateSwapChain();
		sample.CreateCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);		

		std::vector<VkCommandBuffer> commandBuffers;
		sample.AllocateCommandBuffers(1, VK_COMMAND_BUFFER_LEVEL_PRIMARY, commandBuffers);

		sample.BeginCommandBuffer(commandBuffers[0], VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr);

		sample.EndCommandBuffer(commandBuffers[0]);
		sample.SubmitCommandBuffers(0, commandBuffers, {}, {}, {}, nullptr);

		sample.ShowVulkanWindow();
		while (VulkanWindow::ProcessEvents());

		sample.FreeCommandBuffers(commandBuffers);
		sample.DestroyCommandPool();
		sample.DestroySwapChain();
		sample.DestroyPresentationSurface();
		sample.DestroyVulkanWindow();
		sample.DestroyDevice();
		sample.DestroyVulkanInstance();
	}

	return 0;
}
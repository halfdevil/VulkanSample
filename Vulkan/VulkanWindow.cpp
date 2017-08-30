#include "VulkanWindow.h"
#include "Logger.h"

VulkanWindow::VulkanWindow()
	: mWidth(0), mHeight(0), mHandle(nullptr)
{
}

VulkanWindow::~VulkanWindow()
{
	Destroy();
}

bool VulkanWindow::Create(uint32_t width, uint32_t height, const std::string & title)
{
	HINSTANCE instance = GetModuleHandle(nullptr);

	WNDCLASSEX wndClass =
	{
		sizeof(WNDCLASSEX),
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		sizeof(VulkanWindow),
		instance,
		LoadIcon(nullptr, IDI_APPLICATION),
		LoadCursor(nullptr, IDC_ARROW),
		nullptr,
		nullptr,
		"VulkanWindow",
		LoadIcon(nullptr, IDI_WINLOGO)
	};

	if (!RegisterClassEx(&wndClass))
	{
		LOG_ERROR("Unable to register class: %d", GetLastError());
		return false;
	}

	RECT desktopRect;
	GetWindowRect(GetDesktopWindow(), &desktopRect);

	uint32_t x = (desktopRect.right - desktopRect.left) / 2 - width / 2;
	uint32_t y = (desktopRect.bottom - desktopRect.top) / 2 - height / 2;

	mHandle = CreateWindowEx(
		WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,
		"VulkanWindow",
		title.c_str(),
		WS_OVERLAPPEDWINDOW,
		x,
		y,
		width,
		height,
		nullptr,
		nullptr,
		instance,
		this);

	if (!mHandle)
	{
		LOG_ERROR("Unable to create window: %d", GetLastError());
		return false;
	}

	mWidth = width;
	mHeight = height;
	mTitle = title;

	return true;
}

void VulkanWindow::Destroy()
{
	if (mHandle != nullptr)
		DestroyWindow(mHandle);

	mHandle = nullptr;
}

void VulkanWindow::Show()
{
	ShowWindow(mHandle, SW_SHOW);
}

LRESULT VulkanWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool VulkanWindow::ProcessEvents()
{
	MSG msg;

	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
			return false;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return true;
}


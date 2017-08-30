#pragma once

#include <Windows.h>
#include <string>

class VulkanWindow
{
private:

	std::string		mTitle;
	uint32_t		mWidth;
	uint32_t		mHeight;

	HWND			mHandle;

public:

	VulkanWindow();
	~VulkanWindow();

	const std::string& GetTitle() const
	{
		return mTitle;
	}

	uint32_t GetWidth() const
	{
		return mWidth;
	}

	uint32_t GetHeight() const
	{
		return mHeight;
	}

	HWND GetHandle() const
	{
		return mHandle;
	}

	bool Create(uint32_t width, uint32_t height, const std::string &title = "Vulkan Window");
	void Destroy();
	void Show();

public:

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static bool ProcessEvents();
};
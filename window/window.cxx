#include "window.hxx"
#include <iostream>

#pragma comment(lib, "dwmapi.lib")

window_t::window_t(HINSTANCE instance, const std::string& window_name, const std::string& class_name)
{
	if (auto win32_base = ::LoadLibraryA("win32u.dll"))
		::CloseHandle(win32_base);

	if (auto user32_base = ::LoadLibraryA("user32.dll"))
		::CloseHandle(user32_base);

	this->register_class(class_name);
	this->create_window(instance, window_name, class_name);
}

window_t::~window_t()
{
	if (m_window_handle) ::DestroyWindow(m_window_handle);
}



auto __stdcall window_procedure(HWND window_handle, UINT u_message, WPARAM w_param, LPARAM l_param) -> LRESULT
{
	switch (u_message)
	{
		case WM_CLOSE:
		case WM_DESTROY:
			::PostQuitMessage(0);
		default:
			return DefWindowProcA(window_handle, u_message, w_param, l_param);
	}
}

auto window_t::register_class(const std::string& class_name) -> void
{
	auto window_class = WNDCLASSA{
		CS_HREDRAW | CS_VREDRAW,
		::window_procedure,
		0,
		0,
		nullptr,
		LoadIcon(NULL, IDI_APPLICATION),
		LoadCursor(NULL, IDC_CROSS),
		nullptr, // background overriden by vulkan surface
		nullptr,
		class_name.c_str()
	};

	::RegisterClassA(&window_class);
}

auto window_t::create_window(HINSTANCE instance, const std::string& window_name, const std::string& class_name) -> void
{
	auto ws_ex_style = 0;
	auto ws_style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE;

	m_window_handle = ::CreateWindowExA(
		ws_ex_style,
		class_name.c_str(),
		window_name.c_str(),
		ws_style,
		0,
		0,
		window::res_vec.m_x,
		window::res_vec.m_y,
		nullptr,
		nullptr,
		instance,
		nullptr
	);
	
	auto wnd_rect = RECT{0, 0, window::res_vec.m_x, window::res_vec.m_y};
	
	::AdjustWindowRectEx(&wnd_rect, ws_style, 0, ws_ex_style);
	::SetWindowPos(
		m_window_handle,
		nullptr,
		0,
		0,
		wnd_rect.right - wnd_rect.left,
		wnd_rect.bottom - wnd_rect.top,
		SWP_NOZORDER | SWP_NOMOVE
	);

	::SetFocus(m_window_handle);
}

auto window_t::handle_message() -> bool
{
	while (::PeekMessageA(&m_window_message, 0, 0, 0, PM_REMOVE))
	{
		::TranslateMessage(&m_window_message);
		::DispatchMessageA(&m_window_message);

		if (m_window_message.message == WM_QUIT)
			return 0;
	}
	
	return 1;
}

auto window_t::get_hwnd() -> const HWND
{
	return m_window_handle;
}
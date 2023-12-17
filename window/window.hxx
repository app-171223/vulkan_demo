#pragma once

#include <windows.h>
#include <dwmapi.h>
#include <string>

#include "../draw/utils/constants.hxx"

class window_t
{
	HWND m_window_handle{};
	MSG m_window_message{};
	
public:

	window_t(HINSTANCE instance, const std::string& window_name, const std::string& class_name);

	~window_t();

private:

	auto register_class(const std::string& class_name) -> void;

	auto create_window(HINSTANCE instance, const std::string& window_name, const std::string& class_name) -> void;

public:

	auto handle_message() -> bool;
	
	auto get_hwnd() -> const HWND;
};
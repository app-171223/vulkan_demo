#pragma once

#include <cstdint>
#include <windows.h>
#include <vulkan/vulkan.h>

#include "containers.hxx"

namespace window
{
	//const auto res_vec = point_t{
	//	static_cast<std::int16_t>(::GetSystemMetrics(SM_CXSCREEN)),
	//	static_cast<std::int16_t>(::GetSystemMetrics(SM_CYSCREEN))};

	const auto res_vec = point_t{1280, 720};
	
	const auto res_vk = VkExtent2D{(std::uint32_t)(res_vec.m_x), (std::uint32_t)(res_vec.m_y)};

	const auto center = point_t{window::res_vec.m_x / 2, window::res_vec.m_y / 2};
	const auto bottom = point_t{window::res_vec.m_x / 2, window::res_vec.m_y};

	const auto window_rect = RECT{0, 0, window::res_vec.m_x, window::res_vec.m_y};
}

namespace constants
{
	const auto pi = std::float_t{3.14159265359f};
}
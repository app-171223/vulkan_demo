#pragma once

#define VK_USE_PLATFORM_WIN32_KHR

#include <vector>
#include <string>
#include <chrono>
#include <array>
#include <memory>
#include <windows.h>
#include <vulkan/vulkan.h>

#include "../renderer/renderer.hxx"
#include "../fonts/stb_font_consolas_24_latin1.inl"

namespace draw
{
	class scene_t
	{
		const HWND m_wnd{0};
		renderer_t* m_renderer{nullptr};

		mesh_buffer_t m_meshes{};
		line_buffer_t m_lines{};
		text_buffer_t m_text{};

		point_t m_cursor_pos{};
		
		struct {
			std::uint32_t m_current{0};
			std::uint32_t m_pushed{~0u};
		}m_button;

	public:
		
		scene_t(const HWND window_handle);
		
		~scene_t();
		
		auto mesh(std::vector<vertex_t> points) -> void;

		auto mesh(rect_t rect, color_t color) -> void;

		auto circle(vertex_t center, std::uint8_t sides, std::uint16_t radius) -> void;
		
		auto circle(vertex_t center, std::uint8_t sides, std::uint16_t radius, std::float_t line_width, const color_t* override = nullptr) -> void;

		auto line(vertex_t from, vertex_t to, std::float_t width = 1.0f) -> void;
		
		auto line(const std::vector<vertex_t>& points, std::float_t width = 1.0f, const color_t* override = nullptr) -> void;

		auto text(vertex_t point, const std::string& text, std::float_t size, bool center = false) -> void;

		auto button(rect_t rect, const std::string& label, std::float_t text_size, bool held = false, bool center = false) -> bool;

		auto debug_info(std::uint32_t fps, std::float_t text_size, color_t text_color, bool cursor_pos = false, bool crosshair = false) -> void;

		auto begin() -> void;

		auto end() -> void;

		auto get_cursor() -> point_t;
	};
}
#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <vector>
#include <array>
#include <string>

#include "../../utils/containers.hxx"
#include "../fonts/stb_font_consolas_24_latin1.inl"

struct image_view_t // swap chain image
{
	VkImage m_image;
	VkImageView m_view;
};

struct memory_buffer_t
{
	VkDeviceMemory m_memory;
	VkBuffer m_buffer;
	std::size_t m_size;
};

struct vertex_input_t
{
	VkFormat m_format;
	std::size_t m_offset;
};

struct pipeline_setting_t
{
	std::string m_vertex;
	std::string m_fragment;
	std::size_t m_stride;
	std::vector<vertex_input_t> m_vert_input;
	VkPrimitiveTopology m_topology;
	VkPolygonMode m_polygon_mode;
};

// VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP, VK_POLYGON_MODE_FILL
struct vertex_t
{
	vec2_t<std::float_t> m_pos;
	color_t m_col;

	vertex_t() = default;

	vertex_t(std::int32_t x, std::int32_t y, color_t color)
		: m_pos{(std::float_t)(x), (std::float_t)(y)},
		m_col{color} {	}

	vertex_t(std::float_t x, std::float_t y, color_t color)
		: m_pos{x, y},
		m_col{color} {	}

	auto to_absolute(vec2_t<std::int32_t> res) -> vertex_t
	{
		m_pos.m_x = std::float_t{-1.0f + (static_cast<std::float_t>(this->m_pos.m_x) / static_cast<std::float_t>(res.m_x)) * 2.0f};
		m_pos.m_y = std::float_t{-1.0f + (static_cast<std::float_t>(this->m_pos.m_y) / static_cast<std::float_t>(res.m_y)) * 2.0f};

		return *this;
	}
};

struct mesh_info_t
{
	std::vector<vertex_t> m_vertices;
};

struct mesh_buffer_t
{
	VkPipeline m_pipeline;
	memory_buffer_t m_vertex_buffer;
	std::vector<mesh_info_t> m_info;
};

// VK_PRIMITIVE_TOPOLOGY_LINE_LIST, VK_POLYGON_MODE_LINE
struct line_info_t
{
	std::vector<vertex_t> m_vertices;
	std::float_t m_width;
};

struct line_buffer_t
{
	VkPipeline m_pipeline;
	memory_buffer_t m_vertex_buffer;
	std::vector<line_info_t> m_info;
};

// VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP, VK_POLYGON_MODE_FILL
struct text_vertex_t
{
	vec2_t<std::float_t> m_pos;
	vec2_t<std::float_t> m_uv;
	color_t m_col;
};

struct text_info_t
{
	std::vector<text_vertex_t> m_vertices;
};

struct text_buffer_t
{
	stb_fontchar m_font_data[STB_FONT_consolas_24_latin1_NUM_CHARS];
	VkPipeline m_pipeline;
	memory_buffer_t m_vertex_buffer;
	std::vector<text_info_t> m_info;
};
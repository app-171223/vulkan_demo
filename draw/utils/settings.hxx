#pragma once

#include <vulkan/vulkan_win32.h>
#include <vulkan/vulkan.h>

#include "containers.hxx"
#include "constants.hxx"
#include "../fonts/stb_font_consolas_24_latin1.inl"

namespace draw
{
	namespace settings
	{
		namespace shaders
		{
			const auto mesh_vertex = std::string{"mesh.vert.spv"};
			const auto mesh_fragment = std::string{"mesh.frag.spv"};

			const auto text_vertex = std::string{"text.vert.spv" };
			const auto text_fragment = std::string{"text.frag.spv"};

			const auto entry_point = std::string{"main"};
		}

		namespace pipelines
		{
			const auto mesh = pipeline_setting_t{
				settings::shaders::mesh_vertex,
				settings::shaders::mesh_fragment,
				std::size_t{sizeof(vertex_t)},
				std::vector<vertex_input_t>{
					vertex_input_t{VK_FORMAT_R32G32_SFLOAT, offsetof(vertex_t, m_pos)},
					vertex_input_t{VK_FORMAT_R8G8B8A8_UNORM, offsetof(vertex_t, m_col)}
				},
				VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
				VK_POLYGON_MODE_FILL
			};

			const auto line = pipeline_setting_t{
				settings::shaders::mesh_vertex,
				settings::shaders::mesh_fragment,
				std::size_t{sizeof(vertex_t)},
				std::vector<vertex_input_t>{
					vertex_input_t{VK_FORMAT_R32G32_SFLOAT, offsetof(vertex_t, m_pos)},
					vertex_input_t{VK_FORMAT_R8G8B8A8_UNORM, offsetof(vertex_t, m_col)}
				},
				VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
				VK_POLYGON_MODE_LINE
			};

			const auto text = pipeline_setting_t{
				settings::shaders::text_vertex,
				settings::shaders::text_fragment,
				std::size_t{sizeof(text_vertex_t)},
				std::vector<vertex_input_t>{
					vertex_input_t{VK_FORMAT_R32G32_SFLOAT, offsetof(text_vertex_t, m_pos)},
					vertex_input_t{VK_FORMAT_R32G32_SFLOAT, offsetof(text_vertex_t, m_uv)},
					vertex_input_t{VK_FORMAT_R8G8B8A8_UNORM, offsetof(text_vertex_t, m_col)}
				},
				VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
				VK_POLYGON_MODE_FILL
			};
		}
		
		namespace font
		{
			constexpr auto extent = VkExtent2D{STB_FONT_consolas_24_latin1_BITMAP_WIDTH, STB_FONT_consolas_24_latin1_BITMAP_HEIGHT_POW2};
			constexpr auto first_char = std::uint32_t{STB_FONT_consolas_24_latin1_FIRST_CHAR};
		}

		const auto instance_extensions = std::vector<const char*>{VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME};
		const auto device_extensions = std::vector<const char*>{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
		const auto pipeline_dynamic_states = std::vector<VkDynamicState>{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_LINE_WIDTH};
		
		constexpr auto api_version = std::uint32_t{VK_API_VERSION_1_2};
		constexpr auto queue_priority = std::float_t{0.0f};
		constexpr auto vertex_buffer_offset = VkDeviceSize{0};

		constexpr auto pass_color = VkClearColorValue{0.0f, 0.0f, 0.1f, 1.0f};
		constexpr auto pass_depth = VkClearDepthStencilValue{1.0f, 0};
		
		constexpr auto depth_format = VkFormat{VK_FORMAT_D32_SFLOAT_S8_UINT};
		constexpr auto color_format = VkFormat{VK_FORMAT_B8G8R8A8_UNORM};
		constexpr auto color_space = VkColorSpaceKHR{VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
		constexpr auto stage_mask = VkPipelineStageFlags{VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

		const auto viewport = VkViewport{
			std::float_t{0.0f},
			std::float_t{0.0f},
			static_cast<std::float_t>(window::res_vk.width),
			static_cast<std::float_t>(window::res_vk.height),
			std::float_t{0.0f},
			std::float_t{1.0f}
		};
	}
}
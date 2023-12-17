#pragma once

#define VK_USE_PLATFORM_WIN32_KHR

#include <memory>
#include <windows.h>
#include <vulkan/vulkan.h>

#include "../device/device.hxx"
#include "../pipeline/pipeline.hxx"
#include "../swap_chain/swap_chain.hxx"
#include "../utils/containers.hxx"

namespace draw
{
	class renderer_t
	{
		device_t* m_device{nullptr};
		swap_chain_t* m_swap_chain{nullptr};

		VkRenderPass m_render_pass{nullptr};
		VkPipelineCache m_pipeline_cache{nullptr};
		std::vector<VkFramebuffer> m_frame_buffers{ };

		struct {
			VkImage m_image{nullptr};
			VkDeviceMemory m_memory{nullptr};
			VkImageView m_view{nullptr};
		} m_depth_stencil{ };

		pipeline_t* m_mesh_pipeline{nullptr};
		pipeline_t* m_line_pipeline{nullptr};
		pipeline_t* m_text_pipeline{nullptr};

		std::vector<VkPipeline> m_pipelines{};

		std::array<VkClearValue, 2> m_clear_values{};
		VkCommandBufferBeginInfo m_render_command_buffer_bi{};
		VkRenderPassBeginInfo m_render_pass_bi{};

		std::float_t m_line_width{1.0f};

	public:

		renderer_t(const HWND window_handle, stb_fontchar* font_data);

		~renderer_t();

	private:

		auto setup_depth_stencil() -> void;

		auto create_render_pass() -> void;

		auto create_frame_buffers() -> void;

		auto prepare_render_pass() -> void;

		auto create_vertex_buffer(memory_buffer_t& vertex_buffer) -> void;

	public:
		
		auto allocate_vertices(mesh_buffer_t& mesh_buffer) -> void;
		auto render_vertices(mesh_buffer_t& mesh_buffer) -> void;

		auto allocate_vertices(line_buffer_t& line_buffer) -> void;
		auto render_vertices(line_buffer_t& line_buffer) -> void;

		auto allocate_vertices(text_buffer_t& text_buffer) -> void;
		auto render_vertices(text_buffer_t& text_buffer) -> void;

		auto begin_frame() -> void;

		auto end_frame() -> void;
	};
}

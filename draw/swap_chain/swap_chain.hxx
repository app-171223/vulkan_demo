#pragma once

#define VK_USE_PLATFORM_WIN32_KHR

#include <cstdint>
#include <vector>
#include <chrono>
#include <windows.h>
#include <vulkan/vulkan.h>

#include "../utils/containers.hxx"
#include "../utils/constants.hxx"

namespace draw
{
	class swap_chain_t
	{
		VkInstance m_instance{ nullptr };
		VkDevice m_logical_device{ nullptr };

		VkSurfaceKHR m_surface{ nullptr };
		VkSwapchainKHR m_swap_chain{ nullptr };

		VkCommandPool m_graphics_command_pool{ nullptr };
		std::vector<VkCommandBuffer> m_graphics_command_buffers{ };
		std::vector<image_view_t> m_image_views{ };
		std::vector<VkFence> m_wait_fences{ };

		VkSemaphore m_present_semaphore{ nullptr };
		VkSemaphore m_render_semaphore{ nullptr };

		std::uint64_t m_timeout{ std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(5)).count() };
		std::uint32_t m_buffer_index{ 0 };

		VkSubmitInfo m_submit_info{ };
		VkPresentInfoKHR m_present_info{ };

	public:

		swap_chain_t(VkInstance instance, VkDevice logical_device,
			const HWND window_handle, std::uint32_t graphics_queue_index);

		~swap_chain_t();

	private:

		auto create_surface(const HWND window_handle) -> void;

		auto create_command_pools(std::uint32_t graphics_queue_index) -> void;

		auto create_swap_chain(bool v_sync = false) -> void;

		auto create_image_views() -> void;

		auto allocate_command_buffers() -> void;

		auto create_sync_primitives() -> void;

		auto set_queue_info() -> void;

	public:

		auto get_buffer_index() -> const std::uint32_t;

		auto get_image_views() -> const std::vector<image_view_t>&;

		auto get_render_buffer() -> const VkCommandBuffer;

		auto get_render_fence() -> const VkFence;

		auto get_command_pool() -> const VkCommandPool;

		auto acquire_next_image() -> void;

		auto queue_submit(VkQueue queue) -> void;

		auto queue_present(VkQueue queue) -> void;
	};
}

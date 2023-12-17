#include "swap_chain.hxx"

#include "../utils/error.hxx"
#include "../utils/constants.hxx"
#include "../utils/settings.hxx"
#include "../utils/init.hxx"

draw::swap_chain_t::swap_chain_t(VkInstance instance, VkDevice logical_device,
	const HWND window_handle, std::uint32_t graphics_queue_index)
	: m_instance{ instance },
	m_logical_device{ logical_device }
{
	this->create_surface(window_handle);
	this->create_command_pools(graphics_queue_index);
	this->create_swap_chain();
	this->create_image_views();
	this->allocate_command_buffers();
	this->create_sync_primitives();
	this->set_queue_info();
}

draw::swap_chain_t::~swap_chain_t()
{
	if (m_present_semaphore) ::vkDestroySemaphore(m_logical_device, m_present_semaphore, nullptr);
	if (m_render_semaphore) ::vkDestroySemaphore(m_logical_device, m_render_semaphore, nullptr);

	if (m_swap_chain) for (auto& image_view : m_image_views) ::vkDestroyImageView(m_logical_device, image_view.m_view, nullptr);
	if (m_swap_chain) vkDestroySwapchainKHR(m_logical_device, m_swap_chain, nullptr);
	if (m_surface && m_instance) vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
	if (m_graphics_command_pool) ::vkDestroyCommandPool(m_logical_device, m_graphics_command_pool, nullptr);
}

auto draw::swap_chain_t::create_surface(const HWND window_handle) -> void
{
	auto surface_ci = init::surface_create_info(window_handle);
	vk_check_result(::vkCreateWin32SurfaceKHR(m_instance, &surface_ci, nullptr, &m_surface));
}

auto draw::swap_chain_t::create_command_pools(std::uint32_t graphics_queue_index) -> void
{
	auto graphics_comman_pool_ci = init::command_pool_create_info(graphics_queue_index, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	vk_check_result(::vkCreateCommandPool(m_logical_device, &graphics_comman_pool_ci, nullptr, &m_graphics_command_pool));
}

auto draw::swap_chain_t::create_swap_chain(bool v_sync) -> void
{
	auto swap_chain_ci = init::swap_chain_create_info(m_surface, v_sync, settings::color_format, settings::color_space, window::res_vk);
	vk_check_result(::vkCreateSwapchainKHR(m_logical_device, &swap_chain_ci, nullptr, &m_swap_chain));
}

auto draw::swap_chain_t::create_image_views() -> void
{
	auto image_count = std::uint32_t{ 0 };
	vk_check_result(::vkGetSwapchainImagesKHR(m_logical_device, m_swap_chain, &image_count, 0));

	auto images = std::vector<VkImage>{ image_count };
	vk_check_result(::vkGetSwapchainImagesKHR(m_logical_device, m_swap_chain, &image_count, images.data()));

	m_image_views.resize(image_count);

	for (auto i = std::uint32_t{ 0 }; i < image_count; i++)
	{
		auto image_view_ci = init::image_view_create_info(images.at(i), settings::color_format);

		m_image_views.at(i).m_image = images.at(i);
		vk_check_result(::vkCreateImageView(m_logical_device, &image_view_ci, nullptr, &m_image_views.at(i).m_view));
	}
}

auto draw::swap_chain_t::allocate_command_buffers() -> void
{
	m_graphics_command_buffers.resize(m_image_views.size());
	auto graphics_command_buffer_ai = init::command_buffer_allocate_info(m_graphics_command_pool, m_graphics_command_buffers.size());

	vk_check_result(::vkAllocateCommandBuffers(m_logical_device, &graphics_command_buffer_ai, m_graphics_command_buffers.data()));
}

auto draw::swap_chain_t::create_sync_primitives() -> void
{
	auto semaphore_ci = init::semaphore_create_info();
	vk_check_result(::vkCreateSemaphore(m_logical_device, &semaphore_ci, nullptr, &m_present_semaphore));
	vk_check_result(::vkCreateSemaphore(m_logical_device, &semaphore_ci, nullptr, &m_render_semaphore));

	m_wait_fences.resize(m_graphics_command_buffers.size());
	auto fence_ci = init::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);

	for (auto& fence : m_wait_fences)
		vk_check_result(::vkCreateFence(m_logical_device, &fence_ci, nullptr, &fence))
}

auto draw::swap_chain_t::set_queue_info() -> void
{
	m_submit_info = init::submit_info(m_present_semaphore, settings::stage_mask, m_render_semaphore);
	m_present_info = init::present_info(m_present_semaphore, m_swap_chain, m_buffer_index);
}



auto draw::swap_chain_t::get_buffer_index() -> const std::uint32_t
{
	return m_buffer_index;
}

auto draw::swap_chain_t::get_image_views() -> const std::vector<image_view_t>&
{
	return m_image_views;
}

auto draw::swap_chain_t::get_render_buffer() -> const VkCommandBuffer
{
	return m_graphics_command_buffers.at(m_buffer_index);
}

auto draw::swap_chain_t::get_render_fence() -> const VkFence
{
	return m_wait_fences.at(m_buffer_index);
}

auto draw::swap_chain_t::get_command_pool() -> const VkCommandPool
{
	return m_graphics_command_pool;
}

auto draw::swap_chain_t::acquire_next_image() -> void
{
	::vkAcquireNextImageKHR(m_logical_device, m_swap_chain, m_timeout, m_present_semaphore, (VkFence)nullptr, &m_buffer_index);
}

auto draw::swap_chain_t::queue_submit(VkQueue queue) -> void
{
	m_submit_info.pCommandBuffers = &m_graphics_command_buffers.at(m_buffer_index);

	::vkQueueSubmit(queue, 1, &m_submit_info, m_wait_fences.at(m_buffer_index));
}

auto draw::swap_chain_t::queue_present(VkQueue queue) -> void
{
	::vkQueuePresentKHR(queue, &m_present_info);

	::vkWaitForFences(m_logical_device, 1, &m_wait_fences.at(m_buffer_index), 1, m_timeout);
	::vkResetFences(m_logical_device, 1, &m_wait_fences.at(m_buffer_index));
}
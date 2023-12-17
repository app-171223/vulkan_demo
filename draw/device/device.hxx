#pragma once

#define VK_USE_PLATFORM_WIN32_KHR

#include <cstdint>
#include <vector>
#include <vulkan/vulkan.h>

namespace draw
{
	class device_t
	{
		VkInstance m_instance{ nullptr };
		VkPhysicalDevice m_physical_device{ nullptr };
		VkDevice m_logical_device{ nullptr };

		VkPhysicalDeviceFeatures m_physical_device_features{ };
		VkPhysicalDeviceProperties m_physical_device_properties{ };
		VkPhysicalDeviceMemoryProperties m_memory_properties{ };

		std::vector<VkQueueFamilyProperties> m_queue_family_properties{ };
		VkQueue m_graphics_queue{ nullptr };

		struct {
			std::uint32_t m_graphics{};
			std::uint32_t m_compute{};
			std::uint32_t m_transfer{};
		} m_queue_family_indices;

	public:

		device_t();

		~device_t();

	private:

		auto create_instance() -> void;

		auto pick_physical_device() -> void;

		auto find_device_specs() -> void;

		auto find_queue_specs() -> void;

		auto queue_family_index(std::uint32_t queue_flag) -> std::uint32_t;

		auto get_queue_create_infos() -> std::vector<VkDeviceQueueCreateInfo>;

		auto create_logical_device() -> void;

		auto retrieve_device_queues() -> void;

	public:

		auto get_instance() -> const VkInstance;

		auto get_device() -> const VkDevice;

		auto get_graphics_queue() -> const VkQueue;

		auto get_graphics_queue_index() -> std::uint32_t;

		auto get_memory_type_index(std::uint32_t type_bits, VkMemoryPropertyFlags property_flags) -> std::uint32_t;
	};
}

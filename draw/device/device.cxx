#include "device.hxx"

#include "../utils/settings.hxx"
#include "../utils/init.hxx"
#include "../utils/error.hxx"

draw::device_t::device_t()
{
	this->create_instance();
	this->pick_physical_device();
	this->find_device_specs();
	this->find_queue_specs();
	this->create_logical_device();
	this->retrieve_device_queues();
}

draw::device_t::~device_t()
{
	if (m_logical_device) ::vkDestroyDevice(m_logical_device, nullptr);
	if (m_instance) ::vkDestroyInstance(m_instance, nullptr);
}

auto draw::device_t::create_instance() -> void
{
	auto app_i = init::application_info(settings::api_version);
	auto instance_ci = init::instance_create_info(app_i, settings::instance_extensions);

	vk_check_result(::vkCreateInstance(&instance_ci, nullptr, &m_instance));
}

auto draw::device_t::pick_physical_device() -> void
{
	auto device_count = std::uint32_t{0};
	vk_check_result(::vkEnumeratePhysicalDevices(m_instance, &device_count, nullptr));

	auto physical_devices = std::vector<VkPhysicalDevice>{ device_count };
	vk_check_result(::vkEnumeratePhysicalDevices(m_instance, &device_count, physical_devices.data()));

	m_physical_device = physical_devices.at(0); // select the first device
}

auto draw::device_t::find_device_specs() -> void
{
	::vkGetPhysicalDeviceFeatures(m_physical_device, &m_physical_device_features);
	::vkGetPhysicalDeviceProperties(m_physical_device, &m_physical_device_properties);
	::vkGetPhysicalDeviceMemoryProperties(m_physical_device, &m_memory_properties);
}

auto draw::device_t::find_queue_specs() -> void
{
	auto queue_family_count = std::uint32_t{ 0 };
	::vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device, &queue_family_count, nullptr);
	m_queue_family_properties.resize(queue_family_count);
	::vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device, &queue_family_count, m_queue_family_properties.data());
}

auto draw::device_t::queue_family_index(std::uint32_t queue_flag) -> std::uint32_t
{
	for (const auto& queue_family : m_queue_family_properties)
		if ((queue_flag ^ queue_family.queueFlags) == 0) // has to match every flag
			return &queue_family - &m_queue_family_properties.at(0); // return index
}

auto draw::device_t::get_queue_create_infos() -> std::vector<VkDeviceQueueCreateInfo>
{
	// assign queue family ids
	m_queue_family_indices.m_graphics = this->queue_family_index(
		VK_QUEUE_GRAPHICS_BIT |
		VK_QUEUE_COMPUTE_BIT |
		VK_QUEUE_TRANSFER_BIT |
		VK_QUEUE_SPARSE_BINDING_BIT);

	m_queue_family_indices.m_compute = this->queue_family_index(
		VK_QUEUE_COMPUTE_BIT |
		VK_QUEUE_TRANSFER_BIT |
		VK_QUEUE_SPARSE_BINDING_BIT);

	m_queue_family_indices.m_transfer = this->queue_family_index(
		VK_QUEUE_TRANSFER_BIT |
		VK_QUEUE_SPARSE_BINDING_BIT);

	auto create_infos = std::vector<VkDeviceQueueCreateInfo>{ 3 };

	create_infos.at(0).sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	create_infos.at(0).queueFamilyIndex = m_queue_family_indices.m_graphics;
	create_infos.at(0).queueCount = std::uint32_t{ 1 };
	create_infos.at(0).pQueuePriorities = &settings::queue_priority;

	create_infos.at(1).sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	create_infos.at(1).queueFamilyIndex = m_queue_family_indices.m_compute;
	create_infos.at(1).queueCount = std::uint32_t{ 1 };
	create_infos.at(1).pQueuePriorities = &settings::queue_priority;

	create_infos.at(2).sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	create_infos.at(2).queueFamilyIndex = m_queue_family_indices.m_transfer;
	create_infos.at(2).queueCount = std::uint32_t{ 1 };
	create_infos.at(2).pQueuePriorities = &settings::queue_priority;

	return create_infos;
}

auto draw::device_t::create_logical_device() -> void
{
	auto create_infos = this->get_queue_create_infos();
	auto device_ci = init::device_create_info(create_infos, settings::device_extensions);
	vk_check_result(::vkCreateDevice(m_physical_device, &device_ci, nullptr, &m_logical_device))
}

auto draw::device_t::retrieve_device_queues() -> void
{
	::vkGetDeviceQueue(m_logical_device, m_queue_family_indices.m_graphics, 0, &m_graphics_queue);
}

auto draw::device_t::get_instance() -> const VkInstance
{
	return m_instance;
}

auto draw::device_t::get_device() -> const VkDevice
{
	return m_logical_device;
}

auto draw::device_t::get_graphics_queue() -> const VkQueue
{
	return m_graphics_queue;
}

auto draw::device_t::get_graphics_queue_index() -> std::uint32_t
{
	return m_queue_family_indices.m_graphics;
}

auto draw::device_t::get_memory_type_index(std::uint32_t type_bits, VkMemoryPropertyFlags property_flags) -> std::uint32_t
{
	for (auto i = std::uint32_t{ 0 }; i < m_memory_properties.memoryTypeCount; i++)
	{
		if ((type_bits & 1) == 1)
			if ((m_memory_properties.memoryTypes[i].propertyFlags & property_flags) == property_flags)
				return i;

		type_bits >>= 1;
	}
}
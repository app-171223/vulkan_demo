#include "pipeline.hxx"

#include <fstream>

#include "../utils/error.hxx"
#include "../utils/init.hxx"
#include "../utils/settings.hxx"

draw::pipeline_t::pipeline_t(const pipeline_setting_t& setting, device_t* device, swap_chain_t* swap_chain, VkRenderPass render_pass)
	: m_device{device},
	m_swap_chain{swap_chain}
{
	this->create_descriptor_set_layout(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	this->create_descriptor_pool(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
	this->create_descriptor_set(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

	this->create_pipeline_cache();
	this->create_graphics_pipeline(setting, render_pass);
}

draw::pipeline_t::pipeline_t(const pipeline_setting_t& setting, device_t* device, swap_chain_t* swap_chain, VkRenderPass render_pass, stb_fontchar* font_data)
	: m_device{device},
	m_swap_chain{swap_chain}
{
	this->create_font_image(font_data);

	this->create_descriptor_set_layout(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	this->create_descriptor_pool(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
	this->create_descriptor_set(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

	this->create_pipeline_cache();
	this->create_graphics_pipeline(setting, render_pass);
}

draw::pipeline_t::~pipeline_t()
{
	// pipeline
	if (m_graphics_pipeline) ::vkDestroyPipeline(m_device->get_device(), m_graphics_pipeline, nullptr);
	
	// shaders
	if (m_shaders.m_fragment) ::vkDestroyShaderModule(m_device->get_device(), m_shaders.m_fragment, nullptr);
	if (m_shaders.m_vertex) ::vkDestroyShaderModule(m_device->get_device(), m_shaders.m_vertex, nullptr);

	// pipeline cache
	if (m_pipeline_cache) ::vkDestroyPipelineCache(m_device->get_device(), m_pipeline_cache, nullptr);

	// image
	if (m_image.m_sampler) ::vkDestroySampler(m_device->get_device(), m_image.m_sampler, nullptr);
	if (m_image.m_view) ::vkDestroyImageView(m_device->get_device(), m_image.m_view, nullptr);
	if (m_image.m_memory) ::vkFreeMemory(m_device->get_device(), m_image.m_memory, nullptr);
	if (m_image.m_image) ::vkDestroyImage(m_device->get_device(), m_image.m_image, nullptr);

	// descriptor
	if (m_descriptor.m_set_layout) ::vkDestroyDescriptorSetLayout(m_device->get_device(), m_descriptor.m_set_layout, nullptr);
	if (m_descriptor.m_pool) ::vkDestroyDescriptorPool(m_device->get_device(), m_descriptor.m_pool, nullptr);
}

auto draw::pipeline_t::create_descriptor_set_layout(VkDescriptorType desc_type, VkShaderStageFlags shader_stage) -> void
{
	auto descriptor_set_layout_b = init::descriptor_set_layout_binding(desc_type, shader_stage);
	auto descriptor_set_layout_ci = init::descriptor_set_layout_create_info(descriptor_set_layout_b);
	vk_check_result(::vkCreateDescriptorSetLayout(m_device->get_device(), &descriptor_set_layout_ci, nullptr, &m_descriptor.m_set_layout));

	auto pipeline_layout_ci = init::pipeline_layout_create_info(m_descriptor.m_set_layout);
	vk_check_result(::vkCreatePipelineLayout(m_device->get_device(), &pipeline_layout_ci, nullptr, &m_pipeline_layout));
}

auto draw::pipeline_t::create_descriptor_pool(VkDescriptorType desc_type) -> void
{
	auto descriptor_pool_size = init::descriptor_pool_size(desc_type);
	auto descriptor_pool_ci = init::descriptor_pool_create_info(descriptor_pool_size);
	vk_check_result(::vkCreateDescriptorPool(m_device->get_device(), &descriptor_pool_ci, nullptr, &m_descriptor.m_pool));
}

auto draw::pipeline_t::create_descriptor_set(VkDescriptorType desc_type) -> void
{
	auto descriptor_set_ai = init::descriptor_set_allocate_info(m_descriptor.m_pool, m_descriptor.m_set_layout);
	vk_check_result(::vkAllocateDescriptorSets(m_device->get_device(), &descriptor_set_ai, &m_descriptor.m_set));

	if (desc_type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
	{
		auto descriptor_bi = VkDescriptorBufferInfo{ };
		auto write_descriptor_set = init::write_descriptor_set(m_descriptor.m_set, desc_type, descriptor_bi);
		::vkUpdateDescriptorSets(m_device->get_device(), 1, &write_descriptor_set, 0, nullptr);
	}
	else
	{
		auto descriptor_ii = init::descriptor_image_info(m_image.m_sampler, m_image.m_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		auto write_descriptor_set = init::write_descriptor_set(m_descriptor.m_set, desc_type, descriptor_ii);
		::vkUpdateDescriptorSets(m_device->get_device(), 1, &write_descriptor_set, 0, nullptr);
	}
}

auto draw::pipeline_t::create_font_image(stb_fontchar* font_data) -> void
{
	auto font_pixels = new std::uint8_t[settings::font::extent.width][settings::font::extent.height];
	
	//std::uint8_t font_pixels[settings::font::extent.width][settings::font::extent.height];
	::stb_font_consolas_24_latin1(font_data, font_pixels, settings::font::extent.height);

	auto image_ci = init::image_create_info(VK_FORMAT_R8_UNORM, settings::font::extent, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
	vk_check_result(::vkCreateImage(m_device->get_device(), &image_ci, nullptr, &m_image.m_image));

	auto memory_reqs = init::memory_requirements();
	auto memory_ai = init::memory_allocate_info();

	::vkGetImageMemoryRequirements(m_device->get_device(), m_image.m_image, &memory_reqs);
	memory_ai.allocationSize = memory_reqs.size;
	memory_ai.memoryTypeIndex = m_device->get_memory_type_index(memory_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	::vkAllocateMemory(m_device->get_device(), &memory_ai, nullptr, &m_image.m_memory);
	::vkBindImageMemory(m_device->get_device(), m_image.m_image, m_image.m_memory, 0);

	auto staging_memory = VkDeviceMemory{nullptr};
	auto staging_buffer = VkBuffer{nullptr};

	auto buffer_ci = init::buffer_create_info(memory_ai.allocationSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	vk_check_result(::vkCreateBuffer(m_device->get_device(), &buffer_ci, nullptr, &staging_buffer));

	::vkGetBufferMemoryRequirements(m_device->get_device(), staging_buffer, &memory_reqs);
	memory_ai.allocationSize = memory_reqs.size;
	memory_ai.memoryTypeIndex = m_device->get_memory_type_index(memory_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	vk_check_result(::vkAllocateMemory(m_device->get_device(), &memory_ai, nullptr, &staging_memory));
	vk_check_result(::vkBindBufferMemory(m_device->get_device(), staging_buffer, staging_memory, 0));

	void* data;
	::vkMapMemory(m_device->get_device(), staging_memory, 0, memory_ai.allocationSize, 0, &data);
	std::memcpy(data, &font_pixels[0][0], settings::font::extent.width * settings::font::extent.height);
	::vkUnmapMemory(m_device->get_device(), staging_memory);

	auto command_buffer_ai = init::command_buffer_allocate_info(m_swap_chain->get_command_pool(), 1);
	auto copy_command_buffer = VkCommandBuffer{nullptr};
	vk_check_result(::vkAllocateCommandBuffers(m_device->get_device(), &command_buffer_ai, &copy_command_buffer));

	auto command_buffer_bi = init::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	vk_check_result(::vkBeginCommandBuffer(copy_command_buffer, &command_buffer_bi));

	auto image_sr = init::image_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT);
	auto image_mb = init::image_memory_barier(VK_IMAGE_ASPECT_NONE_KHR, VK_ACCESS_TRANSFER_WRITE_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_image.m_image, image_sr);

	::vkCmdPipelineBarrier(
		copy_command_buffer,
		VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		std::uint32_t{0},
		std::uint32_t{0},
		nullptr,
		std::uint32_t{0},
		nullptr,
		std::uint32_t{1},
		&image_mb
	);

	auto buffer_cr = init::buffer_image_copy(settings::font::extent);

	::vkCmdCopyBufferToImage(
		copy_command_buffer,
		staging_buffer,
		m_image.m_image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&buffer_cr
	);

	auto image_copy_sr = init::image_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT);
	auto image_copy_mb = init::image_memory_barier(VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_image.m_image, image_sr);

	::vkCmdPipelineBarrier(
		copy_command_buffer,
		VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		std::uint32_t{0},
		std::uint32_t{0},
		nullptr,
		std::uint32_t{0},
		nullptr,
		std::uint32_t{1},
		&image_copy_mb
	);

	vk_check_result(::vkEndCommandBuffer(copy_command_buffer));

	auto submit_info = VkSubmitInfo{ };
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = std::uint32_t{1};
	submit_info.pCommandBuffers = &copy_command_buffer;
	vk_check_result(::vkQueueSubmit(m_device->get_graphics_queue(), 1, &submit_info, nullptr));
	vk_check_result(::vkQueueWaitIdle(m_device->get_graphics_queue()));

	::vkFreeCommandBuffers(m_device->get_device(), m_swap_chain->get_command_pool(), 1, &copy_command_buffer);
	::vkFreeMemory(m_device->get_device(), staging_memory, nullptr);
	::vkDestroyBuffer(m_device->get_device(), staging_buffer, nullptr);

	auto image_view_ci = init::image_view_create_info(m_image.m_image, VK_FORMAT_R8_UNORM);
	vk_check_result(::vkCreateImageView(m_device->get_device(), &image_view_ci, nullptr, &m_image.m_view));

	auto sampler_ci = init::sampler_create_info(VK_FILTER_LINEAR);
	vk_check_result(::vkCreateSampler(m_device->get_device(), &sampler_ci, nullptr, &m_image.m_sampler));

	delete[] font_pixels;
}

auto draw::pipeline_t::create_pipeline_cache() -> void
{
	auto pipeline_cache_ci = init::pipeline_cache_create_info();
	vk_check_result(::vkCreatePipelineCache(m_device->get_device(), &pipeline_cache_ci, nullptr, &m_pipeline_cache));
}

auto draw::pipeline_t::read_shader_file(const std::string& file_name) -> VkShaderModule
{
	auto shader_file = std::ifstream{file_name, std::ios::binary | std::ios::in | std::ios::ate};
	auto shader_size = static_cast<std::size_t>(shader_file.tellg());
	auto shader_code = std::unique_ptr<char[]>{new char[shader_size]};

	shader_file.seekg(0, std::ios::beg);
	shader_file.read(shader_code.get(), shader_size);
	shader_file.close();

	auto shader_module_ci = init::shader_module_create_info(shader_size, reinterpret_cast<std::uint32_t*>(shader_code.get()));
	auto shader_module = VkShaderModule{nullptr};
	vk_check_result(::vkCreateShaderModule(m_device->get_device(), &shader_module_ci, nullptr, &shader_module));

	return shader_module;
}

auto draw::pipeline_t::create_graphics_pipeline(const pipeline_setting_t& p_settings, VkRenderPass render_pass) -> void
{
	auto vertex_shader = this->read_shader_file(p_settings.m_vertex);
	auto fragment_shader = this->read_shader_file(p_settings.m_fragment);

	auto vertex_input_binding = init::vertex_input_binding_description(p_settings.m_stride);
	auto vertex_input_attributes = init::vertex_input_attribute_descriptions(p_settings.m_vert_input);
	auto color_blend_as = init::pipeline_color_blend_attachment_state();

	auto shader_stage_ci = init::pipeline_shader_stage_create_info(vertex_shader, fragment_shader, settings::shaders::entry_point);
	auto vertex_input_state_ci = init::pipeline_vertex_input_state_create_info(vertex_input_binding, vertex_input_attributes);
	auto input_assembly_state_ci = init::pipeline_input_assembly_state_create_info(p_settings.m_topology);
	auto rasterization_state_ci = init::pipeline_rasterization_state_create_info(p_settings.m_polygon_mode, std::float_t{1.0f});
	auto multisample_state_ci = init::pipeline_multisample_state_create_info();
	auto color_blend_state_ci = init::pipeline_color_blend_create_info(color_blend_as);
	auto dynamic_state_ci = init::pipeline_dynamic_state_create_info(settings::pipeline_dynamic_states);
	
	auto pipeline_ci = init::graphics_pipeline_create_info(
		shader_stage_ci,
		vertex_input_state_ci,
		input_assembly_state_ci,
		rasterization_state_ci,
		multisample_state_ci,
		color_blend_state_ci,
		dynamic_state_ci,
		m_pipeline_layout,
		render_pass
	);
	
	vk_check_result(::vkCreateGraphicsPipelines(m_device->get_device(), m_pipeline_cache, 1, &pipeline_ci, nullptr, &m_graphics_pipeline));
}



auto draw::pipeline_t::get_descriptor_set() -> const VkDescriptorSet*
{
	return &m_descriptor.m_set;
}

auto draw::pipeline_t::get_graphics_pipeline() -> const VkPipeline
{
	return m_graphics_pipeline;
}
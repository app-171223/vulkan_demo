#include "renderer.hxx"

#include "../utils/error.hxx"
#include "../utils/constants.hxx"
#include "../utils/settings.hxx"
#include "../utils/init.hxx"

draw::renderer_t::renderer_t(const HWND window_handle, stb_fontchar* font_data)
{
	// construct m_device
	m_device = new device_t{ };

	// construct m_swap_chain
	m_swap_chain = new swap_chain_t{
		m_device->get_instance(),
		m_device->get_device(),
		window_handle,
		m_device->get_graphics_queue_index()
	};

	this->create_render_pass();
	this->setup_depth_stencil();
	this->create_frame_buffers();

	// construct pipelines
	m_mesh_pipeline = new pipeline_t{settings::pipelines::mesh, m_device, m_swap_chain, m_render_pass}; // meshes
	m_line_pipeline = new pipeline_t{settings::pipelines::line, m_device, m_swap_chain, m_render_pass}; // lines
	m_text_pipeline = new pipeline_t{settings::pipelines::text, m_device, m_swap_chain, m_render_pass, font_data}; // text

	this->prepare_render_pass();
}

draw::renderer_t::~renderer_t()
{
	// destruct pipelines
	if (m_text_pipeline) delete m_text_pipeline;
	if (m_line_pipeline) delete m_line_pipeline;

	// frame buffers
	if (m_swap_chain) for (auto& frame_buffer : m_frame_buffers) ::vkDestroyFramebuffer(m_device->get_device(), frame_buffer, nullptr);

	// depth stencil
	if (m_depth_stencil.m_memory) ::vkFreeMemory(m_device->get_device(), m_depth_stencil.m_memory, nullptr);
	if (m_depth_stencil.m_view) ::vkDestroyImageView(m_device->get_device(), m_depth_stencil.m_view, nullptr);
	if (m_depth_stencil.m_image) ::vkDestroyImage(m_device->get_device(), m_depth_stencil.m_image, nullptr);

	// render pass
	if (m_render_pass) ::vkDestroyRenderPass(m_device->get_device(), m_render_pass, nullptr);

	if (m_swap_chain) delete m_swap_chain; // destruct m_swap_chain
	if (m_device) delete m_device; // destruct m_device
}



auto draw::renderer_t::create_render_pass() -> void
{
	auto color_attachment_ref = VkAttachmentReference{std::uint32_t{0}, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
	auto depth_attachment_ref = VkAttachmentReference{std::uint32_t{1}, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};

	auto attachments = init::attachment_descriptions(settings::color_format, settings::depth_format);
	auto subpass = init::subpass_description(color_attachment_ref, depth_attachment_ref);
	auto dependencies = init::subpass_dependencies();

	auto render_pass_ci = init::render_pass_create_info(attachments, subpass, dependencies);
	vk_check_result(::vkCreateRenderPass(m_device->get_device(), &render_pass_ci, nullptr, &m_render_pass));
}

auto draw::renderer_t::setup_depth_stencil() -> void
{
	auto image_ci = init::image_create_info(settings::depth_format, window::res_vk, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
	vk_check_result(::vkCreateImage(m_device->get_device(), &image_ci, nullptr, &m_depth_stencil.m_image));
	
	auto image_memory_reqs = VkMemoryRequirements{ };
	::vkGetImageMemoryRequirements(m_device->get_device(), m_depth_stencil.m_image, &image_memory_reqs);

	auto memory_ai = VkMemoryAllocateInfo{ };
	memory_ai.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memory_ai.allocationSize = image_memory_reqs.size;
	memory_ai.memoryTypeIndex = m_device->get_memory_type_index(image_memory_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	vk_check_result(::vkAllocateMemory(m_device->get_device(), &memory_ai, nullptr, &m_depth_stencil.m_memory));
	vk_check_result(::vkBindImageMemory(m_device->get_device(), m_depth_stencil.m_image, m_depth_stencil.m_memory, 0));

	auto image_view_ci = init::image_view_create_info(m_depth_stencil.m_image, settings::depth_format);
	vk_check_result(::vkCreateImageView(m_device->get_device(), &image_view_ci, nullptr, &m_depth_stencil.m_view));
}

auto draw::renderer_t::create_frame_buffers() -> void
{
	auto attachments = std::array<VkImageView, 2>{ nullptr, m_depth_stencil.m_view };
	auto frame_buffer_ci = init::frame_buffer_create_info(m_render_pass, attachments, window::res_vk);

	m_frame_buffers.resize(m_swap_chain->get_image_views().size());
	for (auto i = std::uint32_t{0}; i < m_frame_buffers.size(); i++)
	{
		attachments[0] = m_swap_chain->get_image_views().at(i).m_view;
		vk_check_result(::vkCreateFramebuffer(m_device->get_device(), &frame_buffer_ci, nullptr, &m_frame_buffers.at(i)));
	}
}

auto draw::renderer_t::prepare_render_pass() -> void
{
	m_clear_values[0].color = settings::pass_color;
	m_clear_values[1].depthStencil = settings::pass_depth;

	m_render_command_buffer_bi = init::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	m_render_pass_bi = init::render_pass_begin_info(m_render_pass, nullptr, window::res_vk, m_clear_values);
}

auto draw::renderer_t::create_vertex_buffer(memory_buffer_t& vertex_buffer) -> void
{
	// destroy buffer
	if (vertex_buffer.m_buffer) ::vkDestroyBuffer(m_device->get_device(), vertex_buffer.m_buffer, nullptr);
	if (vertex_buffer.m_memory) ::vkFreeMemory(m_device->get_device(), vertex_buffer.m_memory, nullptr);

	// allocate buffer
	auto vertex_buffer_ci = init::buffer_create_info(vertex_buffer.m_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
	auto memory_requirements = init::memory_requirements();
	auto memory_ai = init::memory_allocate_info();

	::vkCreateBuffer(m_device->get_device(), &vertex_buffer_ci, nullptr, &vertex_buffer.m_buffer);
	::vkGetBufferMemoryRequirements(m_device->get_device(), vertex_buffer.m_buffer, &memory_requirements);
	memory_ai.allocationSize = memory_requirements.size;
	memory_ai.memoryTypeIndex = m_device->get_memory_type_index(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	::vkAllocateMemory(m_device->get_device(), &memory_ai, nullptr, &vertex_buffer.m_memory);
	::vkBindBufferMemory(m_device->get_device(), vertex_buffer.m_buffer, vertex_buffer.m_memory, 0);
}

// MESH RENDERING
auto draw::renderer_t::allocate_vertices(mesh_buffer_t& mesh_buffer) -> void
{
	auto vertices = std::vector<vertex_t>{};

	for (const auto& mesh_info : mesh_buffer.m_info)
		vertices.insert(vertices.end(), mesh_info.m_vertices.begin(), mesh_info.m_vertices.end());

	if (auto new_size = sizeof(vertex_t) * vertices.size(); new_size != mesh_buffer.m_vertex_buffer.m_size)
	{
		mesh_buffer.m_vertex_buffer.m_size = new_size;
		this->create_vertex_buffer(mesh_buffer.m_vertex_buffer);
	}

	// copy data to buffer
	void* data{nullptr};
	::vkMapMemory(m_device->get_device(), mesh_buffer.m_vertex_buffer.m_memory, 0, mesh_buffer.m_vertex_buffer.m_size, 0, &data);
	std::memcpy(data, vertices.data(), mesh_buffer.m_vertex_buffer.m_size);
	::vkUnmapMemory(m_device->get_device(), mesh_buffer.m_vertex_buffer.m_memory);
}

auto draw::renderer_t::render_vertices(mesh_buffer_t& mesh_buffer) -> void
{
	::vkCmdBindDescriptorSets(m_swap_chain->get_render_buffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_mesh_pipeline->m_pipeline_layout, 0, 1, m_mesh_pipeline->get_descriptor_set(), 0, nullptr);
	::vkCmdBindVertexBuffers(m_swap_chain->get_render_buffer(), 0, 1, &mesh_buffer.m_vertex_buffer.m_buffer, &settings::vertex_buffer_offset);
	::vkCmdBindPipeline(m_swap_chain->get_render_buffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_mesh_pipeline->get_graphics_pipeline());
	
	for (auto i = std::uint32_t{0}, drawn_vertices = std::uint32_t{0}; i < mesh_buffer.m_info.size(); i++)
	{
		::vkCmdDraw(m_swap_chain->get_render_buffer(), mesh_buffer.m_info.at(i).m_vertices.size(), 1, drawn_vertices, 0);
		drawn_vertices += mesh_buffer.m_info.at(i).m_vertices.size();
	}
}

// LINE RENDERING
auto draw::renderer_t::allocate_vertices(line_buffer_t& line_buffer) -> void
{
	auto vertices = std::vector<vertex_t>{};

	for (const auto& line_info : line_buffer.m_info)
		vertices.insert(vertices.end(), line_info.m_vertices.begin(), line_info.m_vertices.end());

	if (auto new_size = sizeof(vertex_t) * vertices.size(); new_size != line_buffer.m_vertex_buffer.m_size)
	{
		line_buffer.m_vertex_buffer.m_size = new_size;
		this->create_vertex_buffer(line_buffer.m_vertex_buffer);
	}

	// copy data to buffer
	void* data{nullptr};
	::vkMapMemory(m_device->get_device(), line_buffer.m_vertex_buffer.m_memory, 0, line_buffer.m_vertex_buffer.m_size, 0, &data);
	std::memcpy(data, vertices.data(), line_buffer.m_vertex_buffer.m_size);
	::vkUnmapMemory(m_device->get_device(), line_buffer.m_vertex_buffer.m_memory);
}

auto draw::renderer_t::render_vertices(line_buffer_t& line_buffer) -> void
{
	::vkCmdBindDescriptorSets(m_swap_chain->get_render_buffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_line_pipeline->m_pipeline_layout, 0, 1, m_line_pipeline->get_descriptor_set(), 0, nullptr);
	::vkCmdBindVertexBuffers(m_swap_chain->get_render_buffer(), 0, 1, &line_buffer.m_vertex_buffer.m_buffer, &settings::vertex_buffer_offset);
	::vkCmdBindPipeline(m_swap_chain->get_render_buffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_line_pipeline->get_graphics_pipeline());

	for (auto i = std::uint32_t{0}, drawn_vertices = std::uint32_t{0}; i < line_buffer.m_info.size(); i++)
	{
		if (auto new_width = line_buffer.m_info.at(i).m_width; m_line_width != new_width)
			::vkCmdSetLineWidth(m_swap_chain->get_render_buffer(), m_line_width = new_width);

		::vkCmdDraw(m_swap_chain->get_render_buffer(), line_buffer.m_info.at(i).m_vertices.size(), 1, drawn_vertices, 0);
		drawn_vertices += line_buffer.m_info.at(i).m_vertices.size();
	}
}

// TEXT RENDERING
auto draw::renderer_t::allocate_vertices(text_buffer_t& text_buffer) -> void
{
	auto vertices = std::vector<text_vertex_t>{};

	for (const auto& text_info : text_buffer.m_info)
		vertices.insert(vertices.end(), text_info.m_vertices.begin(), text_info.m_vertices.end());

	if (auto new_size = sizeof(text_vertex_t) * vertices.size(); new_size != text_buffer.m_vertex_buffer.m_size)
	{
		text_buffer.m_vertex_buffer.m_size = new_size;
		this->create_vertex_buffer(text_buffer.m_vertex_buffer);
	}

	// copy data to buffer
	void* data{nullptr};
	::vkMapMemory(m_device->get_device(), text_buffer.m_vertex_buffer.m_memory, 0, text_buffer.m_vertex_buffer.m_size, 0, &data);
	std::memcpy(data, vertices.data(), text_buffer.m_vertex_buffer.m_size);
	::vkUnmapMemory(m_device->get_device(), text_buffer.m_vertex_buffer.m_memory);
}

auto draw::renderer_t::render_vertices(text_buffer_t& text_buffer) -> void
{
	::vkCmdBindDescriptorSets(m_swap_chain->get_render_buffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_text_pipeline->m_pipeline_layout, 0, 1, m_text_pipeline->get_descriptor_set(), 0, nullptr);
	::vkCmdBindVertexBuffers(m_swap_chain->get_render_buffer(), 0, 1, &text_buffer.m_vertex_buffer.m_buffer, &settings::vertex_buffer_offset);
	::vkCmdBindPipeline(m_swap_chain->get_render_buffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_text_pipeline->get_graphics_pipeline());

	for (auto drawn = std::uint32_t{0}; auto info : text_buffer.m_info)
	{
		for (auto letter = std::uint32_t{0}; letter != info.m_vertices.size() / 4; letter++)
			::vkCmdDraw(m_swap_chain->get_render_buffer(), 4, 1, drawn + letter * 4, 0);

		drawn += info.m_vertices.size();
	}
}


auto draw::renderer_t::begin_frame() -> void
{
	m_swap_chain->acquire_next_image();
	
	::vkBeginCommandBuffer(m_swap_chain->get_render_buffer(), &m_render_command_buffer_bi);
	::vkCmdSetViewport(m_swap_chain->get_render_buffer(), 0, 1, &settings::viewport);

	m_render_pass_bi.framebuffer = m_frame_buffers.at(m_swap_chain->get_buffer_index());
	::vkCmdBeginRenderPass(m_swap_chain->get_render_buffer(), &m_render_pass_bi, VK_SUBPASS_CONTENTS_INLINE);
}

auto draw::renderer_t::end_frame() -> void
{
	::vkCmdEndRenderPass(m_swap_chain->get_render_buffer());
	::vkEndCommandBuffer(m_swap_chain->get_render_buffer());

	m_swap_chain->queue_submit(m_device->get_graphics_queue());
	m_swap_chain->queue_present(m_device->get_graphics_queue());
}
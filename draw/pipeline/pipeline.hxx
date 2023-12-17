#pragma once

#include <vector>
#include <string>
#include <vulkan/vulkan.h>

#include "../device/device.hxx"
#include "../swap_chain/swap_chain.hxx"
#include "../utils/containers.hxx"
#include "../utils/settings.hxx"
#include "../fonts/stb_font_consolas_24_latin1.inl"

namespace draw
{
	class pipeline_t
	{
		device_t* m_device{nullptr};
		swap_chain_t* m_swap_chain{nullptr};

		// descriptor
		struct {
			VkDescriptorSetLayout m_set_layout{ nullptr };
			VkDescriptorPool m_pool{ nullptr };
			VkDescriptorSet m_set{ nullptr };
		} m_descriptor{ };

		// pipeline cache
		VkPipelineCache m_pipeline_cache{nullptr};
		
		// image
		struct {
			VkImage m_image{ nullptr };
			VkDeviceMemory m_memory{ nullptr };
			VkImageView m_view{ nullptr };
			VkSampler m_sampler{ nullptr };
		} m_image{ };

		// shaders
		struct {
			VkShaderModule m_vertex{ nullptr };
			VkShaderModule m_fragment{ nullptr };
		} m_shaders;

		// pipeline
		VkPipeline m_graphics_pipeline{nullptr};

	public:

		VkPipelineLayout m_pipeline_layout{nullptr};

		pipeline_t(const pipeline_setting_t& setting, device_t* device, swap_chain_t* swap_chain, VkRenderPass render_pass);

		pipeline_t(const pipeline_setting_t& setting, device_t* device, swap_chain_t* swap_chain, VkRenderPass render_pass, stb_fontchar* font_data);
		
		~pipeline_t();

	private:

		auto create_descriptor_set_layout(VkDescriptorType desc_type, VkShaderStageFlags shader_stage) -> void;

		auto create_descriptor_pool(VkDescriptorType desc_type) -> void;
		
		auto create_descriptor_set(VkDescriptorType desc_type) -> void;

		auto create_font_image(stb_fontchar* font_data) -> void;

		auto create_pipeline_cache() -> void;

		auto read_shader_file(const std::string& file_name) -> VkShaderModule;

		auto create_graphics_pipeline(const pipeline_setting_t& p_settings, VkRenderPass render_pass) -> void;

	public:

		auto get_descriptor_set() -> const VkDescriptorSet*;

		auto get_graphics_pipeline() -> const VkPipeline;
	};
}
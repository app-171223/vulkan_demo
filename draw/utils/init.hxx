#pragma once

#define VK_USE_PLATFORM_WIN32_KHR

#include <vulkan/vulkan.h>
#include <windows.h>
#include <array>
#include <vulkan/vulkan_win32.h>

#include "containers.hxx"

namespace draw
{
	namespace init
	{
		inline auto application_info(
			std::uint32_t api_version
		) -> const VkApplicationInfo
		{
			return VkApplicationInfo{
				VK_STRUCTURE_TYPE_APPLICATION_INFO,
				nullptr,
				nullptr,
				std::uint32_t{ 0 },
				nullptr,
				std::uint32_t{ 0 },
				api_version
			};
		}

		inline auto instance_create_info(
			const VkApplicationInfo& application_info,
			const std::vector<const char*>& instance_extensions
		) -> const VkInstanceCreateInfo
		{
			return VkInstanceCreateInfo{
				VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
				nullptr,
				std::uint32_t{ 0 },
				&application_info,
				std::uint32_t{ 0 },
				nullptr,
				static_cast<std::uint32_t>( instance_extensions.size( ) ),
				instance_extensions.data( )
			};
		}

		inline auto device_create_info(
			const std::vector<VkDeviceQueueCreateInfo>& queue_create_info,
			const std::vector<const char*>& device_extensions
		) -> const VkDeviceCreateInfo
		{
			return VkDeviceCreateInfo{
				VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
				nullptr,
				std::uint32_t{ 0 },
				static_cast<std::uint32_t>( queue_create_info.size( ) ),
				queue_create_info.data( ),
				std::uint32_t{ 0 },
				nullptr,
				static_cast<std::uint32_t>( device_extensions.size( ) ),
				device_extensions.data( ),
				nullptr
			};
		}

		inline auto surface_create_info(
			const HWND window_handle
		) -> const VkWin32SurfaceCreateInfoKHR
		{
			return VkWin32SurfaceCreateInfoKHR{
				VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
				nullptr,
				std::uint32_t{ 0 },
				nullptr,
				window_handle
			};
		}

		inline auto command_pool_create_info(
			std::uint32_t queue_family_index,
			VkCommandPoolCreateFlags flags
		) -> const VkCommandPoolCreateInfo
		{
			return VkCommandPoolCreateInfo{
				VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
				nullptr,
				flags,
				queue_family_index
			};
		}

		inline auto swap_chain_create_info(
			const VkSurfaceKHR surface,
			bool v_sync,
			VkFormat color_format,
			VkColorSpaceKHR color_space,
			VkExtent2D extent
		) -> const VkSwapchainCreateInfoKHR
		{
			return VkSwapchainCreateInfoKHR{
				VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
				nullptr,
				std::uint32_t{ 0 },
				surface,
				std::uint32_t{ v_sync ? 2U : 3U },
				color_format,
				color_space,
				extent,
				std::uint32_t{ 1 },
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
				VK_SHARING_MODE_EXCLUSIVE,
				std::uint32_t{ 0 },
				nullptr,
				VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
				VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
				VkPresentModeKHR{ v_sync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_MAILBOX_KHR },
				std::int32_t{ 1 },
				nullptr
			};
		}

		inline auto image_view_create_info(
			const VkImage image,
			VkFormat color_format
		) -> const VkImageViewCreateInfo
		{
			auto components = VkComponentMapping{
				VK_COMPONENT_SWIZZLE_R,
				VK_COMPONENT_SWIZZLE_G,
				VK_COMPONENT_SWIZZLE_B,
				VK_COMPONENT_SWIZZLE_A
			};

			auto subresource_range = VkImageSubresourceRange{
				VK_IMAGE_ASPECT_COLOR_BIT,
				std::uint32_t{ 0 },
				std::uint32_t{ 1 },
				std::uint32_t{ 0 },
				std::uint32_t{ 1 }
			};

			return VkImageViewCreateInfo{
				VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				nullptr,
				std::uint32_t{ 0 },
				image,
				VK_IMAGE_VIEW_TYPE_2D,
				color_format,
				components,
				subresource_range
			};
		}

		inline auto command_buffer_allocate_info(
			const VkCommandPool command_pool,
			std::size_t buffer_count
		) -> const VkCommandBufferAllocateInfo
		{
			return VkCommandBufferAllocateInfo{
				VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
				nullptr,
				command_pool,
				VK_COMMAND_BUFFER_LEVEL_PRIMARY,
				static_cast<std::uint32_t>( buffer_count )
			};
		}

		inline auto semaphore_create_info( ) -> const VkSemaphoreCreateInfo
		{
			return VkSemaphoreCreateInfo{
				VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
				nullptr,
				std::uint32_t{ 0 },
			};
		}

		inline auto fence_create_info(
			VkFenceCreateFlags flags
		) -> const VkFenceCreateInfo
		{
			return VkFenceCreateInfo{
				VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
				nullptr,
				flags
			};
		}

		inline auto submit_info(
			const VkSemaphore& wait_semaphore,
			const VkPipelineStageFlags& stage_mask,
			const VkSemaphore& signal_semaphore
		) -> const VkSubmitInfo
		{
			return VkSubmitInfo{
				VK_STRUCTURE_TYPE_SUBMIT_INFO,
				nullptr,
				std::uint32_t{ 1 },
				&wait_semaphore,
				&stage_mask,
				std::uint32_t{ 1 },
				nullptr, // set every vkQueueSubmit call
				std::uint32_t{ 1 },
				&signal_semaphore
			};
		}

		inline auto present_info(
			const VkSemaphore& wait_semaphore,
			const VkSwapchainKHR& swap_chain,
			const std::uint32_t& image_index
		) -> const VkPresentInfoKHR
		{
			return VkPresentInfoKHR{
				VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
				nullptr,
				std::uint32_t{ 1 },
				&wait_semaphore,
				std::uint32_t{ 1 },
				&swap_chain,
				&image_index,
				nullptr
			};
		}

		inline auto shader_module_create_info(
			std::size_t shader_size,
			std::uint32_t* shader_code
		) -> const VkShaderModuleCreateInfo
		{
			return VkShaderModuleCreateInfo{
				VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
				nullptr,
				std::uint32_t{ 0 },
				shader_size,
				shader_code
			};
		}

		inline auto attachment_descriptions(
			VkFormat color_format,
			VkFormat depth_format
		) -> const std::array<VkAttachmentDescription, 2>
		{
			return std::array<VkAttachmentDescription, 2>{
				VkAttachmentDescription{ // color attachment
					std::uint32_t{ 0 },
					color_format,
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_CLEAR,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
				},
				VkAttachmentDescription{ // depth attachment
					std::uint32_t{ 0 },
					depth_format,
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_CLEAR,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_CLEAR,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
				}
			};
		}

		inline auto subpass_description(
			const VkAttachmentReference& color_attachment_ref,
			const VkAttachmentReference& depth_attachment_ref
		) -> const VkSubpassDescription
		{
			return VkSubpassDescription{
				std::uint32_t{ 0 },
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				std::uint32_t{ 0 },
				nullptr,
				std::uint32_t{ 1 },
				&color_attachment_ref,
				nullptr,
				&depth_attachment_ref,
				std::uint32_t{ 0 },
				nullptr
			};
		}

		inline auto subpass_dependencies( ) -> const std::array<VkSubpassDependency, 2>
		{
			return std::array<VkSubpassDependency, 2>{
				VkSubpassDependency{
					std::uint32_t{ ~0U },
					std::uint32_t{ 0 },
					VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					VK_ACCESS_MEMORY_READ_BIT,
					VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					VK_DEPENDENCY_BY_REGION_BIT
				},
				VkSubpassDependency{
					std::uint32_t{ 0 },
					std::uint32_t{ ~0u },
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
					VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					VK_ACCESS_MEMORY_READ_BIT,
					VK_DEPENDENCY_BY_REGION_BIT
				}
			};
		}

		inline auto render_pass_create_info(
			const std::array<VkAttachmentDescription, 2>& attachments,
			const VkSubpassDescription& subpass,
			const std::array<VkSubpassDependency, 2>& dependencies
		) -> const VkRenderPassCreateInfo
		{
			return VkRenderPassCreateInfo{
				VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
				nullptr,
				std::uint32_t{ 0 },
				static_cast<std::uint32_t>( attachments.size( ) ),
				attachments.data( ),
				std::uint32_t{ 1 },
				&subpass,
				static_cast<std::uint32_t>( dependencies.size( ) ),
				dependencies.data( )
			};
		}

		inline auto image_create_info(
			VkFormat format,
			VkExtent2D extent,
			VkImageUsageFlags usage
		) -> const VkImageCreateInfo
		{
			return VkImageCreateInfo{
				VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
				nullptr,
				std::uint32_t{ 0 },
				VK_IMAGE_TYPE_2D,
				format,
				VkExtent3D{ extent.width, extent.height, 1 },
				std::uint32_t{ 1 },
				std::uint32_t{ 1 },
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				usage,
				VK_SHARING_MODE_EXCLUSIVE,
				std::uint32_t{ 0 },
				nullptr,
				VK_IMAGE_LAYOUT_UNDEFINED
			};
		}

		inline auto pipeline_cache_create_info( ) -> const VkPipelineCacheCreateInfo
		{
			return VkPipelineCacheCreateInfo{
				VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
				nullptr,
				std::uint32_t{ 0 },
				std::size_t{ 0 },
				nullptr
			};
		}

		inline auto frame_buffer_create_info(
			const VkRenderPass render_pass,
			const std::array<VkImageView, 2>& attachments,
			VkExtent2D extent
		) -> const VkFramebufferCreateInfo
		{
			return VkFramebufferCreateInfo{
				VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
				nullptr,
				std::uint32_t{ 0 },
				render_pass,
				static_cast<std::uint32_t>( attachments.size( ) ),
				attachments.data( ),
				extent.width,
				extent.height,
				std::uint32_t{ 1 }
			};
		}

		inline auto descriptor_set_layout_binding(
			VkDescriptorType type,
			VkShaderStageFlags stage_flags
		) -> const VkDescriptorSetLayoutBinding
		{
			return VkDescriptorSetLayoutBinding{
				std::uint32_t{ 0 },
				type,
				std::uint32_t{ 1 },
				stage_flags,
				nullptr
			};
		}

		inline auto descriptor_set_layout_create_info(
			const VkDescriptorSetLayoutBinding& layout_b
		) -> const VkDescriptorSetLayoutCreateInfo
		{
			return VkDescriptorSetLayoutCreateInfo{
				VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
				nullptr,
				std::uint32_t{ 0 },
				std::uint32_t{ 1 },
				&layout_b
			};
		}

		inline auto pipeline_layout_create_info(
			const VkDescriptorSetLayout& layout
		) -> const VkPipelineLayoutCreateInfo
		{
			return VkPipelineLayoutCreateInfo{
				VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
				nullptr,
				std::uint32_t{ 0 },
				std::uint32_t{ 1 },
				&layout,
				std::uint32_t{ 0 },
				nullptr
			};
		}

		inline auto descriptor_pool_size(
			VkDescriptorType type
		) -> const VkDescriptorPoolSize
		{
			return VkDescriptorPoolSize{
				type,
				std::uint32_t{ 1 }
			};
		}

		inline auto descriptor_pool_create_info(
			const VkDescriptorPoolSize& pool_size
		) -> const VkDescriptorPoolCreateInfo
		{
			return VkDescriptorPoolCreateInfo{
				VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
				nullptr,
				std::uint32_t{ 0 },
				std::uint32_t{ 1 },
				std::uint32_t{ 1 },
				&pool_size
			};
		}

		inline auto descriptor_set_allocate_info(
			const VkDescriptorPool& pool,
			const VkDescriptorSetLayout& layout
		) -> const VkDescriptorSetAllocateInfo
		{
			return VkDescriptorSetAllocateInfo{
				VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
				nullptr,
				pool,
				std::uint32_t{ 1 },
				&layout
			};
		}

		inline auto write_descriptor_set(
			const VkDescriptorSet& set,
			VkDescriptorType type,
			const VkDescriptorBufferInfo& buffer_info
		) -> const VkWriteDescriptorSet
		{
			return VkWriteDescriptorSet{
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				nullptr,
				set,
				std::uint32_t{ 0 },
				std::uint32_t{ 0 },
				std::uint32_t{ 1 },
				type,
				nullptr,
				&buffer_info,
				nullptr
			};
		}

		inline auto write_descriptor_set(
			const VkDescriptorSet& set,
			VkDescriptorType type,
			const VkDescriptorImageInfo& image_info
		) -> const VkWriteDescriptorSet
		{
			return VkWriteDescriptorSet{
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				nullptr,
				set,
				std::uint32_t{ 0 },
				std::uint32_t{ 0 },
				std::uint32_t{ 1 },
				type,
				&image_info,
				nullptr,
				nullptr
			};
		}

		inline auto pipeline_input_assembly_state_create_info(
			VkPrimitiveTopology topology
		) -> const VkPipelineInputAssemblyStateCreateInfo
		{
			return VkPipelineInputAssemblyStateCreateInfo{
				VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
				nullptr,
				std::uint32_t{ 0 },
				topology,
				std::uint32_t{ 0 }
			};
		}

		inline auto pipeline_rasterization_state_create_info(
			VkPolygonMode polygon_mode,
			std::float_t line_width
		) -> const VkPipelineRasterizationStateCreateInfo
		{
			return VkPipelineRasterizationStateCreateInfo{
				VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
				nullptr,
				std::uint32_t{ 0 },
				std::uint32_t{ 0 },
				std::uint32_t{ 0 },
				polygon_mode,
				VK_CULL_MODE_NONE,
				VK_FRONT_FACE_COUNTER_CLOCKWISE, //VK_FRONT_FACE_CLOCKWISE,
				std::uint32_t{ 0 },
				std::float_t{ 0.0f },
				std::float_t{ 0.0f },
				std::float_t{ 0.0f },
				line_width
			};
		}

		inline auto pipeline_color_blend_attachment_state( ) -> const VkPipelineColorBlendAttachmentState
		{
			return VkPipelineColorBlendAttachmentState{
				std::uint32_t{ 1 },
				VK_BLEND_FACTOR_SRC_ALPHA,
				VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
				VK_BLEND_OP_ADD,
				VK_BLEND_FACTOR_SRC_ALPHA,
				VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
				VK_BLEND_OP_ADD,
				VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
			};
		}

		inline auto pipeline_color_blend_create_info(
			const VkPipelineColorBlendAttachmentState& attachment_state
		) -> const VkPipelineColorBlendStateCreateInfo
		{
			return VkPipelineColorBlendStateCreateInfo{
				VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
				nullptr,
				std::uint32_t{ 0 },
				std::uint32_t{ 0 },
				VK_LOGIC_OP_COPY,
				std::uint32_t{ 1 },
				&attachment_state,
				{ std::float_t{ 1.0f }, std::float_t{ 1.0f }, std::float_t{ 1.0f }, std::float_t{ 1.0f } }
			};
		}

		inline auto pipeline_dynamic_state_create_info(
			const std::vector<VkDynamicState>& dynamic_states
		) -> const VkPipelineDynamicStateCreateInfo
		{
			return VkPipelineDynamicStateCreateInfo{
				VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
				nullptr,
				std::uint32_t{ 0 },
				static_cast<std::uint32_t>( dynamic_states.size( ) ),
				dynamic_states.data( )
			};
		}

		inline auto pipeline_multisample_state_create_info( ) -> const VkPipelineMultisampleStateCreateInfo
		{
			return VkPipelineMultisampleStateCreateInfo{
				VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
				nullptr,
				std::uint32_t{ 0 },
				VK_SAMPLE_COUNT_1_BIT,
				std::uint32_t{ 0 },
				std::float_t{ 0.0f },
				nullptr,
				std::uint32_t{ 0 },
				std::uint32_t{ 0 }
			};
		}

		inline auto vertex_input_binding_description(
			std::size_t stride_size
		) -> const VkVertexInputBindingDescription
		{
			return VkVertexInputBindingDescription{
				std::uint32_t{ 0 },
				static_cast<std::uint32_t>( stride_size ),
				VK_VERTEX_INPUT_RATE_VERTEX
			};
		}

		inline auto vertex_input_attribute_descriptions(
			const std::vector<vertex_input_t>& input_infos
		) -> const std::vector<VkVertexInputAttributeDescription>
		{
			auto ret = std::vector<VkVertexInputAttributeDescription>{ };

			for ( const auto& input : input_infos )
			{
				ret.push_back( VkVertexInputAttributeDescription{
					static_cast<std::uint32_t>( &input - &input_infos.at( 0 ) ), // curr index
					std::uint32_t{ 0 },
					input.m_format,
					static_cast<std::uint32_t>( input.m_offset )
				} );
			}

			return ret;
		}

		inline auto pipeline_vertex_input_state_create_info(
			const VkVertexInputBindingDescription& binding,
			const std::vector<VkVertexInputAttributeDescription>& attributes
		) -> const VkPipelineVertexInputStateCreateInfo
		{
			return VkPipelineVertexInputStateCreateInfo{
				VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
				nullptr,
				std::uint32_t{ 0 },
				std::uint32_t{ 1 },
				&binding,
				static_cast<std::uint32_t>( attributes.size( ) ),
				attributes.data( )
			};
		}

		inline auto pipeline_shader_stage_create_info(
			const VkShaderModule vertex_shader,
			const VkShaderModule fragment_shader,
			const std::string& entry_point
		) -> const std::array<VkPipelineShaderStageCreateInfo, 2>
		{
			return std::array<VkPipelineShaderStageCreateInfo, 2>{
				VkPipelineShaderStageCreateInfo{
					VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
					nullptr,
					std::uint32_t{ 0 },
					VK_SHADER_STAGE_VERTEX_BIT,
					vertex_shader,
					entry_point.c_str( ),
					nullptr
				},
				VkPipelineShaderStageCreateInfo{
					VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
					nullptr,
					std::uint32_t{ 0 },
					VK_SHADER_STAGE_FRAGMENT_BIT,
					fragment_shader,
					entry_point.c_str( ),
					nullptr
				}
			};
		}

		inline auto graphics_pipeline_create_info(
			const std::array<VkPipelineShaderStageCreateInfo, 2>& shader_stages,
			const VkPipelineVertexInputStateCreateInfo& vertex_input_state,
			const VkPipelineInputAssemblyStateCreateInfo& input_assembly_state,
			const VkPipelineRasterizationStateCreateInfo& rasterization_state,
			const VkPipelineMultisampleStateCreateInfo& multisample_state,
			const VkPipelineColorBlendStateCreateInfo& color_blend_state,
			const VkPipelineDynamicStateCreateInfo& dynamic_state,
			const VkPipelineLayout pipeline_layout,
			const VkRenderPass render_pass
		) -> const VkGraphicsPipelineCreateInfo
		{
			return VkGraphicsPipelineCreateInfo{
				VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
				nullptr,
				std::uint32_t{ 0 },
				static_cast<std::uint32_t>( shader_stages.size( ) ),
				shader_stages.data( ),
				&vertex_input_state,
				&input_assembly_state,
				nullptr,
				nullptr,
				&rasterization_state,
				&multisample_state,
				nullptr,
				&color_blend_state,
				&dynamic_state,
				pipeline_layout,
				render_pass,
				std::uint32_t{ 0 },
				nullptr,
				std::uint32_t{ 0 },
			};
		}

		inline auto memory_allocate_info( ) -> const VkMemoryAllocateInfo
		{
			return VkMemoryAllocateInfo{
				VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
				nullptr,
				VkDeviceSize{ 0 },
				std::uint32_t{ 0 }
			};
		}

		inline auto memory_requirements( ) -> const VkMemoryRequirements
		{
			return VkMemoryRequirements{
				VkDeviceSize{ 0 },
				VkDeviceSize{ 0 },
				std::uint32_t{ 0 }
			};
		}

		inline auto buffer_create_info(
			VkDeviceSize size,
			VkBufferUsageFlags usage
		) -> const VkBufferCreateInfo
		{
			return VkBufferCreateInfo{
				VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
				nullptr,
				std::uint32_t{ 0 },
				size,
				usage,
				VK_SHARING_MODE_EXCLUSIVE,
				std::uint32_t{ 0 },
				nullptr
			};
		}

		inline auto command_buffer_begin_info(
			VkCommandBufferUsageFlags flags
		) -> const VkCommandBufferBeginInfo
		{
			return VkCommandBufferBeginInfo{
				VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
				nullptr,
				flags,
				nullptr
			};
		}

		inline auto render_pass_begin_info(
			const VkRenderPass render_pass,
			const VkFramebuffer frame_buffer,
			const VkExtent2D extent,
			const std::array<VkClearValue, 2>& clear_values
		) -> const VkRenderPassBeginInfo
		{
			return VkRenderPassBeginInfo{
				VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
				nullptr,
				render_pass,
				frame_buffer,
				VkRect2D{ { 0, 0 }, extent },
				static_cast<std::uint32_t>( clear_values.size( ) ),
				clear_values.data( )
			};
		}

		inline auto buffer_copy(
			VkDeviceSize buffer_size
		) -> VkBufferCopy
		{
			return VkBufferCopy{
				VkDeviceSize{ 0 },
				VkDeviceSize{ 0 },
				buffer_size
			};
		}

		inline auto image_subresource_range(
			VkImageAspectFlags aspect_mask
		) -> VkImageSubresourceRange
		{
			return VkImageSubresourceRange{
				aspect_mask,
				std::uint32_t{ 0 },
				std::uint32_t{ 1 },
				std::uint32_t{ 0 },
				std::uint32_t{ 1 },
			};
		}

		inline auto image_memory_barier(
			VkAccessFlags src_access_flag,
			VkAccessFlags dst_access_flag,
			VkImageLayout old_img_layout,
			VkImageLayout new_img_layout,
			VkImage image,
			const VkImageSubresourceRange& range
		) -> VkImageMemoryBarrier
		{
			return VkImageMemoryBarrier{
				VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
				nullptr,
				src_access_flag,
				dst_access_flag,
				old_img_layout,
				new_img_layout,
				VK_QUEUE_FAMILY_IGNORED,
				VK_QUEUE_FAMILY_IGNORED,
				image,
				range
			};
		}

		inline auto buffer_image_copy(
			VkExtent2D extent
		) -> VkBufferImageCopy
		{
			auto image_layers = VkImageSubresourceLayers{
				VK_IMAGE_ASPECT_COLOR_BIT,
				std::uint32_t{ 0 },
				std::uint32_t{ 0 },
				std::uint32_t{ 1 }
			};

			return VkBufferImageCopy{
				VkDeviceSize{ 0 },
				std::uint32_t{ 0 },
				std::uint32_t{ 0 },
				image_layers,
				VkOffset3D{ 0, 0, 0 },
				VkExtent3D{ extent.width, extent.height, 1 }
			};
		}

		inline auto sampler_create_info(
			VkFilter filter
		) -> VkSamplerCreateInfo
		{
			return VkSamplerCreateInfo{
				VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
				nullptr,
				std::uint32_t{ 0 },
				filter,
				filter,
				VK_SAMPLER_MIPMAP_MODE_LINEAR,
				VK_SAMPLER_ADDRESS_MODE_REPEAT,
				VK_SAMPLER_ADDRESS_MODE_REPEAT,
				VK_SAMPLER_ADDRESS_MODE_REPEAT,
				std::float_t{ 0.0f },
				std::uint32_t{ 0 },
				std::float_t{ 1.0f },
				std::uint32_t{ 0 },
				VK_COMPARE_OP_NEVER,
				std::float_t{ 0.0f },
				std::float_t{ 1.0f },
				VK_BORDER_COLOR_INT_OPAQUE_BLACK,
				std::uint32_t{ 0 }
			};
		}

		inline auto descriptor_image_info(
			VkSampler sampler,
			VkImageView view,
			VkImageLayout image_layout
		) -> VkDescriptorImageInfo
		{
			return VkDescriptorImageInfo{
				sampler,
				view,
				image_layout
			};
		}
	}
}
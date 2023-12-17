#pragma once

#define VK_USE_PLATFORM_WIN32_KHR

#include <string>
#include <iostream>
#include <vulkan/vulkan.h>

namespace vulkan_utils
{
	constexpr auto error_string(VkResult error_code) -> const std::string
	{
		switch (error_code)
		{
			#define error_message(r) case VK_ ##r: return #r
			error_message(NOT_READY);
			error_message(TIMEOUT);
			error_message(EVENT_SET);
			error_message(EVENT_RESET);
			error_message(INCOMPLETE);
			error_message(ERROR_OUT_OF_HOST_MEMORY);
			error_message(ERROR_OUT_OF_DEVICE_MEMORY);
			error_message(ERROR_INITIALIZATION_FAILED);
			error_message(ERROR_DEVICE_LOST);
			error_message(ERROR_MEMORY_MAP_FAILED);
			error_message(ERROR_LAYER_NOT_PRESENT);
			error_message(ERROR_EXTENSION_NOT_PRESENT);
			error_message(ERROR_FEATURE_NOT_PRESENT);
			error_message(ERROR_INCOMPATIBLE_DRIVER);
			error_message(ERROR_TOO_MANY_OBJECTS);
			error_message(ERROR_FORMAT_NOT_SUPPORTED);
			error_message(ERROR_SURFACE_LOST_KHR);
			error_message(ERROR_NATIVE_WINDOW_IN_USE_KHR);
			error_message(SUBOPTIMAL_KHR);
			error_message(ERROR_OUT_OF_DATE_KHR);
			error_message(ERROR_INCOMPATIBLE_DISPLAY_KHR);
			error_message(ERROR_VALIDATION_FAILED_EXT);
			error_message(ERROR_INVALID_SHADER_NV);
			#undef error_message;
		default:
			return "UNKNOWN_ERROR";
		}
	}
}

#define vk_check_result(f) { if (VkResult res = f; res != VK_SUCCESS) { \
	std::cout << "Fatal : VkResult is \"" << vulkan_utils::error_string(res) << "\" in " << __FILE__ << " at line " << __LINE__ << std::endl;	} }
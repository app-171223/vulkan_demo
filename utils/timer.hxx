#pragma once

#include <chrono>
#include <cstdint>
#include <cmath>

class timer_t
{
	std::uint32_t m_fps{0};
	std::uint32_t m_frame_count{0};
	std::chrono::steady_clock::time_point m_fps_update{};

	std::double_t m_delta{0.0};
	std::chrono::steady_clock::time_point m_delta_update{};
	
public:

	timer_t()
		: m_delta_update{std::chrono::steady_clock::now()} { }

	auto update() -> void
	{
		auto current_time = std::chrono::steady_clock::now();
		auto diff = std::chrono::duration_cast<std::chrono::microseconds>(current_time - m_delta_update).count();

		m_delta = (std::double_t)(diff / 1000000.0); // one million microseconds in a second
		m_delta_update = current_time;

		// update fps after fps interval elapsed
		if (++m_frame_count; current_time - m_fps_update >= std::chrono::seconds(1))
		{
			m_fps = m_frame_count;
			m_frame_count = 0;
			m_fps_update = current_time;
		}
	}

	auto get_delta() -> std::double_t { return m_delta; };

	auto get_fps() -> std::uint32_t { return m_fps; };
};
#pragma once

#include "../window/window.hxx"
#include "../draw/scene/scene.hxx"
#include "../utils/containers.hxx"
#include "../utils/timer.hxx"
#include "../input/input.hxx"

class demo_t
{
	enum class menu_state : std::uint8_t
	{
		main,
		example_1,
		example_2,
		example_3
	};

	menu_state m_ui_state{menu_state::main};
	std::uint16_t m_bar{200};

	// example 1 data
	struct {
		std::vector<vertex_t> m_vertices{};
		bool m_points{true};
		bool m_line{false};
		bool m_mesh{false};
		bool m_rand{false};
	} m_draw;

	// example 2 data
	struct {
		// general settings
		bool m_outer{true};
		bool m_inner{false};
		bool m_inner_lines{false};
		bool m_animate{false};

		// circle settings
		std::uint16_t m_sides{16};
		std::float_t m_radius{300.0f};
		std::float_t m_width{2.0f};

		// anim settings
		std::uint16_t m_anim_vert{1};
		std::float_t m_anim_prog{0.0f};
		std::float_t m_anim_lenght{8.0f}; // in seconds time to complete animation
	} m_circle;
	
	// example 3 data
	struct {
		// general settings
		bool m_lines{false};
		
		// circle settings
		std::uint16_t m_circles{1};
		std::float_t m_radius{300.0f};
		std::float_t m_anim_lenght{4.0f}; // in seconds time to complete animation

		// changing
		std::float_t m_alpha{0.0f};
	} m_motion;

	auto draw_menu(draw::scene_t& scene, timer_t& timer) -> void;

	auto main(draw::scene_t& scene, timer_t& timer) -> void;

	auto example_1(draw::scene_t& scene, timer_t& timer) -> void;

	auto example_2(draw::scene_t& scene, timer_t& timer) -> void;

	auto example_3(draw::scene_t& scene, timer_t& timer) -> void;

public:

	auto render(draw::scene_t& scene, timer_t& timer) -> void;
};
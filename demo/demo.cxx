#include "demo.hxx"

auto demo_t::draw_menu(draw::scene_t& scene, timer_t& timer) -> void
{
	// menu background
	scene.mesh(
		rect_t(
			window::res_vec.m_x - m_bar - 5,
			0,
			m_bar + 5,
			window::res_vec.m_y
		),
		color_t{35, 35, 35, 255}
	);

	// main menu button
	if (scene.button(rect_t(window::res_vec.m_x - m_bar, window::res_vec.m_y - 45, 195, 35), "Main menu", 16))
		m_ui_state = menu_state::main;

	// render debug info
	scene.debug_info(timer.get_fps(), 16, color_t{0, 192, 255, 255}, m_ui_state == menu_state::example_1, 0);
}

auto demo_t::main(draw::scene_t& scene, timer_t& timer) -> void
{
	scene.text(vertex_t(window::center.m_x, 200, color_t{255, 255, 255, 255}), "Main menu", 24.0f, 1);

	if (scene.button(rect_t(window::center.m_x, 250, 400, 50), "Mesh drawing", 18, 0, 1))
		m_ui_state = menu_state::example_1;

	else if (scene.button(rect_t(window::center.m_x, 305, 400, 50), "Rendering circles", 18, 0, 1))
		m_ui_state = menu_state::example_2;

	else if (scene.button(rect_t(window::center.m_x, 360, 400, 50), "Circular motion illusion", 18, 0, 1))
		m_ui_state = menu_state::example_3;
}

auto demo_t::example_1(draw::scene_t& scene, timer_t& timer) -> void
{
	auto menu_y = std::uint16_t{10};

	// user interface
	{
		if (scene.button(rect_t(window::res_vec.m_x - m_bar, menu_y, 195, 35), m_draw.m_points ? "Point: on" : "Points: off", 16))
			m_draw.m_points = !m_draw.m_points;

		if (scene.button(rect_t(window::res_vec.m_x - m_bar, menu_y += 40, 195, 35), m_draw.m_line ? "Line: on" : "Lines: off", 16))
			m_draw.m_line = !m_draw.m_line;

		if (scene.button(rect_t(window::res_vec.m_x - m_bar, menu_y += 40, 195, 35), m_draw.m_mesh ? "Polygons: on" : "Polygons: off", 16))
			m_draw.m_mesh = !m_draw.m_mesh;

		if (scene.button(rect_t(window::res_vec.m_x - m_bar, menu_y += 40, 195, 35), m_draw.m_rand ? "Random color: on" : "Random color: off", 16))
			m_draw.m_rand = !m_draw.m_rand;

		if (scene.button(rect_t(window::res_vec.m_x - m_bar, menu_y += 70, 195, 35), "Clear", 16))
			m_draw.m_vertices.clear();
	}


	
	// add a point
	if (auto cursor = scene.get_cursor(); input::key_down(key::lmb, key_flag::pushed) && cursor.m_x < window::res_vec.m_x - 210)
	{
		auto color = m_draw.m_rand ? color_t(::rand() % 256, ::rand() % 256, ::rand() % 256, 255) : color_t{255, 0, 80, 255};
		m_draw.m_vertices.push_back(vertex_t{cursor.m_x, cursor.m_y, color});
	}

	// draw line
	auto color_override = color_t{255, 255, 255, 255};
	if (m_draw.m_line) scene.line(m_draw.m_vertices, 2.0f, &color_override);

	// draw meshes
	if (m_draw.m_mesh) scene.mesh(m_draw.m_vertices);

	// draw points
	if (m_draw.m_points)
		for (auto vertex : m_draw.m_vertices)
			scene.circle(vertex, 12, 6, 2.0f, &color_override);
}

auto demo_t::example_2(draw::scene_t& scene, timer_t& timer) -> void
{
	auto menu_y = std::uint16_t{10};

	// user interface
	{ 
		// controls
		if (scene.button(rect_t(window::res_vec.m_x - m_bar, menu_y, 195, 35), m_circle.m_outer ? "Outer: on" : "Outer: off", 16))
			m_circle.m_outer = !m_circle.m_outer;

		if (scene.button(rect_t(window::res_vec.m_x - m_bar, menu_y += 40, 195, 35), m_circle.m_inner ? "Inner: on" : "Inner: off", 16))
			m_circle.m_inner = !m_circle.m_inner;

		// inner settings
		if (scene.button(rect_t(window::res_vec.m_x - m_bar, menu_y += 40, 195, 35), m_circle.m_inner_lines ? "Inner-lines: on" : "Inner-lines: off", 16))
			m_circle.m_inner_lines = !m_circle.m_inner_lines;

		if (scene.button(rect_t(window::res_vec.m_x - m_bar, menu_y += 40, 195, 35), m_circle.m_animate ? "Inner-animate: on" : "Inner-animate: off", 16))
			m_circle.m_animate = !m_circle.m_animate;

		// sides controls
		scene.text(vertex_t(window::res_vec.m_x - m_bar, menu_y += 55,
			color_t{255, 255, 255, 255}), "Sides: " + std::to_string(m_circle.m_sides), 16);

		if (scene.button(rect_t(window::res_vec.m_x - m_bar, menu_y += 25, 95, 35), "+", 16))
			if (m_circle.m_sides < 48) m_circle.m_sides++;

		if (scene.button(rect_t(window::res_vec.m_x - m_bar + 100, menu_y, 95, 35), "-", 16))
			if (m_circle.m_sides > 3) m_circle.m_sides--;


		// radius controls
		scene.text(vertex_t(window::res_vec.m_x - m_bar, menu_y += 55,
			color_t{255, 255, 255, 255}), "Radius: " + std::to_string((std::int32_t)std::roundf(m_circle.m_radius)) + "px", 16);

		if (scene.button(rect_t(window::res_vec.m_x - m_bar, menu_y += 25, 95, 35), "+", 16, 1, 0))
			if (m_circle.m_radius < 350.0f)
				m_circle.m_radius += 100.0f * timer.get_delta();

		if (scene.button(rect_t(window::res_vec.m_x - m_bar + 100, menu_y, 95, 35), "-", 16, 1, 0))
			if (m_circle.m_radius > 10.0f)
				m_circle.m_radius -= 100.0f * timer.get_delta();

		// line width controls
		scene.text(vertex_t(window::res_vec.m_x - m_bar, menu_y += 55,
			color_t{255, 255, 255, 255}), "Outer width: " + std::to_string((std::int32_t)std::roundf(m_circle.m_width)) + "px", 16);

		if (scene.button(rect_t(window::res_vec.m_x - m_bar, menu_y += 25, 95, 35), "+", 16, 1, 0))
			if (m_circle.m_width < 40.0f)
				m_circle.m_width += 20.0f * timer.get_delta();

		if (scene.button(rect_t(window::res_vec.m_x - m_bar + 100, menu_y, 95, 35), "-", 16, 1, 0))
			if (m_circle.m_width > 1.0f)
				m_circle.m_width -= 20.0f * timer.get_delta();

		// anim duration controls
		scene.text(vertex_t(window::res_vec.m_x - m_bar, menu_y += 55,
			color_t{255, 255, 255, 255}), "Duration: " + std::to_string((std::int32_t)std::roundf(m_circle.m_anim_lenght)) + "s", 16);

		if (scene.button(rect_t(window::res_vec.m_x - m_bar, menu_y += 25, 95, 35), "+", 16, 1, 0))
			if (m_circle.m_anim_lenght < 20.0f)
				m_circle.m_anim_lenght += 10.0f * timer.get_delta();

		if (scene.button(rect_t(window::res_vec.m_x - m_bar + 100, menu_y, 95, 35), "-", 16, 1, 0))
			if (m_circle.m_anim_lenght > 2.0f)
				m_circle.m_anim_lenght -= 10.0f * timer.get_delta();		
	}
	
	
	
	auto circle_center = point_t{window::center.m_x - m_bar / 2, window::center.m_y};

	if (m_circle.m_outer)
	{
		scene.circle(vertex_t(circle_center.m_x, circle_center.m_y, color_t{255, 0, 80, 255}), m_circle.m_sides, m_circle.m_radius, m_circle.m_width);
	}
	if (m_circle.m_inner)
	{
		// get circle positions
		auto circle_vert = std::vector<vertex_t>{};
		auto side_rot = (constants::pi * 2) / m_circle.m_sides;
		
		for (auto i = std::uint8_t(0), x = std::uint8_t(0), side = std::uint8_t(0); i != m_circle.m_sides; i++)
		{
			auto rot = (side = ~side) ? side_rot * x : side_rot * (m_circle.m_sides - ++x);
			
			circle_vert.push_back(
				vertex_t(
					m_circle.m_radius * std::cos(rot) + circle_center.m_x,
					m_circle.m_radius * std::sin(rot) + circle_center.m_y,
					color_t{255, 255, 255, 255}
				)
			);
		}
		
		// render animated circle
		if (m_circle.m_animate)
		{
			// reverse vector only if animating
			std::reverse(circle_vert.begin(), circle_vert.end());

			auto anim_vert = std::vector<vertex_t>{};
			for (auto i = std::uint16_t{0}; i != circle_vert.size() && i != m_circle.m_anim_vert + 1; i++)
				anim_vert.push_back(circle_vert.at(i));
			
			m_circle.m_anim_prog += m_circle.m_sides / m_circle.m_anim_lenght * timer.get_delta() * constants::pi;
			if (m_circle.m_anim_prog >= (constants::pi * 3) / 2) m_circle.m_anim_prog = (constants::pi * 3) / 2;

			auto line_progress = std::float_t(std::sin(m_circle.m_anim_prog));

			if (m_circle.m_anim_vert == m_circle.m_sides) m_circle.m_anim_vert = 1;

			auto& last_1 = anim_vert.end()[-1];
			auto& last_2 = anim_vert.end()[-2];

			last_1.m_pos.m_x = last_1.m_pos.m_x + (0.5 - line_progress / 2 * -1) * (last_2.m_pos.m_x - last_1.m_pos.m_x);
			last_1.m_pos.m_y = last_1.m_pos.m_y + (0.5 - line_progress / 2 * -1) * (last_2.m_pos.m_y - last_1.m_pos.m_y);

			if (m_circle.m_anim_prog >= (constants::pi * 3) / 2)
			{
				m_circle.m_anim_prog = constants::pi / 2;
				m_circle.m_anim_vert++;
			}

			last_1.m_col = color_t{255, 0, 80, 255};

			scene.mesh(anim_vert);

			if (m_circle.m_inner_lines)
			{
				auto override = color_t{255, 0, 80, 255};
				scene.line(anim_vert, 2.0f, &override);
			}
		}
		else // render filled circle, not animated
		{
			scene.circle(vertex_t(circle_center.m_x, circle_center.m_y, color_t{255, 255, 255, 255}), m_circle.m_sides, m_circle.m_radius);

			if (m_circle.m_inner_lines)
			{
				auto override = color_t{255, 0, 80, 255};
				scene.line(circle_vert, 2.0f, &override);
			}
		}
	}
}

auto demo_t::example_3(draw::scene_t& scene, timer_t& timer) -> void
{
	auto menu_y = std::uint16_t{10};

	// user interface
	{
		// render lines
		if (scene.button(rect_t(window::res_vec.m_x - m_bar, menu_y, 195, 35), m_motion.m_lines ? "Lines: on" : "Lines: off", 16))
			m_motion.m_lines = !m_motion.m_lines;

		// sides controls
		scene.text(vertex_t(window::res_vec.m_x - m_bar, menu_y += 55,
			color_t{255, 255, 255, 255}), "Circles: " + std::to_string(m_motion.m_circles), 16);

		if (scene.button(rect_t(window::res_vec.m_x - m_bar, menu_y += 25, 95, 35), "+", 16))
			if (m_motion.m_circles < 40) m_motion.m_circles++;

		if (scene.button(rect_t(window::res_vec.m_x - m_bar + 100, menu_y, 95, 35), "-", 16))
			if (m_motion.m_circles > 1) m_motion.m_circles--;


		// radius controls
		scene.text(vertex_t(window::res_vec.m_x - m_bar, menu_y += 55,
			color_t{255, 255, 255, 255}), "Radius: " + std::to_string((std::int32_t)std::roundf(m_motion.m_radius)) + "px", 16);

		if (scene.button(rect_t(window::res_vec.m_x - m_bar, menu_y += 25, 95, 35), "+", 16, 1, 0))
			if (m_motion.m_radius < 350.0f)
				m_motion.m_radius += 100.0f * timer.get_delta();

		if (scene.button(rect_t(window::res_vec.m_x - m_bar + 100, menu_y, 95, 35), "-", 16, 1, 0))
			if (m_motion.m_radius > 50.0f)
				m_motion.m_radius -= 100.0f * timer.get_delta();

		// anim duration controls
		scene.text(vertex_t(window::res_vec.m_x - m_bar, menu_y += 55,
			color_t{255, 255, 255, 255}), "Duration: " + std::to_string((std::int32_t)std::roundf(m_motion.m_anim_lenght)) + "s", 16);

		if (scene.button(rect_t(window::res_vec.m_x - m_bar, menu_y += 25, 95, 35), "+", 16, 1, 0))
			if (m_motion.m_anim_lenght < 20.0f)
				m_motion.m_anim_lenght += 10.0f * timer.get_delta();

		if (scene.button(rect_t(window::res_vec.m_x - m_bar + 100, menu_y, 95, 35), "-", 16, 1, 0))
			if (m_motion.m_anim_lenght > 2.0f)
				m_motion.m_anim_lenght -= 10.0f * timer.get_delta();
	}



	auto center = point_t(window::center.m_x - m_bar / 2, window::center.m_y);
	auto offset = constants::pi / m_motion.m_circles;	

	if (m_motion.m_alpha += 1 / m_motion.m_anim_lenght * timer.get_delta() * constants::pi * 2; m_motion.m_alpha > constants::pi * 2)
		m_motion.m_alpha -= constants::pi * 2;
	
	for (auto i = 0; i != m_motion.m_circles; i++)
	{
		auto line_pos = std::float_t(std::sin(m_motion.m_alpha + (-i * offset)) * m_motion.m_radius);
	
		auto circle_vert = vertex_t{
			line_pos * std::cos(i * offset) + center.m_x,
			line_pos * std::sin(i * offset) + center.m_y,
			color_t{255, 0, 80, 255}
		};
	
		scene.circle(circle_vert, 16, 7);

		if (m_motion.m_lines)
		{
			scene.line(
				vertex_t{
					m_motion.m_radius * std::cos(i * offset) + center.m_x,
					m_motion.m_radius * std::sin(i * offset) + center.m_y,
					color_t{255, 255, 255, 60}
				},
				vertex_t{
					-m_motion.m_radius * std::cos(i * offset) + center.m_x,
					-m_motion.m_radius * std::sin(i * offset) + center.m_y,
					color_t{255, 255, 255, 60}
				}
			);
		}
	}
}



auto demo_t::render(draw::scene_t& scene, timer_t& timer) -> void
{
	if (m_ui_state != menu_state::main)
		this->draw_menu(scene, timer);

	switch (m_ui_state)
	{
		case menu_state::main:
			this->main(scene, timer);
			break;
		case menu_state::example_1:
			this->example_1(scene, timer);
			break;
		case menu_state::example_2:
			this->example_2(scene, timer);
			break;
		case menu_state::example_3:
			this->example_3(scene, timer);
			break;
	}
}
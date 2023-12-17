#include "scene.hxx"

#include "../utils/settings.hxx"
#include "../utils/constants.hxx"
#include "../../input/input.hxx"

draw::scene_t::scene_t(const HWND window_handle)
	: m_wnd{window_handle}
{
	m_renderer = new renderer_t{window_handle, m_text.m_font_data};
}

draw::scene_t::~scene_t()
{
	if (m_renderer) delete m_renderer;
}



auto draw::scene_t::mesh(std::vector<vertex_t> points) -> void
{
	if (points.size() < 3)
		return;

	auto info = mesh_info_t{}; // arbitrary amount of points in a mesh
	
	for (auto point : points)
		info.m_vertices.push_back(point.to_absolute(window::res_vec));

	m_meshes.m_info.push_back(info);
}

auto draw::scene_t::mesh(rect_t rect, color_t color) -> void
{
	this->mesh(
		std::vector<vertex_t>{
			vertex_t(rect.m_x, rect.m_y, color),
			vertex_t(rect.m_x, rect.m_y + rect.m_height, color),
			vertex_t(rect.m_x + rect.m_width, rect.m_y, color),
			vertex_t(rect.m_x + rect.m_width, rect.m_y + rect.m_height, color),
		}
	);
}

auto draw::scene_t::circle(vertex_t center, std::uint8_t sides, std::uint16_t radius) -> void
{
	auto vertices = std::vector<vertex_t>{};
	auto side_rot = (constants::pi * 2) / sides;
	
	for (auto i = std::uint8_t(sides), x = std::uint8_t{0}, side = std::uint8_t{0}; i; i--)
	{
		auto rot = (side = ~side) ? side_rot * x : side_rot * (sides - ++x);

		vertices.push_back(
			vertex_t(
				radius * std::cos(rot) + center.m_pos.m_x,
				radius * std::sin(rot) + center.m_pos.m_y,
				center.m_col
			)
		);
	}
	
	this->mesh(vertices);
}

auto draw::scene_t::circle(vertex_t center, std::uint8_t sides, std::uint16_t radius, std::float_t line_width, const color_t* override) -> void
{
	auto vertices = std::vector<vertex_t>{};
	auto side_rot = (constants::pi * 2) / sides;

	for (auto i = std::uint8_t{0}; i < sides + 1; i++)
	{
		auto rot = side_rot * i;

		vertices.push_back(
			vertex_t(
				radius * std::cos(rot) + center.m_pos.m_x,
				radius * std::sin(rot) + center.m_pos.m_y,
				override ? *override : center.m_col
			)
		);
	}

	this->line(vertices, line_width);
}

auto draw::scene_t::line(vertex_t from, vertex_t to, std::float_t width) -> void
{
	m_lines.m_info.push_back(
		line_info_t{
			std::vector<vertex_t>{
				from.to_absolute(window::res_vec),
				to.to_absolute(window::res_vec)
			},
			width
		}
	);
}

auto draw::scene_t::line(const std::vector<vertex_t>& points, std::float_t width, const color_t* override) -> void
{
	if (points.size() < 2)
		return;
	
	auto old_pos = vertex_t{points.front().m_pos.m_x, points.front().m_pos.m_y, override ? *override : points.front().m_col};
	for (auto i = std::uint32_t{1}; i != points.size(); i++)
	{
		auto point = vertex_t{points.at(i).m_pos.m_x, points.at(i).m_pos.m_y, override ? *override : points.at(i).m_col};
		
		this->line(old_pos, point, width);
		old_pos = point;
	}
}

auto draw::scene_t::text(vertex_t abs, const std::string& text, std::float_t size, bool center) -> void
{
	abs.to_absolute(window::res_vec);
	
	auto char_w = (size / 10.0f) * 1.0f / window::res_vk.width;
	auto char_h = (size / 10.0f) * 1.0f / window::res_vk.height;
	
	// same width for every consolas character
	auto advance = (&m_text.m_font_data[static_cast<std::uint32_t>(0x61) - settings::font::first_char])->advance;
	
	if (center) // center text horizontally and vertically
	{
		abs.m_pos.m_x -= (char_w * advance * text.size()) / 2;
		abs.m_pos.m_y -= (advance * char_h) / 1.5;
	}

	auto info = text_info_t{}; // arbitrary amount of characters in a label

	for (auto letter : text)
	{
		auto char_data = &m_text.m_font_data[static_cast<std::uint32_t>(letter) - settings::font::first_char];
		
		info.m_vertices.push_back(text_vertex_t{
			vec2_t{abs.m_pos.m_x + (float)char_data->x0 * char_w, abs.m_pos.m_y + (float)char_data->y0 * char_h},
			vec2_t{char_data->s0, char_data->t0}, abs.m_col
			});
		info.m_vertices.push_back(text_vertex_t{
			vec2_t{abs.m_pos.m_x + (float)char_data->x1 * char_w, abs.m_pos.m_y + (float)char_data->y0 * char_h},
			vec2_t{char_data->s1, char_data->t0}, abs.m_col
			});
		info.m_vertices.push_back(text_vertex_t{
			vec2_t{abs.m_pos.m_x + (float)char_data->x0 * char_w, abs.m_pos.m_y + (float)char_data->y1 * char_h},
			vec2_t{char_data->s0, char_data->t1}, abs.m_col
			});
		info.m_vertices.push_back(text_vertex_t{
			vec2_t{abs.m_pos.m_x + (float)char_data->x1 * char_w, abs.m_pos.m_y + (float)char_data->y1 * char_h},
			vec2_t{char_data->s1, char_data->t1}, abs.m_col
		});

		abs.m_pos.m_x += char_data->advance * char_w;
	}

	m_text.m_info.push_back(info);
}

auto draw::scene_t::button(rect_t rect, const std::string& label, std::float_t text_size, bool held, bool center) -> bool
{
	if (center) rect.m_x -= rect.m_width / 2; // center button horizontally

	auto hovered =
		m_cursor_pos.m_x >= rect.m_x &&
		m_cursor_pos.m_x <= rect.m_x + rect.m_width &&
		m_cursor_pos.m_y >= rect.m_y &&
		m_cursor_pos.m_y <= rect.m_y + rect.m_height;

	++m_button.m_current;

	auto pushed = hovered && input::key_down(key::lmb, key_flag::down);
	if (pushed)	m_button.m_pushed = m_button.m_current;

	auto released = !pushed && m_button.m_pushed == m_button.m_current;
	if (released) m_button.m_pushed = ~0u;

	released &= hovered;

	auto mesh_color = pushed ? color_t{255, 220, 220, 255} : hovered ? color_t{255, 255, 255, 255} : color_t{200, 200, 200, 255};
	auto outline_color = pushed ? color_t{255, 0, 100, 255} : hovered ? color_t{170, 170, 170, 255} : color_t{200, 200, 200, 255};

	// button mesh
	this->mesh(rect, mesh_color);	
	
	// button outline
	this->line(
		std::vector<vertex_t>{
			vertex_t(rect.m_x, rect.m_y, outline_color),
			vertex_t(rect.m_x, rect.m_y + rect.m_height, outline_color),
			vertex_t(rect.m_x + rect.m_width, rect.m_y + rect.m_height, outline_color),
			vertex_t(rect.m_x + rect.m_width, rect.m_y, outline_color),
			vertex_t(rect.m_x, rect.m_y, outline_color)
		},
		2.0f
	);

	// button text
	this->text(
		vertex_t{rect.m_x + rect.m_width / 2, rect.m_y + rect.m_height / 2, color_t{0, 0, 0, 255}},
		label,
		text_size,
		1
	);

	return held ? pushed : released;
}

auto draw::scene_t::debug_info(std::uint32_t fps, std::float_t text_size, color_t text_color, bool cursor_pos, bool crosshair) -> void
{
	// draw fps
	this->text(vertex_t{10, 20, color_t{255, 0, 100, 255}},
		"fps: " + std::to_string(fps), text_size);

	if (cursor_pos)
	{
		// draw cursor pos relative to the window
		this->text(vertex_t{10, 40, color_t{255, 0, 100, 255}},
			"x: " + std::to_string(m_cursor_pos.m_x) + ", y: " + std::to_string(m_cursor_pos.m_y), text_size);
	}

	// draw crosshar
	if (crosshair) 
	{
		this->line(
			vertex_t{0, m_cursor_pos.m_y, color_t{255, 255, 255, 255}},
			vertex_t{window::res_vec.m_x, m_cursor_pos.m_y, color_t{255, 255, 255, 255}}
		);

		this->line(
			vertex_t{m_cursor_pos.m_x, 0, color_t{255, 255, 255, 255}},
			vertex_t{m_cursor_pos.m_x, window::res_vec.m_y, color_t{255, 255, 255, 255}}
		);
	}
}

auto draw::scene_t::begin() -> void
{
	m_renderer->begin_frame();

	// update cursor position in scene
	auto cursor_pos = POINT{};
	::GetCursorPos(&cursor_pos);
	::ScreenToClient(m_wnd, &cursor_pos);

	m_cursor_pos = point_t(cursor_pos.x, cursor_pos.y);
}

auto draw::scene_t::end() -> void
{
	m_button.m_current = 0; // current button = first in the list

	if (!m_meshes.m_info.empty())
	{
		m_renderer->allocate_vertices(m_meshes);
		m_renderer->render_vertices(m_meshes);
		m_meshes.m_info.clear();
	}

	if (!m_lines.m_info.empty())
	{
		m_renderer->allocate_vertices(m_lines);
		m_renderer->render_vertices(m_lines);
		m_lines.m_info.clear();
	}

	if (!m_text.m_info.empty())
	{
		m_renderer->allocate_vertices(m_text);
		m_renderer->render_vertices(m_text);
		m_text.m_info.clear();
	}

	m_renderer->end_frame();
}

auto draw::scene_t::get_cursor() -> point_t { return m_cursor_pos; }
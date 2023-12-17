#pragma once

#include <cstdint>
#include <cmath>
#include <algorithm>

template<typename T>
struct vec2_t
{
	vec2_t() = default;

	vec2_t(T x, T y) : m_x{x}, m_y{y} { }

	auto operator+(const vec2_t<T>& vec) const -> vec2_t<T> { return {m_x + vec.m_x, m_y + vec.m_y}; }

	auto operator+=(const vec2_t<T>& vec) const -> vec2_t<T>& { m_x += vec.m_x;	m_y += vec.m_y; return *this; }

	auto operator*(const float val) const -> vec2_t<T> { return {m_x * val, m_y * val}; }

	auto operator-(const vec2_t<T>& vec) const -> vec2_t<T> { return {m_x - vec.m_x, m_y - vec.m_y}; }

	auto operator-=(const vec2_t<T>& vec) const -> vec2_t<T>& { m_x -= vec.m_x; m_y -= vec.m_y; return *this; }

	auto operator-=(const T val) const -> vec2_t<T>& { m_x -= val; m_y -= val; return *this; }

	auto operator==(const vec2_t<T>& vec) const -> bool { return (m_x == vec.m_x && m_y == vec.m_y); }

	auto operator/(const float val) const -> vec2_t<T> { return {m_x / val, m_y / val}; }

	auto operator!() const -> bool { return {!m_x && !m_y}; }

	auto magnitude(const vec2_t<T>& vec) const -> float { return std::sqrt((vec.m_x - m_x) * (vec.m_x - m_x) + (vec.m_y - m_y) * (vec.m_y - m_y)); }

	auto to_absolute(vec2_t<std::int16_t> res) const -> vec2_t<std::float_t>
	{
		return vec2_t<std::float_t>{
			std::float_t{-1.0f + (static_cast<std::float_t>(this->m_x) / static_cast<std::float_t>(res.m_x)) * 2.0f},
			std::float_t{-1.0f + (static_cast<std::float_t>(this->m_y) / static_cast<std::float_t>(res.m_y)) * 2.0f}
		};
	}

	T m_x, m_y;
};

typedef vec2_t<std::int32_t> point_t;

template<typename T>
struct vec3_t
{
	vec3_t() = default;

	vec3_t(T x, T y, T z) : m_x{x}, m_y{y}, m_z{z} { }

	auto operator+(const vec3_t<T>& vec) const -> vec3_t<T> { return {m_x + vec.m_x, m_y + vec.m_y, m_z + vec.m_z}; }

	auto operator+(const float val) const -> vec3_t<T> { return {m_x + val, m_y + val, m_z + val}; }

	auto operator+=(const vec3_t<T>& vec) const -> vec3_t<T>& { m_x += vec.m_x; m_y += vec.m_y; m_z += vec.m_z; return *this; }

	auto operator-(const vec3_t<T>& vec) const -> vec3_t<T> { return {m_x - vec.m_x, m_y - vec.m_y, m_z - vec.m_z}; }

	auto operator-=(const vec3_t<T>& vec) const -> vec3_t<T>& { m_x -= vec.m_x; m_y -= vec.m_y; m_z -= vec.m_z; return *this; }

	auto operator==(const vec3_t<T>& vec) const -> bool { return (m_x == vec.m_x && m_y == vec.m_y && m_z == vec.m_z); }

	auto operator*(const vec3_t<T>& vec) const -> vec3_t<T> { return {m_x * vec.m_x, m_y * vec.m_y, m_z * vec.m_z}; }

	auto operator*(const float val) const -> vec3_t<T> { return {m_x * val, m_y * val, m_z * val}; }

	auto operator/(const float val) const -> vec3_t<T> { return {m_x / val, m_y / val, m_z / val}; }

	auto operator!() const -> bool { return {!m_x && !m_y && !m_z}; }

	auto magnitude() const -> float { return std::sqrt(m_x * m_x + m_y * m_y + m_z * m_z); }

	auto magnitude(const vec3_t<T>& vec) const -> float { return std::sqrt((vec.m_x - this->m_x) * (vec.m_x - this->m_x) + (vec.m_y - this->m_y) * (vec.m_y - this->m_y) + (vec.m_z - this->m_z) * (vec.m_z - this->m_z)); }

	auto dot_product(const vec3_t<float>& vec) const -> float { return this->m_x * vec.m_x + this->m_y * vec.m_y + this->m_z * vec.m_z; }

	T m_x, m_y, m_z;
};

struct rect_t
{
	rect_t() = default;

	rect_t(std::uint16_t x, std::uint16_t y, std::uint16_t widht, std::uint16_t height)
		: m_x{x},
		m_y{y},
		m_width{widht},
		m_height{height} { }

	std::uint16_t m_x, m_y, m_width, m_height;
};

struct color_t
{
	color_t() = default;

	color_t(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a)
		: m_r{r}, m_g{g}, m_b{b}, m_a{a} { }
	
	auto operator*(float num) const -> color_t { return color_t(m_r * num, m_g * num, m_b * num, m_a * num); }

	auto operator-(color_t col) const -> color_t { return color_t(m_r - col.m_r, m_g - col.m_g, m_b - col.m_b, m_a - col.m_a); }

	auto operator+(color_t col) const -> color_t { return color_t(m_r + col.m_r, m_g + col.m_g, m_b + col.m_b, m_a + col.m_a); }

	auto operator*(color_t col) const -> color_t { return color_t(m_r * col.m_r, m_g * col.m_g, m_b * col.m_b, m_a * col.m_a); }
	
	std::uint8_t m_r, m_g, m_b, m_a;
};
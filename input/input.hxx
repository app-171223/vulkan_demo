#pragma once

#include <cstdint>

enum class key_flag : std::uint32_t
{
	pushed = 0x0001,
	down = 0x8000
};

enum class key : std::uint8_t
{
	lmb = 0x01,
	rmb = 0x02,
	mmb = 0x04,
	x1mb = 0x05,
	x2mb = 0x06,
	space = 0x20
};

namespace input
{
	auto key_down(key code, key_flag flag) -> bool;
};
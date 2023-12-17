#include "input.hxx"

#include <windows.h>

auto input::key_down(key code, key_flag flag) -> bool
{
	return ::GetAsyncKeyState((std::uint32_t)code) & (std::uint32_t)(flag);
}
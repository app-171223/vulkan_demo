#include <windows.h>

#include "demo/demo.hxx"

auto __stdcall WinMain(HINSTANCE instance, HINSTANCE prev_instance, char* cmd_line, std::int32_t show_cmd) -> std::int32_t
{
	auto window = window_t{instance, "Vulkan demo", "demo class"};
	auto scene = draw::scene_t{window.get_hwnd()};
	auto timer = timer_t{};
	
	auto demo = demo_t{};

	for (; window.handle_message(); )
	{
		scene.begin();

		demo.render(scene, timer);
		
		scene.end();
		timer.update();
	}
}
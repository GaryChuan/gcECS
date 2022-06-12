#pragma once

namespace ecs::settings
{
	constexpr auto max_component_types			= 128u;
	constexpr auto max_component_per_entities	= 32u;
	constexpr auto max_entities_per_pool		= 2000u;
	constexpr auto max_entities					= 100000u;
	constexpr auto virtual_page_size			= 4096u;
}
/******************************************************************************
filename: ecs_settings.h
author: Gary Chuan gary.chuan@digipen.edu
Project: CS396 - Midterm Project
Description:
This file contains the declaration of ecs setting values.
******************************************************************************/
#pragma once

namespace ecs::settings
{
	constexpr auto max_component_types			= 128u;
	constexpr auto max_component_per_entities	= 32u;
	constexpr auto max_entities					= 100000u;
	constexpr auto max_entities_per_pool		= max_entities;
	constexpr auto virtual_page_size			= 4096u;
}
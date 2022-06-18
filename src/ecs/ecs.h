/******************************************************************************
filename: ecs.h
author: Gary Chuan gary.chuan@digipen.edu
Project: CS396 - Midterm Project
Description:
This file contains the includes of all related ecs and core header files as 
well as cpp headers.
******************************************************************************/
#pragma once

#include <span>
#include <array>
#include <cstdint>
#include <cstddef>
#include <tuple>
#include <memory>
#include <cassert>
#include <cstdint>
#include <vector>
#include <concepts>
#include <functional>
#include <type_traits>

#include "core.h"

//--------------------------------------------------------------
// FORWARD DECLARATION OF ECS MANAGER
//--------------------------------------------------------------
namespace ecs
{
	class manager;
}

//--------------------------------------------------------------
// FILES
//--------------------------------------------------------------

#include "ecs_settings.h"
#include "ecs_component.h"
#include "ecs_pool.h"
#include "ecs_archetype.h"
#include "ecs_query.h"
#include "ecs_system.h"
#include "ecs_manager.h"

//--------------------------------------------------------------
// INLINE FILES
//--------------------------------------------------------------

#include "details/ecs_query.hpp"
#include "details/ecs_component.hpp"
#include "details/ecs_system.hpp"
#include "details/ecs_pool.hpp"
#include "details/ecs_manager.hpp"
#include "details/ecs_archetype.hpp"
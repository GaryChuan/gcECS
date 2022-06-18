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

#include "ecs_settings.h"
#include "ecs_component.h"
#include "ecs_pool.h"
#include "ecs_archetype.h"
#include "ecs_query.h"
#include "ecs_system.h"
#include "ecs_manager.h"
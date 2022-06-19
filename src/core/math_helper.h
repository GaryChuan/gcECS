/******************************************************************************
filename: math.h
author: Gary Chuan gary.chuan@digipen.edu
Project: CS396 - Midterm Project
Description:
This file contains the includes of all math related header files and helper
functions.
******************************************************************************/
#pragma once
#include <cmath>

#include "math/vector2.h"
#include "math/vector3.h"
#include "math/arithmetic_type.h"

namespace core::math
{
	template <typename TVector>
	TVector Lerp(const TVector& start, const TVector& end, float interpolant)
	{
		return (1 - interpolant) * start + interpolant * end;
	}
}

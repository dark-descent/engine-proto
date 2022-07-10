#pragma once

#include "framework.hpp"
#include "Vector2.hpp"

PACK(struct Transform 
{
	Vector2 position;
	Vector2 rotation;
	Vector2 scale;
});
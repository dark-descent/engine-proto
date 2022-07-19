#pragma once

#include "framework.hpp"
#include "Vector2.hpp"

PACK(struct BoxCollider
{
	Vector2 scale;
});

PACK(struct CircleCollider
{
	float radius;
});
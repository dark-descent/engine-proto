#pragma once

#include "framework.hpp"

struct Entity;

PACK(struct Component
{
	Entity* entity;
	Component* nextComponent = nullptr;
});
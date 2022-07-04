#pragma once

struct Component;

using EntityHandle = size_t;

struct Entity
{
	Component* firstComponent = nullptr;
	EntityHandle* handle;
};
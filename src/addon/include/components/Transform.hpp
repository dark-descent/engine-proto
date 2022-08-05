#pragma once

#include "framework.hpp"
#include "Vector2.hpp"
#include "Component.hpp"

PACK(struct Transform
{
	Vector2 position;
	Vector2 rotation;
	Vector2 scale;
});

class JsTransform : public JsComponent
{
	static void constructCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
	{

	}

public:
	JS_COMPONENT_CTOR(JsTransform, JsTransform::constructCallback)
	{

	}
};
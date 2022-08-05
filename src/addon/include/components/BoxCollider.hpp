#pragma once

#include "framework.hpp"
#include "Vector2.hpp"
#include "Component.hpp"

PACK(struct BoxCollider
{
	Vector2 scale;
});

PACK(struct CircleCollider
{
	float radius;
});

class JsBoxCollider : public JsComponent
{
	static void constructCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
	{

	}

public:
	JS_COMPONENT_CTOR(JsBoxCollider, JsBoxCollider::constructCallback)
	{

	}
};

class JsCircleCollider : public JsComponent
{
	static void constructCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
	{

	}

public:
	JS_COMPONENT_CTOR(JsCircleCollider, JsCircleCollider::constructCallback)
	{

	}
};
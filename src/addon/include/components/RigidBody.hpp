#pragma once

#include "framework.hpp"
#include "Vector2.hpp"
#include "Component.hpp"

PACK(struct RigidBody
{
	float mass;
	Vector2 velocity;
});


class JsRigidBody : public JsComponent
{
	static void constructCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
	{
		// Engine& engine = fetchEngineFromInfo(info);
		printf("rigid body constructed!\n");
	}

public:
	JS_COMPONENT_CTOR(JsRigidBody, JsRigidBody::constructCallback)
	{

	}
};
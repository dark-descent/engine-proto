#pragma once

#include "framework.hpp"

class Engine
{
public:
	static void initialize(const v8::FunctionCallbackInfo<v8::Value>& info);
	static void destory(const v8::FunctionCallbackInfo<v8::Value>& info);
	Engine();

};
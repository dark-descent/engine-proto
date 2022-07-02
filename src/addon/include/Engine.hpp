#pragma once

#include "framework.hpp"

class Engine
{
private:
	static Engine* engine;

public:
	static void initialize(const v8::FunctionCallbackInfo<v8::Value>& info);
	static void destory(const v8::FunctionCallbackInfo<v8::Value>& info);

private:
	Engine();

};
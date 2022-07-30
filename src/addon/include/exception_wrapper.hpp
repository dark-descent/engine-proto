#pragma once

#include "framework.hpp"
#include "Engine.hpp"

template<typename Callback>
inline void exceptionWrapper(V8CallbackArgs args, Callback callback)
{
	try
	{
		callback(args);
	}
	catch(std::runtime_error err)
	{
		Engine* engine = Engine::get();
		engine->logger.error(err.what());
		args.GetIsolate()->ThrowException(createString(args.GetIsolate(), err.what()));
	}
	catch(const char* err)
	{
		Engine* engine = Engine::get();
		engine->logger.error(err);
		args.GetIsolate()->ThrowException(createString(args.GetIsolate(), err));
	}
	catch(std::string err)
	{
		Engine* engine = Engine::get();
		engine->logger.error(err);
		args.GetIsolate()->ThrowException(createString(args.GetIsolate(), err.c_str()));
	}
}
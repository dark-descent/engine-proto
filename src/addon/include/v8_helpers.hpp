#pragma once
#include "node.h"

inline v8::Local<v8::String> createString(v8::Isolate* isolate, const char* str)
{
	return v8::String::NewFromUtf8(isolate, str, v8::NewStringType::kNormal).ToLocalChecked();
}

template<typename T>
inline v8::Local<v8::String> createNumber(v8::Isolate* isolate, T num)
{
	return v8::Number::New(isolate, num).ToLocalChecked();
}

inline v8::Local<v8::Value> createError(v8::Isolate* isolate, const char* str)
{
	return v8::Exception::Error(createString(isolate, str));
}

inline v8::Local<v8::Value> createException(v8::Isolate* isolate, const char* str)
{
	return createError(isolate, str);
}
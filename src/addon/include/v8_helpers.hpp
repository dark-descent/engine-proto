#pragma once

#include "node.h"


inline v8::Local<v8::String> createString(v8::Isolate* isolate, const char* str)
{
	return v8::String::NewFromUtf8(isolate, str, v8::NewStringType::kNormal).ToLocalChecked();
}

template<typename T>
inline v8::Local<v8::Number> createNumber(v8::Isolate* isolate, T num)
{
	return v8::Number::New(isolate, static_cast<double>(num));
}

inline v8::Local<v8::Value> createError(v8::Isolate* isolate, const char* str)
{
	return v8::Exception::Error(createString(isolate, str));
}

inline v8::Local<v8::Value> createException(v8::Isolate* isolate, const char* str)
{
	return createError(isolate, str);
}

template<typename T>
inline v8::Local<v8::Value> createPointer(v8::Isolate* isolate, T pointer)
{
	uint64_t ptr = reinterpret_cast<uint64_t>(pointer);
	return createNumber<uint64_t>(isolate, ptr);
}

struct ObjectBuilder;

template<typename ConstructCallback>
inline v8::Local<v8::Object> createObject(v8::Isolate* isolate, ConstructCallback callback)
{
	v8::Local<v8::Object> obj = v8::Object::New(isolate);
	ObjectBuilder builder(isolate, obj);
	callback(builder);
	return obj;
}

struct ObjectBuilder
{
private:
	v8::Isolate* isolate_;
	v8::Local<v8::Context> ctx_;
	const v8::Local<v8::Object>& obj_;

public:
	ObjectBuilder(v8::Isolate* isolate, const v8::Local<v8::Object>& obj) : isolate_(isolate), ctx_(isolate->GetCurrentContext()), obj_(obj) { }

	void set(const char* key, const v8::Local<v8::Value>& val) { this->obj_->Set(this->ctx_, createString(this->isolate_, key), val); }

	void set(const char* key, const char* string) { this->obj_->Set(this->ctx_, createString(this->isolate_, key), createString(this->isolate_, string)); }

	void set(const char* key, bool boolean) { this->obj_->Set(this->ctx_, createString(this->isolate_, key), v8::Boolean::New(this->isolate_, boolean)); }

	template<typename T>
	void set(const char* key, T number) { this->obj_->Set(this->ctx_, createString(this->isolate_, key), createNumber(this->isolate_, number)); }

	template<typename ConstructCallback>
	void setObject(const char* key, ConstructCallback callback) { this->obj_->Set(this->ctx_, createString(this->isolate_, key), createObject(this->isolate_, callback)); }
};
#pragma once

#include "node.h"
#define V8CallbackArgs const v8::FunctionCallbackInfo<v8::Value>&

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

inline v8::Local<v8::Function> createFunction(v8::Isolate* isolate, v8::FunctionCallback callback)
{
	return v8::FunctionTemplate::New(isolate, callback)->GetFunction(isolate->GetCurrentContext()).ToLocalChecked();
}

inline v8::Local<v8::Function> createFunction(v8::Isolate* isolate, v8::FunctionCallback callback, void* pointer)
{
	v8::Local<v8::External> ptr = v8::External::New(isolate, pointer);
	return v8::FunctionTemplate::New(isolate, callback, ptr)->GetFunction(isolate->GetCurrentContext()).ToLocalChecked();
}

template<typename T>
inline v8::Local<v8::Function> createFunction(v8::Isolate* isolate, v8::FunctionCallback callback, T* pointer)
{
	return createFunction(isolate, callback, static_cast<void*>(pointer));
}

inline v8::Local<v8::SharedArrayBuffer> createSharedArrayBuffer(v8::Isolate* isolate, std::unique_ptr<v8::BackingStore> backingStore)
{
	return v8::SharedArrayBuffer::New(isolate, std::move(backingStore));
}

inline void throwException(v8::Isolate* isolate, const char* exception)
{
	isolate->ThrowException(createString(isolate, exception));
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
	v8::Local<v8::Object> obj_;

public:
	ObjectBuilder(v8::Isolate* isolate) : isolate_(isolate), ctx_(isolate->GetCurrentContext()), obj_(v8::Object::New(isolate)) { }
	ObjectBuilder(v8::Isolate* isolate, v8::Local<v8::Object> obj) : isolate_(isolate), ctx_(isolate->GetCurrentContext()), obj_(obj) { }

	void set(const char* key, const v8::Local<v8::Value>& val) { this->obj_->Set(this->ctx_, createString(this->isolate_, key), val); }

	void set(const char* key, const char* string) { this->obj_->Set(this->ctx_, createString(this->isolate_, key), createString(this->isolate_, string)); }

	void set(const char* key, bool boolean) { this->obj_->Set(this->ctx_, createString(this->isolate_, key), v8::Boolean::New(this->isolate_, boolean)); }

	template<typename T>
	void set(const char* key, T number) { this->obj_->Set(this->ctx_, createString(this->isolate_, key), createNumber(this->isolate_, number)); }

	void setFunction(const char* key, v8::FunctionCallback callback) { this->obj_->Set(this->ctx_, createString(this->isolate_, key), createFunction(this->isolate_, callback)); }

	template<typename T>
	void setFunction(const char* key, v8::FunctionCallback callback, T* data) { this->obj_->Set(this->ctx_, createString(this->isolate_, key), createFunction(this->isolate_, callback, data)); }

	template<typename ConstructCallback>
	void setObject(const char* key, ConstructCallback callback) { this->obj_->Set(this->ctx_, createString(this->isolate_, key), createObject(this->isolate_, callback)); }

	v8::Local<v8::Object> build()
	{
		return obj_;
	}
};
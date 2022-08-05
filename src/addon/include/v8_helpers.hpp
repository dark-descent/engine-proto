#pragma once

#include "node.h"

#define V8CallbackArgs const v8::FunctionCallbackInfo<v8::Value>&
#define V8UnpackArgs(__ARGS__) v8::Isolate* isolate = __ARGS__.GetIsolate(); \
v8::Local<v8::Context> context = isolate->GetCurrentContext(); \
const size_t argc = __ARGS__.Length()

template<typename T>
inline T getExternalData(V8CallbackArgs args) { return static_cast<T>(args.Data().As<v8::External>()->Value()); }

template<typename T>
inline T getExternalData(v8::Local<v8::Value> val) { return static_cast<T>(val.As<v8::External>()->Value()); }

inline v8::Local<v8::String> createString(v8::Isolate* isolate, const char* str)
{
	return v8::String::NewFromUtf8(isolate, str, v8::NewStringType::kNormal).ToLocalChecked();
}

template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
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

template<typename T>
inline v8::Local<v8::Value> createExternal(v8::Isolate* isolate, T* pointer)
{
	return v8::External::New(isolate, pointer);
}

template<typename T>
inline v8::Local<v8::Value> createExternal(v8::Isolate* isolate, T& ref)
{
	return v8::External::New(isolate, std::addressof(ref));
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

inline v8::Local<v8::Function> createFunction(v8::Isolate* isolate, v8::FunctionCallback callback, v8::Local<v8::Value> val)
{
	return v8::FunctionTemplate::New(isolate, callback, val)->GetFunction(isolate->GetCurrentContext()).ToLocalChecked();
}

inline v8::Local<v8::SharedArrayBuffer> createSharedArrayBuffer(v8::Isolate* isolate, std::unique_ptr<v8::BackingStore> backingStore)
{
	return v8::SharedArrayBuffer::New(isolate, std::move(backingStore));
}

inline v8::Local<v8::Array> createArray(v8::Isolate* isolate, size_t length = 0)
{
	return v8::Array::New(isolate, length);
}

inline void throwException(v8::Isolate* isolate, const char* exception)
{
	isolate->ThrowException(createString(isolate, exception));
}

struct ObjectBuilder;
struct ArrayBuilder;

template<typename ConstructCallback>
inline v8::Local<v8::Object> createObject(v8::Isolate* isolate, ConstructCallback callback)
{
	ObjectBuilder builder(isolate);
	callback(builder);
	return builder.build();
}

template<typename FillCallback>
inline v8::Local<v8::Array> createArray(v8::Isolate* isolate, FillCallback callback)
{
	ArrayBuilder builder(isolate);
	callback(builder);
	return builder.build();
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
	void setVal(const char* key, const v8::Local<v8::Value>& val) { this->obj_->Set(this->ctx_, createString(this->isolate_, key), val); }

	void set(const char* key, const char* string) { this->obj_->Set(this->ctx_, createString(this->isolate_, key), createString(this->isolate_, string)); }

	void set(const char* key, bool boolean) { this->obj_->Set(this->ctx_, createString(this->isolate_, key), v8::Boolean::New(this->isolate_, boolean)); }

	template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
	void set(const char* key, T number) { this->obj_->Set(this->ctx_, createString(this->isolate_, key), createNumber(this->isolate_, number)); }

	template<typename FillCallback>
	void setArray(const char* key, FillCallback callback = [](v8::Local<v8::Array>&) { }) { this->obj_->Set(this->ctx_, createString(this->isolate_, key), createArray(isolate_, callback)); }

	void setFunction(const char* key, v8::FunctionCallback callback) { this->obj_->Set(this->ctx_, createString(this->isolate_, key), createFunction(this->isolate_, callback)); }

	template<typename T>
	void setFunction(const char* key, v8::FunctionCallback callback, T* data) { this->obj_->Set(this->ctx_, createString(this->isolate_, key), createFunction(this->isolate_, callback, data)); }

	void setFunction(const char* key, v8::FunctionCallback callback, v8::Local<v8::Value> val) { this->obj_->Set(this->ctx_, createString(this->isolate_, key), createFunction(this->isolate_, callback, val)); }

	template<typename ConstructCallback>
	void setObject(const char* key, ConstructCallback callback) { this->obj_->Set(this->ctx_, createString(this->isolate_, key), createObject(this->isolate_, callback)); }

	v8::Local<v8::Object> build()
	{
		return obj_;
	}
};


struct ArrayBuilder
{
private:
	v8::Isolate* isolate_;
	v8::Local<v8::Context> ctx_;
	v8::Local<v8::Array> arr_;
	size_t pushIndex = 0;

public:
	ArrayBuilder(v8::Isolate* isolate) : isolate_(isolate), ctx_(isolate->GetCurrentContext()), arr_(v8::Array::New(isolate)) { }
	ArrayBuilder(v8::Isolate* isolate, v8::Local<v8::Array> obj) : isolate_(isolate), ctx_(isolate->GetCurrentContext()), arr_(obj) { }

	void push(const char* string) { arr_->Set(ctx_, pushIndex++, createString(isolate_, string)); }
	void push(bool boolean) { arr_->Set(ctx_, pushIndex++, v8::Boolean::New(isolate_, boolean)); }
	template<typename T>
	void push(T number) { arr_->Set(ctx_, pushIndex++, createNumber(isolate_, number)); }

	template<typename FillCallback>
	void pushArray(FillCallback callback = [](v8::Local<v8::Array>&) { }) { this->arr_->Set(this->ctx_, pushIndex++, createArray(isolate_, callback)); }

	void pushFunction(v8::FunctionCallback callback) { this->arr_->Set(this->ctx_, pushIndex++, createFunction(this->isolate_, callback)); }

	template<typename T>
	void pushFunction(v8::FunctionCallback callback, T* data) { this->arr_->Set(this->ctx_, pushIndex++, createFunction(this->isolate_, callback, data)); }

	template<typename ConstructCallback>
	void pushObject(ConstructCallback callback) { this->arr_->Set(this->ctx_, pushIndex++, createObject(this->isolate_, callback)); }

	v8::Local<v8::Array> build()
	{
		return arr_;
	}
};

class JsClass
{
public:
	template<typename T, class... Args, std::enable_if_t<std::is_constructible<T, Args&&...>::value, int> = 0>
	static T New(v8::Isolate* isolate, Args&&... args)
	{
		T jsClass = T(std::forward<Args>(args)...);
		jsClass.init(isolate);
		return jsClass;
	}

protected:
	v8::Persistent<v8::FunctionTemplate> template_;

public:
	JsClass(v8::Isolate* isolate, const char* className, v8::FunctionCallback callback, v8::Local<v8::Value> data = v8::Local<v8::Value>()) : template_()
	{
		v8::Local<v8::FunctionTemplate> ctor = v8::FunctionTemplate::New(isolate, callback, data);
		ctor->SetClassName(createString(isolate, className));
		template_.Reset(isolate, ctor);
	}

	template<typename T>
	JsClass(v8::Isolate* isolate, const char* className, v8::FunctionCallback callback, T* data) : JsClass(isolate, className, callback, v8::External::New(isolate, data)) { }

	v8::Local<v8::Function> getClass(v8::Isolate* isolate)
	{
		return template_.Get(isolate)->GetFunction(isolate->GetCurrentContext()).ToLocalChecked();
	}

	v8::Local<v8::Value> create(v8::Isolate* isolate)
	{
		auto val = template_.Get(isolate)->GetFunction(isolate->GetCurrentContext()).ToLocalChecked()->CallAsConstructor(isolate->GetCurrentContext(), 0, nullptr).ToLocalChecked();
		return val;
	}
};
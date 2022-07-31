#pragma once


#include "Arch.hpp"
#include "Component.hpp"

struct Entity
{
	char* entity;
	AllocIndex index;
	Handle<Arch>* arch;

	template<typename T = void>
	T* getComponent(ComponentIndex index)
	{
		size_t bitMask = arch->data.bitMask;
		int offset = -1;

		for (size_t i = 0, l = index + 1; i < l; i++)
		{
			if ((bitMask & 1) == 1)
				offset++;
			bitMask >>= 1;
		}

		if (offset > -1)
			return reinterpret_cast<T*>(entity + arch->data.offsets[offset]);
		return nullptr;
	}


	template<typename T = void>
	T* getComponent(ComponentInfo& componentInfo)
	{
		return getComponent<T>(componentInfo.getIndex());
	}
};

class Engine;

class JsEntity : public JsClass
{
	static void constructCallback(const v8::FunctionCallbackInfo<v8::Value>& info);

protected:
	virtual void onInit(v8::Local<v8::FunctionTemplate>& obj) override;

public:
	JsEntity(Engine& engine);
};
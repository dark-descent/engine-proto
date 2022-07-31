#include "Component.hpp"
#include "Engine.hpp"

ComponentInfo::ComponentInfo(size_t index, size_t bitMask, size_t size) : index_(index), bitMask_(bitMask), size_(size) { }

const size_t ComponentInfo::getSize() const
{
	return size_;
}

const size_t ComponentInfo::getBitMask() const
{
	return bitMask_;
}

const size_t ComponentInfo::getIndex() const
{
	return index_;
}

JsComponent::JsComponent(Engine& engine, const char* name, v8::FunctionCallback callback) : JsClass(engine.getIsolate(), name, callback, std::addressof(engine)), engine(engine)
{

}
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


v8::Persistent<v8::Symbol> JsComponent::internalSymbol_;

v8::Local<v8::Symbol> JsComponent::getInternalSymbol(v8::Isolate* isolate) { return internalSymbol_.Get(isolate); };

void JsComponent::ctorWrapper(V8CallbackArgs args)
{
	v8::Isolate* isolate = args.GetIsolate();
	v8::Local<v8::Context> ctx = isolate->GetCurrentContext();

	const size_t argc = args.Length();

	JsComponent* self = getExternalData<JsComponent*>(args);
	if (argc == 0 || !args[0]->Equals(ctx, internalSymbol_.Get(isolate)).ToChecked())
	{
		self->engine.logger.error("You can only create Components by using Entity.addComponent<T extends Component>(type: T)");
		isolate->ThrowException(createString(isolate, "You can only create Components by using Entity.addComponent<T extends Component>(type: T)"));
		return;
	}

	self->ctorCallback_(args);
}

void JsComponent::getIndexCallback(V8CallbackArgs args)
{
	JsComponent* self = getExternalData<JsComponent*>(args);
	args.GetReturnValue().Set(createNumber(args.GetIsolate(), self->index_));
}

JsComponent::JsComponent(Engine& engine, const char* name, uint64_t index, v8::FunctionCallback callback) : JsClass(engine.getIsolate(), name, ctorWrapper, this), engine(engine), index_(index), ctorCallback_(callback)
{
	v8::Isolate* isolate = engine.getIsolate();

	if (internalSymbol_.IsEmpty())
		internalSymbol_.Reset(isolate, v8::Symbol::New(isolate, createString(isolate, "__INTERNAL__")));

	auto fn = template_.Get(isolate)->GetFunction(isolate->GetCurrentContext()).ToLocalChecked();
	fn->SetAccessorProperty(createString(isolate, "index"), createFunction(isolate, getIndexCallback, this));
}
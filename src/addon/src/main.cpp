#include "framework.hpp"
#include "Engine.hpp"
#include "Bin.hpp"

using namespace v8;

PARSE_TEMPLATE(Test, TestParser, {
	U8(u8);
	U16(u16);
	U32(u32);
	U64(u64);
	I8(i8);
	I16(i16);
	I32(i32);
	I64(i64);
	I64V(i64v);
	F64V(f64v);
	STR(a);
	STR(b);
	STR(c);
	STR(d);
	});

NODE_MODULE_INIT()
{
	Isolate* isolate = context->GetIsolate();

	const char* binPath = "H:\\dmtrllv\\Code\\dark-descent\\editor\\test.bin";

	Test t = Test();
	t.u8 = 123;
	t.u16 = 123;
	t.u32 = 12345;
	t.u64 = 12345;
	t.i8 = 123;
	t.i16 = 123;
	t.i32 = 12345;
	t.i64 = 12345;
	t.i64v.push_back(1);
	t.i64v.push_back(2);
	t.i64v.push_back(3);
	t.i64v.push_back(4);
	t.i64v.push_back(5);
	t.f64v.push_back(0.0);
	t.f64v.push_back(1.2);
	t.f64v.push_back(3.4);
	t.f64v.push_back(5.6);
	t.f64v.push_back(9.6);

	t.a = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer hendrerit aliquet mattis. Duis finibus volutpat nunc a eleifend. Sed ac mi dolor. Nullam ac nunc non purus fermentum porttitor. Nam feugiat elit massa, et mollis sem ultricies sit amet. Nulla efficitur ante enim, in placerat elit condimentum non. Duis eu augue maximus, aliquam sem eu, porttitor ipsum. Cras pulvinar augue neque. Phasellus iaculis molestie augue eu accumsan. Fusce dapibus dui erat, in dictum libero venenatis non. Aenean quis nisl vulputate, interdum sapien ac, dictum erat.";
	t.b = "In fermentum odio vel nulla auctor bibendum. Nunc mollis, lorem quis suscipit lobortis, justo magna faucibus nibh, vel facilisis ex felis at leo. Fusce ac aliquet nisi. Aenean pellentesque arcu faucibus sem sagittis faucibus. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. Aliquam semper diam dui, sed consequat urna rhoncus quis. Donec nec mauris nunc. Vestibulum cursus feugiat leo nec tempor. Nam mattis pulvinar tortor sodales vehicula. In iaculis magna non est tristique cursus. Maecenas eget porta magna, eu rutrum arcu. Nam eu mollis turpis, vitae tempus lorem.";
	t.c = "Nulla facilisi. Mauris tristique fringilla ex, at pretium mauris fringilla ac. Nunc et ultrices enim. Cras ullamcorper neque et hendrerit ultrices. Vestibulum ornare ligula et ipsum fringilla dictum. Nullam aliquam justo eget urna sagittis elementum. Aliquam in fringilla velit. Nullam dolor est, tempor quis neque eu, facilisis imperdiet nisi. Pellentesque a nulla et turpis elementum finibus. Etiam ultrices semper dapibus. Maecenas eget quam volutpat, porttitor neque dictum, ultrices metus. Maecenas non neque tempor, viverra erat eget, interdum turpis. Pellentesque rhoncus tempus commodo. Aliquam massa est, sollicitudin ullamcorper lacinia nec, volutpat sed metus.";
	t.d = "Quisque nec purus a ante pulvinar laoreet. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia curae; Fusce ullamcorper dictum tincidunt. Vivamus hendrerit eget elit vitae metus.";

	TestParser.write(&t, binPath);

	Test testRead = Test();

	TestParser.read(binPath, &testRead);

	std::cout << testRead.a << "\n" << testRead.b << "\n" << testRead.c << "\n" << testRead.d << std::endl;

	exports->Set(context, createString(isolate, "initialize"), createFunction(isolate, Engine::initialize));
}
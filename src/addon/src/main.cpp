#include "framework.hpp"
#include "Engine.hpp"
#include "Bin.hpp"

using namespace v8;

BIN_TEMPLATE(Test, TestParser, {
	Bin::u8 u8;
	Bin::vector<Bin::u8> u8Vec;
	Bin::string string;
	});

NODE_MODULE_INIT()
{
	Isolate* isolate = context->GetIsolate();

	const char* binPath = "H:\\dmtrllv\\Code\\dark-descent\\editor\\test.bin";

	{
		Test t = Test();
		t.u8 = 123;
		t.string = "Helloa :D";

		for (size_t i = 0; i < 100; i++)
			t.u8Vec.push_back(i);

		TestParser.write(&t, binPath);
	}


	{
		Test testRead = Test();
		TestParser.read(binPath, &testRead);
		printf("u8: %u\n", testRead.u8);
		printf("string: %s\n", testRead.string.c_str());
		printf("u8Vec values: \t");
		for (size_t i = 0; i < testRead.u8Vec.size(); i++)
			printf("%u \t", testRead.u8Vec[i]);
		printf("\nu8Vec size: %zu\n", testRead.u8Vec.size());
	}

	exports->Set(context, createString(isolate, "initialize"), createFunction(isolate, Engine::initialize));
}
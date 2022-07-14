#include "framework.hpp"
#include "Engine.hpp"
#include "Bin.hpp"

using namespace v8;

BIN_TEMPLATE(Test, TestParser, {
	Bin::u8 u8;
	Bin::vector<Bin::u8> u8Vec;
	Bin::vector<Bin::string> stringArr;
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

		t.stringArr.push_back("A");
		t.stringArr.push_back("B");
		t.stringArr.push_back("C");
		t.stringArr.push_back("D");
		t.stringArr.push_back("E");
		t.stringArr.push_back("F");
		t.stringArr.push_back("G");
		t.stringArr.push_back("H");

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
		for (size_t i = 0; i < testRead.stringArr.size(); i++)
			printf("str[%zu]: %s\n", i, testRead.stringArr.at(i).c_str());
	}

	exports->Set(context, createString(isolate, "initialize"), createFunction(isolate, Engine::initialize));
}
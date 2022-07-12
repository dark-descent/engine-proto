#include "framework.hpp"
#include "Engine.hpp"
#include "Bin.hpp"

using namespace v8;

// This macro will define a struct and a Parser object with wich you can read and write files.
// The first parameter is the name of the struct
// The second parameter is the name of the Parser object
// The third parameter is the structs body (filled with Types)
// The types that you can pass in the body are defined in Bin.hpp (like: U8, U16, U32, U64, I8, I16, I32, I64) and expect a property name.
PARSE_TEMPLATE(Test, TestParser, {
	F64V(vec);
	});

// This macro expands to:
// struct Test { std::vector<double> vec; };
// Bin::Template<Test> TestParser = Bin::createTemplate<Test>([](auto builder) { builder.parse("{ F64V(vec); }"); });

NODE_MODULE_INIT()
{
	Isolate* isolate = context->GetIsolate();

	// the path where the binary file should be
	const char* binPath = "H:\\dmtrllv\\Code\\dark-descent\\editor\\test.bin";

	// lets create a Test struct (defined from the PARSE_TEMPLATE(...) above)
	Test t;

	// and insert doubles into its vector
	const size_t size = 200000;

	for (size_t i = 0; i < size; i++)
		t.vec.push_back((double)i);

	// and write to "H:\\dmtrllv\\Code\\dark-descent\\editor\\test.bin"
	TestParser.write(&t, binPath);


	// to read a binary file into a struct
	auto testr = TestParser.read(binPath);

	// and to check if the vector size matched
	std::cout << "Vector size did" << (testr.vec.size() == size ? "" : " not") << " match!" << std::endl;

	exports->Set(context, createString(isolate, "initialize"), createFunction(isolate, Engine::initialize));
}
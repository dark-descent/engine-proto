#pragma once

#include "framework.hpp"

#define ASCII(n) n + 48

class Bin
{
public:
	enum class Types
	{
		u8,
		u16,
		u32,
		u64,
		i8,
		i16,
		i32,
		i64,
		f32,
		f64,
		lf64,
		boolean,
		str,
		size,
	};

	enum class DynamicTypes
	{
		vector = 0x10,
		str = 0x20,
	};

	template<Types type>
	static constexpr const unsigned long long TypesIndex()
	{
		return static_cast<const unsigned long long>(type);
	}

	static const unsigned long long TypesIndex(Types type)
	{
		return static_cast<const unsigned long long>(type);
	}

	template<unsigned long long index>
	static constexpr Types TypesIndexToType()
	{
		return static_cast<Types>(index);
	}

	static const Types TypesIndexToType(unsigned long long index)
	{
		return static_cast<Types>(index);
	}

	template<DynamicTypes type>
	static constexpr const unsigned long long DynamicTypesIndex()
	{
		return static_cast<const unsigned long long>(type);
	}

	static const unsigned long long DynamicTypesIndex(DynamicTypes type)
	{
		return static_cast<const unsigned long long>(type);
	}

	template<unsigned long long index>
	static constexpr DynamicTypes DynamicTypesIndexToType()
	{
		return static_cast<DynamicTypes>(index);
	}

	static const DynamicTypes DynamicTypesIndexToType(unsigned long long index)
	{
		return static_cast<DynamicTypes>(index);
	}

	template<Types type>
	static constexpr bool isType(auto number)
	{
		return (static_cast<unsigned long long>(number) & TypesIndex<type>()) == TypesIndex<type>();
	}

	template<DynamicTypes type>
	static constexpr bool isDynamicType(auto number)
	{
		return (static_cast<unsigned long long>(number) & DynamicTypesIndex<type>()) == DynamicTypesIndex<type>();
	}

	static const size_t typeSizes[TypesIndex<Types::size>()];

	using TemplateInfo = std::vector<std::pair<size_t, Types>>;

	template<typename T>
	struct Template
	{
	private:
		struct DataBlocks
		{
			size_t size;
			size_t type;
			DataBlocks() : size(0), type(0) { };
		};

		TemplateInfo info_;

		std::vector<DataBlocks> dataBlocks_;

	public:
		Template(TemplateInfo info) : info_(std::move(info)), dataBlocks_()
		{
			bool isDynamicBlock = false;
			DataBlocks block;

			const auto push = [&]()
			{
				if (block.size != 0)
				{
					dataBlocks_.push_back(block);
					block = DataBlocks();
				}
			};

			for (const auto& info : info_)
			{
				if (TypesIndex(info.second) >= DynamicTypesIndex<DynamicTypes::vector>())
				{
					push();
					isDynamicBlock = true;
					block.type = TypesIndex(info.second);
					block.size = info.first;
				}
				else
				{
					if (isDynamicBlock)
					{
						push();
						isDynamicBlock = false;
					}

					block.size += info.first;
				}
			}
			push();
		};

		void write(T* data, const char* path)
		{
			std::ofstream os;
			os.open(path, std::ios::binary | std::ios::out);

			uintptr_t ptr = reinterpret_cast<uintptr_t>(data);

			for (const auto& info : info_)
			{
				if (isDynamicType<DynamicTypes::vector>(info.second))
				{
					std::vector<char>* v = reinterpret_cast<std::vector<char>*>(ptr);
					size_t size = v->size();
					os.write(reinterpret_cast<char*>(&size), sizeof(size_t));
					os.write(v->data(), size);
					ptr += sizeof(std::vector<void*>);
				}
				else if (isDynamicType<DynamicTypes::str>(info.second))
				{
					std::string* str = reinterpret_cast<std::string*>(ptr);
					size_t size = str->size();
					os.write(reinterpret_cast<char*>(&size), sizeof(size_t));
					os.write(str->c_str(), size);
					ptr += sizeof(std::string);
				}
				else
				{
					os.write(reinterpret_cast<char*>(ptr), info.first);
					ptr += info.first;
				}
			}
		}

		T read(const char* path, const size_t bufferSize = 1024)
		{
			T data = T();

			std::ifstream is;
			is.open(path, std::ios::binary | std::ios::in);

			is.seekg(0, is.end);
			const int maxSize = is.tellg();
			is.seekg(0);

			uintptr_t ptr = reinterpret_cast<uintptr_t>(&data);

			std::vector<char> buffer(16, 0);
			std::vector<char> dynamicBuffer(bufferSize, 0);
			uintptr_t dynBufferPtr = reinterpret_cast<uintptr_t>(dynamicBuffer.data());

			for (const Bin::Template<T>::DataBlocks& block : dataBlocks_)
			{
				if (block.type == 0)
				{
					is.read(buffer.data(), block.size);
					std::streamsize dataSize = is.gcount();
					if (dataSize != block.size)
					{
						std::runtime_error("Could not get correct size of data!");
					}
					else
					{
						memcpy(reinterpret_cast<void*>(ptr), buffer.data(), block.size);
						ptr += block.size;
					}
				}
				else // dynamic type
				{
					is.read(buffer.data(), sizeof(size_t));
					size_t s = *reinterpret_cast<size_t*>(buffer.data());

					char* dest;
					
					if (isDynamicType<DynamicTypes::str>(block.type))
					{
						std::string* str = reinterpret_cast<std::string*>(ptr);
						str->resize(s);
						dest = str->data();
						dest[s] = '\0';
						ptr += sizeof(std::string);
					}
					else if (isDynamicType<DynamicTypes::vector>(block.type))
					{
						std::vector<char>* v = reinterpret_cast<std::vector<char>*>(ptr);
						v->resize(s);
						dest = v->data();
						ptr += sizeof(std::vector<char>);
					}

					size_t m = s / bufferSize;
					size_t rest = s % bufferSize;
					
					for (size_t i = 0; i < m; i++)
					{
						is.read(dest, bufferSize);
						dest += bufferSize;
					}
					
					if (rest)
						is.read(dest, rest);
				}
			}

			return data;
		}
	};

	template<typename T>
	struct TemplateBuilder
	{
	private:
		TemplateInfo template_;

		TemplateBuilder& addType(Types type)
		{
			template_.push_back(std::make_pair(typeSizes[TypesIndex(type)], type));
			return *this;
		}

	public:
		TemplateBuilder& u8() { return addType(Types::u8); }
		TemplateBuilder& u16() { return addType(Types::u16); }
		TemplateBuilder& u32() { return addType(Types::u32); }
		TemplateBuilder& u64() { return addType(Types::u64); }
		TemplateBuilder& i8() { return addType(Types::i8); }
		TemplateBuilder& i16() { return addType(Types::i16); }
		TemplateBuilder& i32() { return addType(Types::i32); }
		TemplateBuilder& i64() { return addType(Types::i64); }
		TemplateBuilder& f32() { return addType(Types::f32); }
		TemplateBuilder& f64() { return addType(Types::f64); }
		TemplateBuilder& lf64() { return addType(Types::lf64); }
		TemplateBuilder& boolean() { return addType(Types::lf64); }

		TemplateBuilder& vector(Types type)
		{
			const auto i = TypesIndex(type);
			template_.push_back(std::make_pair(typeSizes[i], TypesIndexToType(i | DynamicTypesIndex<DynamicTypes::vector>())));
			// printf("added vector of type %zu\n", i);
			return *this;
		}

		TemplateBuilder& string()
		{
			template_.push_back(std::make_pair(typeSizes[TypesIndex<Types::str>()], TypesIndexToType(TypesIndex<Types::str>() | DynamicTypesIndex<DynamicTypes::str>())));
			return *this;
		}

		TemplateBuilder& parse(const char* str)
		{
			size_t i = 0;
			char c;
			char buf[8] = { 0 };
			size_t bufI = 0;
			bool ignore = false;

			const auto getType = [&buf]()
			{
				if (buf[0] == 'u') // unsigned number
				{
					switch (buf[1])
					{
					case ASCII(8): // uint8_t
						return Types::u8;
					case ASCII(1): // uint16_t
						return Types::u16;
					case ASCII(3): // uint32_t
						return Types::u32;
					case ASCII(6): // uint64_t
						return Types::u64;
					}
				}
				else if (buf[0] == 'i') // signed number
				{
					switch (buf[1])
					{
					case ASCII(8): // int8_t
						return Types::u8;
					case ASCII(1): // int16_t
						return Types::u16;
					case ASCII(3): // int32_t
						return Types::u32;
					case ASCII(6): // int64_t
						return Types::u64;
					}
				}
				else if (buf[0] == 'b')
				{
					return Types::boolean;
				}
				else if (buf[0] == 'f')
				{
					return buf[1] == '3' ? Types::f32 : Types::f64;
				}
				else if (buf[0] = 'l')
				{
					return Types::lf64;
				}
				throw std::runtime_error("Could not parse type!");
			};

			do
			{
				c = str[i++];

				if (c < 40 || c == '{' || c == '}')
					continue;

				if (c == '(')
				{
					ignore = true;
					buf[bufI] = '\0';

					if (buf[0] == 's')
						string();
					else if (buf[2] == 'v' || buf[3] == 'v' || buf[4] == 'v')
						vector(getType());
					else
						addType(getType());

					bufI = 0;
				}
				else if (!ignore)
				{
					if (c >= 65)
						buf[bufI++] = c + 32;
					else
						buf[bufI++] = c;
				}
				else if (c == ';')
				{
					ignore = false;
				}
			} while (c != '\0');

			return *this;
		}

		Template<T> build()
		{
			return Template<T>(template_);
		}
	};

	template<typename T>
	using TemplateBuildCallback = void(*)(TemplateBuilder<T>&);

	template<typename T>
	static Template<T> createTemplate(TemplateBuildCallback<T> callback)
	{
		TemplateBuilder<T> builder;
		callback(builder);
		return builder.build();
	}
};

const size_t Bin::typeSizes[TypesIndex<Types::size>()] = {
	sizeof(uint8_t),
	sizeof(uint16_t),
	sizeof(uint32_t),
	sizeof(uint64_t),
	sizeof(int8_t),
	sizeof(int16_t),
	sizeof(int32_t),
	sizeof(int64_t),
	sizeof(float),
	sizeof(double),
	sizeof(bool),
	sizeof(char),
	sizeof(long double),
};

#define U8(__NAME__) uint8_t __NAME__
#define U16(__NAME__) uint16_t __NAME__
#define U32(__NAME__) uint32_t __NAME__
#define U64(__NAME__) uint64_t __NAME__
#define I8(__NAME__) int8_t __NAME__
#define I16(__NAME__) int16_t __NAME__
#define I32(__NAME__) int32_t __NAME__
#define I64(__NAME__) int64_t __NAME__

#define U8V(__NAME__)  std::vector<uint8_t> __NAME__
#define U16V(__NAME__) std::vector<uint16_t> __NAME__
#define U32V(__NAME__) std::vector<uint32_t> __NAME__
#define U64V(__NAME__) std::vector<uint64_t> __NAME__
#define I8V(__NAME__)  std::vector<int8_t> __NAME__
#define I16V(__NAME__) std::vector<int16_t> __NAME__
#define I32V(__NAME__) std::vector<int32_t> __NAME__
#define I64V(__NAME__) std::vector<int64_t> __NAME__

#define F32(__NAME__) float __NAME__
#define F64(__NAME__) double __NAME__
#define LF64(__NAME__) long double __NAME__

#define B(__NAME__) bool __NAME__

#define F32V(__NAME__) std::vector<float> __NAME__
#define F64V(__NAME__) std::vector<double> __NAME__
#define LF64V(__NAME__) std::vector<long double> __NAME__

#define STR(__NAME__) std::string __NAME__

#define PARSE_TEMPLATE(__STRUCT_NAME__, __TEMPLATE_NAME__, ...) PACK(struct __STRUCT_NAME__ ##__VA_ARGS__); \
Bin::Template<__STRUCT_NAME__> __TEMPLATE_NAME__ = Bin::createTemplate<__STRUCT_NAME__>([](auto builder) { builder.parse(#__VA_ARGS__); });

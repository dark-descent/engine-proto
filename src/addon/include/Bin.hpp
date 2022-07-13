#pragma once

#include "framework.hpp"
#include "Enum.hpp"

class Bin
{
public:
	using u8 = uint8_t;
	using u16 = uint16_t;
	using u32 = uint32_t;
	using u64 = uint64_t;
	using i8 = int8_t;
	using i16 = int16_t;
	using i32 = int32_t;
	using i64 = int64_t;
	using f32 = float;
	using f64 = double;
	using lf64 = long double;
	using boolean = bool;

	using string = std::string;

	template<typename T>
	using vector = std::vector<T>;

	enum class Type
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
		boolean,
		size,
		vector = 0x10,
		str = 0x20,
	};

	static const size_t typeSizes[Enum::Cast(Type::size)];

	using TemplateInfo = std::vector<std::pair<size_t, Type>>;

	template<typename T>
	struct Template
	{
	private:
		struct DataBlock
		{
			size_t size;
			Type type;
			DataBlock() : size(0), type(Enum::Wrap<Type>(0)) { };
		};

		TemplateInfo info_;

		std::vector<DataBlock> dataBlocks_;

	public:
		Template(TemplateInfo info) : info_(std::move(info)), dataBlocks_()
		{
			bool isDynamicBlock = false;
			DataBlock block;

			const auto push = [&]()
			{
				if (block.size != 0)
				{
					dataBlocks_.push_back(block);
					block = DataBlock();
				}
			};

			for (const auto& info : info_)
			{
				if (Enum::Cast(info.second) >= Enum::Cast(Type::vector))
				{
					push();
					isDynamicBlock = true;
					block.type = info.second;
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
				if (Enum::Has(info.second, Type::vector))
				{
					std::vector<char>* v = reinterpret_cast<std::vector<char>*>(ptr);
					size_t size = v->size();
					os.write(reinterpret_cast<char*>(&size), sizeof(size_t));
					os.write(v->data(), size);
					ptr += sizeof(std::vector<void*>);
				}
				else if (Enum::Has(info.second, Type::str))
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

		void read(const char* path, T* obj, const size_t bufferSize = 1024)
		{
			std::ifstream is;
			is.open(path, std::ios::binary | std::ios::in);

			is.seekg(0, is.end);
			const int maxSize = is.tellg();
			is.seekg(0);

			char* ptr = reinterpret_cast<char*>(obj);

			std::vector<char> buffer(16, 0);
			std::vector<char> dynamicBuffer(bufferSize, 0);
			uintptr_t dynBufferPtr = reinterpret_cast<uintptr_t>(dynamicBuffer.data());

			for (const Bin::Template<T>::DataBlock& block : dataBlocks_)
			{
				if (Enum::Cast(block.type) == 0)
				{
					is.read(ptr, block.size);
					ptr += block.size;
				}
				else
				{
					is.read(buffer.data(), sizeof(size_t));
					size_t s = *reinterpret_cast<size_t*>(buffer.data());

					char* dest;

					if (block.type == Type::str)
					{
						std::string* str = reinterpret_cast<std::string*>(ptr);
						str->resize(s);
						dest = str->data();
						dest[s] = '\0';
						ptr += sizeof(std::string);
					}
					else if (Enum::Has(block.type, Type::vector))
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
		}
	};

	template<typename T>
	struct TemplateBuilder
	{
	private:
		TemplateInfo template_;

		inline Type getVectorType(Type type)
		{
			return Enum::And(type, Enum::Wrap<Type>(0xF));
		}

		TemplateBuilder& addType(Type type)
		{
			if (Enum::Cast(type) < Enum::Cast(Type::size))
				template_.push_back(std::make_pair(typeSizes[Enum::Cast(type)], type));
			else if (Enum::Has(type, Type::vector))
				template_.push_back(std::make_pair(typeSizes[Enum::Cast(getVectorType(type))], type));
			else if (Enum::Has(type, Type::str))
				template_.push_back(std::make_pair(1, type));
			return *this;
		}

	public:
		TemplateBuilder& u8() { return addType(Type::u8); }
		TemplateBuilder& u16() { return addType(Type::u16); }
		TemplateBuilder& u32() { return addType(Type::u32); }
		TemplateBuilder& u64() { return addType(Type::u64); }
		TemplateBuilder& i8() { return addType(Type::i8); }
		TemplateBuilder& i16() { return addType(Type::i16); }
		TemplateBuilder& i32() { return addType(Type::i32); }
		TemplateBuilder& i64() { return addType(Type::i64); }
		TemplateBuilder& f32() { return addType(Type::f32); }
		TemplateBuilder& f64() { return addType(Type::f64); }
		TemplateBuilder& boolean() { return addType(Type::boolean); }

		TemplateBuilder& vector(Type type)
		{
			const auto i = Enum::Cast(type);
			template_.push_back(std::make_pair(typeSizes[i], Enum::Or(type, Type::vector)));
			return *this;
		}

		TemplateBuilder& string()
		{
			template_.push_back(std::make_pair(1, Type::str));
			return *this;
		}

		TemplateBuilder& parse(const char* str)
		{
			size_t i = 0;
			char c;
			bool found = false;
			Type type = Enum::Wrap<Type>(0);

			do
			{
				c = str[i++];

				if (c < 40 || c == '{' || c == '}')
					continue;

				if (found && (c == ';'))
				{
					found = false;
				}
				else if (!found && (c == ':'))
				{
					i++;
					if (str[i] == ':')
						i++;

					c = str[i];

					if (c == 's') // string
					{
						type = Type::str;
					}
					else
					{
						if (c == 'v') // vector
						{
							i += 12;
							c = str[i];
							type = Type::vector;
						}

						if (c == 'u') // unsigned
						{
							c = str[++i];
							if (c == '8')
							{
								type = Enum::Or(type, Type::u8);
							}
							else if (c == '1')
							{
								type = Enum::Or(type, Type::u16);
							}
							else if (c == '3')
							{
								type = Enum::Or(type, Type::u32);
							}
							else if (c == '6')
							{
								type = Enum::Or(type, Type::u64);
							}
						}
						else if (c == 'i') // signed
						{
							c = str[++i];
							if (c == '8')
							{
								type = Enum::Or(type, Type::i8);
							}
							else if (c == '1')
							{
								type = Enum::Or(type, Type::i16);
							}
							else if (c == '3')
							{
								type = Enum::Or(type, Type::i32);
							}
							else if (c == '6')
							{
								type = Enum::Or(type, Type::i64);
							}
						}
						else if (c == 'b') // boolean
						{
							type = Enum::Or(type, Type::boolean);
						}
						else if (c == 'f')
						{
							c = str[++i];
							if (c == '3')
							{
								type = Enum::Or(type, Type::f32);
							}
							else if (c == '6')
							{
								type = Enum::Or(type, Type::f64);
							}
						}
					}

					addType(type);
					Type type = Enum::Wrap<Type>(0);
					found = true;
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

constexpr size_t Bin::typeSizes[Enum::Cast(Type::size)] = {
	1,
	2,
	3,
	4,
	1,
	2,
	3,
	4,
	2,
	4,
	1,
};

#define BIN_TEMPLATE(__STRUCT_NAME__, __TEMPLATE_NAME__, ...) PACK(struct __STRUCT_NAME__ ##__VA_ARGS__); \
Bin::Template<__STRUCT_NAME__> __TEMPLATE_NAME__ = Bin::createTemplate<__STRUCT_NAME__>([](auto builder) { builder.parse(#__VA_ARGS__); });

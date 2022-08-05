#pragma once

#include "framework.hpp"

namespace Bin
{
	enum Type : uint64_t
	{
		U8,
		U16,
		U32,
		U64,
		I8,
		I16,
		I32,
		I64,
		F32,
		F64,
		BOOL,
		PRIMITIVES_SIZE,
		VECTOR,
		STRING,
		STRUCT,
		END = 0x10Ui64
	};

	constexpr uint8_t typeSizes[Bin::Type::PRIMITIVES_SIZE] = {
		1, 2, 4, 8,
		1, 2, 4, 8,
		4, 8, 1
	};

	PACK(struct TypeInfo
	{
		size_t type;
		size_t size;
		char* ptr;

		TypeInfo();
		TypeInfo(size_t type, size_t size);
		TypeInfo(size_t type, size_t size, char* ptr);
	});

	using Header = std::vector<TypeInfo>;

	class Writer
	{
	private:
		Header header_;

		std::string file_;

		void addPrimitiveType(Type type, char* ptr);

	public:
		Writer(std::string file);
		Writer(const Writer&) = delete;
		Writer(Writer&&) = delete;

		template<typename T>
		Writer& write(std::vector<T>& vector)
		{
			const size_t vectorSize = vector.size();
			header_.push_back(TypeInfo(Type::VECTOR, vectorSize));
			if constexpr (is_specialization<T, std::vector>::value)
			{
				for (size_t i = 0; i < vectorSize; i++)
					write<T::value_type>(vector[i]);
			}
			else if constexpr (std::is_same<T, std::string>::value)
			{
				for (size_t i = 0; i < vectorSize; i++)
				{
					std::string& str = vector.at(i);
					header_.push_back(TypeInfo(Type::STRING, str.length(), str.data()));
				}
			}
			else
			{
				write(*vector.data());
			}
			header_.push_back(TypeInfo(Type::VECTOR | Type::END, 0));
			return *this;
		}

		Writer& write(std::string& str)
		{
			header_.push_back(TypeInfo(Type::STRING, str.length(), reinterpret_cast<char*>(std::addressof(str))));
			return *this;
		}

		template<typename T>
		Writer& write(T& value)
		{
			if constexpr (std::is_same<T, uint8_t>::value)
			{
				addPrimitiveType(Type::U8, reinterpret_cast<char*>(std::addressof(value)));
			}
			else if constexpr (std::is_same<T, uint16_t>::value)
			{
				addPrimitiveType(Type::U16, reinterpret_cast<char*>(std::addressof(value)));
			}
			else if constexpr (std::is_same<T, uint32_t>::value)
			{
				addPrimitiveType(Type::U32, reinterpret_cast<char*>(std::addressof(value)));
			}
			else if constexpr (std::is_same<T, uint64_t>::value)
			{
				addPrimitiveType(Type::U64, reinterpret_cast<char*>(std::addressof(value)));
			}
			else if constexpr (std::is_same<T, int8_t>::value)
			{
				addPrimitiveType(Type::I8, reinterpret_cast<char*>(std::addressof(value)));
			}
			else if constexpr (std::is_same<T, int16_t>::value)
			{
				addPrimitiveType(Type::I16, reinterpret_cast<char*>(std::addressof(value)));
			}
			else if constexpr (std::is_same<T, int32_t>::value)
			{
				addPrimitiveType(Type::I32, reinterpret_cast<char*>(std::addressof(value)));
			}
			else if constexpr (std::is_same<T, int64_t>::value)
			{
				addPrimitiveType(Type::I64, reinterpret_cast<char*>(std::addressof(value)));
			}
			else if constexpr (std::is_same<T, float>::value)
			{
				addPrimitiveType(Type::F32, reinterpret_cast<char*>(std::addressof(value)));
			}
			else if constexpr (std::is_same<T, double>::value)
			{
				addPrimitiveType(Type::F64, reinterpret_cast<char*>(std::addressof(value)));
			}
			else if constexpr (std::is_same<T, bool>::value)
			{
				addPrimitiveType(Type::BOOL, reinterpret_cast<char*>(std::addressof(value)));
			}
			else
			{
				header_.push_back(TypeInfo(Type::STRUCT, sizeof(T), reinterpret_cast<char*>(std::addressof(value))));
			}
			return *this;
		}

		void flush();

	private:
		void flushHeader(std::ofstream& os);

		size_t flushVector(std::ofstream& os, size_t index, size_t size);
	};

	struct Parser
	{
	private:
		std::ifstream& is;
		size_t infoIndex;
		Header header;

	public:
		Parser(std::ifstream& is);

		inline const Header& getHeader() { return header; }

	private:
		void read(char* ptr);
		size_t readVector(char* ptr, size_t nextIndex, size_t size);

	public:
		template<typename T>
		void read(T& data)
		{
			read(reinterpret_cast<char*>(std::addressof(data)));
		}
	};

	class Reader
	{
		std::string file_;

	public:
		Reader(std::string file);
		Reader(const Reader&) = delete;
		Reader(Reader&&) = delete;

		template<typename T>
		void read(T callback)
		{
			std::ifstream is = std::ifstream(file_, std::ifstream::beg | std::ifstream::binary);
			Parser parser = Parser(is);
			callback(parser);
		}
	};
};
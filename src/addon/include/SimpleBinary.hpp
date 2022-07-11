#pragma once

#include "framework.hpp"

namespace SimpleBinary
{
	enum Types
	{
		NONE = -1,
		U8,
		U16,
		U32,
		U64,
		I8,
		I16,
		I32,
		I64,
		FLOAT,
		DOUBLE,
		LONG_DOUBLE,
		BOOL,
		SIZE
	};

	template<typename T>
	inline static T cast(void* value) { return *static_cast<T*>(value); }

	class Reader
	{
	public:
		static void readHeader(const char* file, uint8_t* header);
	private:

		std::ifstream fileStream_;

		uint8_t sizes_[SimpleBinary::Types::SIZE];

	public:
		Reader(const char* file);
		~Reader();

		void getNext();

		template<typename ReadCallback, size_t ChunkSize = 1024>
		void read(ReadCallback callback)
		{
			fileStream_.seekg(0, fileStream_.end);
			const int maxSize = fileStream_.tellg();
			fileStream_.seekg(Types::SIZE);

			size_t parsedSize = Types::SIZE;

			Types type = Types::NONE;
			bool shouldReadSize = false;
			size_t arrSize = 0;
			size_t dataByteSize = 0;
			uint64_t val = 0;
			std::vector<uint8_t> parsedValues;

			std::vector<char> buffer(ChunkSize, 0);

			size_t index = 0;

			while (parsedSize < maxSize)
			{
				fileStream_.read(buffer.data(), ChunkSize);

				std::streamsize dataSize = fileStream_.gcount();

				for (int i = 0; i < dataSize; i++)
				{
					if (type == Types::NONE)
					{
						type = static_cast<Types>(buffer[i] & 0x0F);
						dataByteSize = sizes_[type];
						shouldReadSize = buffer[i] & 0xF0;
					}
					else if (shouldReadSize)
					{
						shouldReadSize = false;
					}
					else
					{
						parsedValues.push_back(static_cast<uint8_t>(buffer[i]));

						if (parsedValues.size() == dataByteSize)
						{
							val = 0;
							for (size_t i = dataByteSize; i > 0; --i)
							{
								val <<= 8;
								val |= parsedValues[i - 1];
							}
							callback(type, static_cast<void*>(&val), index);
							type = Types::NONE;
							index++;
							parsedValues.resize(0);
						}
					}
				}

				parsedSize += ChunkSize;
			}
		}
	};

	class Writer
	{
	private:
		std::ofstream fileStream_;

		uint8_t sizes_[SimpleBinary::Types::SIZE] = {
			static_cast<uint8_t>(sizeof(uint8_t)),
			static_cast<uint8_t>(sizeof(uint16_t)),
			static_cast<uint8_t>(sizeof(uint32_t)),
			static_cast<uint8_t>(sizeof(uint64_t)),
			static_cast<uint8_t>(sizeof(int8_t)),
			static_cast<uint8_t>(sizeof(int16_t)),
			static_cast<uint8_t>(sizeof(int32_t)),
			static_cast<uint8_t>(sizeof(int64_t)),
			static_cast<uint8_t>(sizeof(float)),
			static_cast<uint8_t>(sizeof(double)),
			static_cast<uint8_t>(sizeof(long double)),
			static_cast<uint8_t>(sizeof(bool)),
		};;

		template<typename T>
		void writeParsedValue(Types type, T val)
		{
			fileStream_ << static_cast<uint8_t>(type);
			const size_t typeSize = sizes_[type];
			uint64_t v = *static_cast<uint64_t*>(static_cast<void*>(&val));
			for (size_t i = 0; i < typeSize; i++)
			{
				fileStream_ << static_cast<uint8_t>(v & 0xFF);
				v >>= 8;
			}
		}

	public:
		Writer(const char* file);
		Writer(const char* file, bool append);
		~Writer();

		void write(uint8_t);
		void write(uint16_t);
		void write(uint32_t);
		void write(uint64_t);
		void write(int8_t);
		void write(int16_t);
		void write(int32_t);
		void write(int64_t);
		void write(float);
		void write(double);
		void write(long double);
		void write(bool);

		template<typename T, typename ...Args>
		void write(T a, Args... rest)
		{
			write(a);
			write(rest...);
		}
	};
};
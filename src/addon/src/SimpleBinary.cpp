#include "SimpleBinary.hpp"

void SimpleBinary::Reader::readHeader(const char* file, uint8_t* header)
{
	std::ifstream fileStream(file, std::ifstream::binary);
	fileStream.seekg(0);

	char buffer[SimpleBinary::Types::SIZE] = { 0 };

	fileStream.read(buffer, SimpleBinary::Types::SIZE);

	for(size_t i = 0; i < SimpleBinary::Types::SIZE; i++)
	{
		header[i] = static_cast<uint8_t>(buffer[i]);
	}
}

SimpleBinary::Reader::Reader(const char* file) : fileStream_()
{
	memset(sizes_, 0, SimpleBinary::Types::SIZE);
	fileStream_.open(file, std::fstream::binary);

	if (!fileStream_.is_open())
		throw std::runtime_error("Could not get file stream!");

	Reader::readHeader(file, sizes_);
}

SimpleBinary::Reader::~Reader()
{
	fileStream_.close();
}

SimpleBinary::Writer::Writer(const char* file) : SimpleBinary::Writer::Writer(file, false)
{ }

SimpleBinary::Writer::Writer(const char* file, bool append) : fileStream_()
{
	int openMode = std::fstream::binary;

	if (append)
		openMode |= std::fstream::ate;

	fileStream_.open(file, openMode);

	if (!fileStream_.is_open())
		throw std::runtime_error("Could not get file stream!");

	if (append)
	{
		Reader::readHeader(file, sizes_);
	}
	else
	{
		for (size_t i = 0; i < Types::SIZE; i++)
			fileStream_ << sizes_[i];
	}
}

SimpleBinary::Writer::~Writer()
{
	fileStream_.close();
}

void SimpleBinary::Writer::write(uint8_t val)
{
	writeParsedValue(U8, val);
}

void SimpleBinary::Writer::write(uint16_t val)
{
	writeParsedValue(U16, val);
}

void SimpleBinary::Writer::write(uint32_t val)
{
	writeParsedValue(U32, val);
}

void SimpleBinary::Writer::write(uint64_t val)
{
	writeParsedValue(U64, val);
}

void SimpleBinary::Writer::write(int8_t val)
{
	writeParsedValue(I8, val);
}

void SimpleBinary::Writer::write(int16_t val)
{
	writeParsedValue(I16, val);
}

void SimpleBinary::Writer::write(int32_t val)
{
	writeParsedValue(I32, val);
}

void SimpleBinary::Writer::write(int64_t val)
{
	writeParsedValue(I64, val);
}

void SimpleBinary::Writer::write(float val)
{
	writeParsedValue(FLOAT, val);
}

void SimpleBinary::Writer::write(double val)
{
	writeParsedValue(DOUBLE, val);
}

void SimpleBinary::Writer::write(long double val)
{
	writeParsedValue(LONG_DOUBLE, val);
}

void SimpleBinary::Writer::write(bool val)
{
	writeParsedValue(BOOL, val);
}

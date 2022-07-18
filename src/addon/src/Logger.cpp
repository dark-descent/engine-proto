#include "Logger.hpp" 

namespace Logger
{
	void log(const char* str)
	{
		std::fputs(str, stdout);
	}

	void log(std::string& str)
	{
		std::fputs(str.c_str(), stdout);
	}
	
	void log(std::filesystem::path& str)
	{
		std::fputs(str.string().c_str(), stdout);
	}

	void log(ILog& value)
	{
		value.log();
	}

	void log(IToString& value)
	{
		std::string str = value.toString();
		log(str.c_str());
	}
}
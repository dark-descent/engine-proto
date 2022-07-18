#pragma once

#include "framework.hpp"

namespace Logger
{
	class ILog
	{
	public:
		virtual void log() = 0;
	};

	class IToString
	{
	public:
		virtual std::string toString() = 0;
	};

	void log(const char* str);

	void log(std::string& str);

	void log(std::filesystem::path& str);

	template<typename T>
	void log(T& value)
	{
		if constexpr (std::is_base_of<ILog, T>::value)
		{
			value.log();
		}
		else if constexpr (std::is_base_of<IToString, T>::value)
		{
			std::string msg = value.toString();
			std::fputs(msg.c_str(), stdout);
		}
		else if constexpr (std::is_fundamental<T>::value)
		{
			auto msg = std::format("{}", value);
			std::fputs(msg.c_str(), stdout);
		}
		else
		{
			auto msg = std::format("No toString() or log() implementations found for {}", typeid(T).name());
			std::fputs(msg.c_str(), stdout);
		}
	}

	template <typename T>
	void log(T* val)
	{
		log(*val);
		log("\n");
	}

	template<typename T, typename... Args>
	void log(T& value, Args&...args)
	{
		log(value);
		log(" ");
		log(args...);
		log("\n");
	}
};
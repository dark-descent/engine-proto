#pragma once

#include "framework.hpp"

class Scene
{
private:
	std::string name_;
	std::string path_;

public:
	Scene(std::string name, std::string path);

	const std::string& name();
	const std::string& path();

	void addEntity();
};
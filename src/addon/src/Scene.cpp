#include "Scene.hpp"

Scene::Scene(std::string name, std::string path) : name_(name), path_(path)
{

}

const std::string& Scene::name()
{
	return name_;
}

const std::string& Scene::path()
{
	return path_;
}

void Scene::addEntity()
{
	printf("add entity to scene %s\n", name_.c_str());
}
#include "Scene.hpp"
#include "Engine.hpp"

Scene::Scene(Engine& engine, std::string name, std::string path) :
	engine_(engine),
	name_(name),
	path_(path),
	archTypes_(),
	rootArchType_(archTypes_.emplace_back(0, 0)),
	entityHandles_()
{

}


Scene::Scene(Scene&& other) :
	engine_(other.engine_),
	name_(std::move(other.name_)),
	path_(std::move(other.path_)),
	archTypes_(std::move(other.archTypes_)),
	rootArchType_(other.rootArchType_),
	entityHandles_(std::move(other.entityHandles_))
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

EntityHandle& Scene::addEntity(std::string name)
{
	return entityHandles_.alloc();
}

void Scene::addComponentToEntity(EntityHandle& entity, size_t component)
{
	printf("got bitmask %zu\n", engine_.getComponentBitMask(component));
}
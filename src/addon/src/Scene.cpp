#include "Scene.hpp"
#include "Engine.hpp"

Scene::Scene(Engine& engine, std::string name, std::string path) :
	engine_(engine),
	name_(name),
	path_(path),
	archTypes_(),
	entityHandles_()
{
	addArchType(0);
}

Scene::Scene(Scene&& other) :
	engine_(other.engine_),
	name_(std::move(other.name_)),
	path_(std::move(other.path_)),
	archTypes_(std::move(other.archTypes_)),
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

ArchTypeIndex Scene::addArchType(size_t bitMask)
{
	ArchTypeIndex index = archTypes_.size();
	size_t bitMaskTest = bitMask;
	std::vector<ComponentLayout> componentLayouts;
	size_t size = 0;
	for (size_t i = 0; i < 64; i++)
	{
		if (bitMaskTest & 1)
		{
			const Component& component = engine_.getComponent(i);
			const size_t s = component.getSize();
			componentLayouts.emplace_back(component.getBitMask(), s);
			size += s;
		}
		bitMaskTest >>= 1;
	}
	archTypes_.emplace_back(index, bitMask, size, componentLayouts);
	return index;
}

Handle<Entity>& Scene::addEntity(std::string name)
{
	Handle<Entity>& handle = entityHandles_.alloc();
	handle.data.entity = nullptr;
	handle.data.archType = 0;
	return handle;
}

void Scene::addComponentToEntity(Handle<Entity>& handle, size_t componentIndex)
{
	ArchType& arch = getArchType(handle.data.archType);

	const Component c = engine_.getComponent(componentIndex);

	const size_t componentBitMask = c.getBitMask();
	const size_t newArchBitMask = arch.bitMask | componentBitMask;

	auto it = std::find_if(arch.add.begin(), arch.add.end(), [&](const ArchTypeIndex& i) { return getArchType(i).bitMask == newArchBitMask; });

	ArchTypeIndex targetIndex;

	if (it == arch.add.end())
	{

		targetIndex = addArchType(newArchBitMask);
		ArchType& newArch = getArchType(targetIndex);

		printf("add arch type from %zu -> %zu\n", handle.data.archType, targetIndex);

		// iterate siblings
		for (const auto& index : getArchType(handle.data.archType).add)
		{
			ArchType& a = getArchType(index);
			size_t checkBitMask = a.bitMask | newArchBitMask;
			auto it = std::find_if(a.add.begin(), a.add.end(), [&](const ArchTypeIndex& i) { return getArchType(i).bitMask == checkBitMask; });
			if (it != a.add.end()) // combo found!
			{
				const ArchTypeIndex i = *it;
				getArchType(i).remove.emplace_back(targetIndex);
				newArch.add.emplace_back(i); // bind it
			}
		}

		getArchType(handle.data.archType).add.emplace_back(targetIndex);
		newArch.remove.emplace_back(handle.data.archType);
	}
	else
	{
		targetIndex = *it;
	}

	ArchType& newArchType = getArchType(targetIndex);
	handle.data.archType = targetIndex;
	handle.data.entity = newArchType.alloc();
}

ArchType& Scene::getArchType(size_t index)
{
	return archTypes_[index];
}

ArchType& Scene::getRootArchType()
{
	return archTypes_[0];
}
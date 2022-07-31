#include "Scene.hpp"
#include "Engine.hpp"

Scene::Scene(Engine& engine, std::string name, std::string path) :
	engine_(engine),
	name_(name),
	path_(path),
	entityHandles_(),
	archHandles_(),
	archLevels_(),
	rootArch_(nullptr)
{

}


void Scene::load(bool parse)
{
	engine_.logger.info("load scene ", name_);

	if (!isLoaded())
		return;

	rootArch_ = std::addressof(addArch(0, 0, 0, 0));

	if (parse)
	{
		Bin::Reader reader(path_);

		reader.read([&](Bin::Parser& parser)
		{

		});
	}
}

void Scene::unload()
{
	engine_.logger.info("unload scene ", name_);
	if (!isLoaded())
		return;

	entityHandles_.clear();
	rootArch_ = nullptr;
}

bool Scene::isLoaded()
{
	return rootArch_ != nullptr;
}

const char* Scene::name()
{
	return name_.c_str();
}

const char* Scene::path()
{
	return path_.c_str();
}

Handle<Arch>& Scene::addArch(const size_t componentIndex, const size_t bitMask, const size_t size, const size_t level)
{
	engine_.logger.info("Create arch type ", bitMask, " with size ", size, " at level ", level);

	Handle<Arch>& arch = archHandles_.alloc();
	Handle<Arch>* archPtr = std::addressof(arch);

	arch.data.bitMask = bitMask;
	arch.data.size = size;
	arch.data.allocator.reset(size);

	for (size_t i = archLevels_.size(), l = level + 1; i < l; i++)
		archLevels_.emplace_back();

	// add the arch level 
	archLevels_[level].emplace_back(std::addressof(arch));

	if (bitMask == 0)
	{
		arch.data.offsets.emplace_back(0);
	}
	else if (level != 0)
	{
		// set offsets
		size_t bitMaskTest = bitMask;
		size_t componentCount = 0;
		size_t offsetCounter = 0;

		arch.data.offsets.emplace_back(0);

		for (size_t i = 0; i < 64; i++)
		{
			if (bitMaskTest & 1)
			{
				auto c = engine_.getComponent(i);
				componentCount++;
				offsetCounter += c.getSize();
				arch.data.offsets.emplace_back(offsetCounter);
				if (componentCount >= level)
					break;
			}

			bitMaskTest >>= 1;
		}
	}

	engine_.logger.info("arch offsets: ", arch.data.offsets.size());

	const size_t componentBitMask = 1ULL << componentIndex;

	if (level > 1)
	{
		auto& v = archLevels_[level - 1];
		for (auto& removeArch : v) // set remove arms
		{
			if ((bitMask & removeArch->data.bitMask) == removeArch->data.bitMask)
			{
				// printf("got remove arm: bitMask %zu\n", removeArch->data.bitMask);
				size_t offset = removeArch->data.getOffsetIndex(bitMask);
				removeArch->data.add.emplace_back(archPtr, offset);
				arch.data.remove.emplace_back(removeArch, offset);
			}
		}
	}
	else if (level == 1)
	{
		// printf("add root arch refs\n");
		rootArch_->data.add.emplace_back(archPtr, 0);
		arch.data.remove.emplace_back(rootArch_, 0);
	}

	if (level + 1 < archLevels_.size())
	{
		auto& v = archLevels_[level + 1];

		for (auto& addArch : v) // set add arms
		{
			if ((addArch->data.bitMask & bitMask) == bitMask)
			{
				// printf("got add arm: bitMask %zu\n", addArch->data.bitMask);
				size_t offset = arch.data.getOffsetIndex(addArch->data.bitMask);
				addArch->data.remove.emplace_back(archPtr, offset);
				arch.data.add.emplace_back(addArch, offset);
			}
		}
	}

	return arch;
}

Handle<Entity>& Scene::addEntity(std::string& name)
{
	engine_.logger.info("Added entity with name ", name);
	Handle<Entity>& handle = entityHandles_.alloc();
	handle.data.entity = nullptr;
	handle.data.arch = rootArch_;
	return handle;
}

Handle<Entity>& Scene::addEntity(const char* name)
{
	std::string entityName(name);
	return addEntity(entityName);
}

void* Scene::addComponentToEntity(Handle<Entity>& handle, const ComponentIndex& componentIndex, void* data)
{
	const ComponentInfo& component = engine_.getComponent(componentIndex);
	return addComponentToEntity(handle, component, data);
}

void* Scene::addComponentToEntity(Handle<Entity>& handle, const ComponentInfo& component, void* data)
{
	size_t componentIndex = component.getIndex();

	if (handle.data.arch == nullptr)
		throw std::runtime_error("Arch = nullptr!");

	Arch& from = handle.data.arch->data;

	const size_t componentBitMask = component.getBitMask();
	const size_t newBitMask = componentBitMask | from.bitMask;

	engine_.logger.info("Add component (bitmask: ", component.getBitMask(), ") to arch (bitmask: ", from.bitMask, ") (add-arm size: ", from.add.size(), ")");

	int armIndex = -1;
	int armCounter = 0;
	for (auto& a : from.add)
	{
		Handle<Arch>* handle = a.arch;
		if (handle->data.bitMask == newBitMask)
		{
			armIndex = armCounter;
			break;
		}
		armCounter++;
	}

	size_t offsetIndex = 0;
	Handle<Arch>* newArch = nullptr;

	if (armIndex == -1)
	{
		Handle<Arch>& a = addArch(componentIndex, newBitMask, from.size + component.getSize(), from.getComponentCount() + 1);
		offsetIndex = from.getOffsetIndex(newBitMask);
		newArch = std::addressof(a);
	}
	else
	{
		auto& archArm = from.add.at(armIndex);
		newArch = archArm.arch;
		offsetIndex = archArm.offsetIndex;
	}

	// update the new arch type
	engine_.logger.info("updated entity arch type archIndex: ", newArch->index.index);
	handle.data.arch = newArch;

	const size_t componentOffset = from.offsets[offsetIndex];
	const size_t componentSize = component.getSize();
	const size_t lastOffset = from.offsets[from.offsets.size() - 1];

	char* oldPtr = handle.data.entity;

	handle.data.index = newArch->data.allocator.alloc();
	handle.data.entity = newArch->data.allocator.at(handle.data.index);

	if (from.bitMask != 0)
	{
		if (componentOffset == 0)
		{
			printf("copy as start\n");
			if (data != nullptr)
				memcpy(handle.data.entity, data, componentSize);
			memcpy(handle.data.entity + componentSize, oldPtr, from.size);
			return handle.data.entity;
		}
		else if (componentOffset == lastOffset)
		{
			printf("copy as last\n");
			memcpy(handle.data.entity, oldPtr, from.size);
			if (data != nullptr)
				memcpy(handle.data.entity + from.size, data, componentSize);
			return handle.data.entity + from.size;
		}
		else
		{
			printf("copy center %zu\n", componentOffset);
			memcpy(handle.data.entity, oldPtr, componentOffset);
			if (data != nullptr)
				memcpy(handle.data.entity + componentOffset, data, componentSize);
			memcpy(handle.data.entity + componentSize + componentOffset, oldPtr + componentOffset, from.size - componentOffset);
			return handle.data.entity + componentOffset;
		}
	}

	return nullptr;
}

Handle<Arch>& Scene::getArch(HandleIndex index)
{
	return archHandles_.at(index);
}

Handle<Arch>* Scene::getArchPtr(HandleIndex index)
{
	return std::addressof(archHandles_.at(index));
}

size_t Scene::filterArchTypes(size_t bitMask, ArchGroup& archTypes)
{
	size_t count = 0;
	archHandles_.iterate([&](Handle<Arch>& arch, size_t i)
	{
		if ((arch->bitMask & bitMask) == bitMask)
		{
			archTypes.emplace_back(&arch.data);
			count++;
		}
	});
	return count;
}
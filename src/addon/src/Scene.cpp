#include "Scene.hpp"
#include "Engine.hpp"

Scene::Scene(Engine& engine, std::string name, std::string path) :
	engine_(engine),
	name_(name),
	path_(path),
	archTypes_(),
	entityHandles_()
{
	std::vector<uint16_t> offsets;
	archTypes_.emplace_back(0, 0, 0, offsets);
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

// ArchTypeIndex Scene::addArchType(size_t bitMask)
// {
	// ArchTypeIndex index = archTypes_.size();
	// size_t bitMaskTest = bitMask;
	// std::vector<uint16_t> componentLayouts;
	// size_t size = 0;

	// for (size_t i = 0; i < 64; i++)
	// {
	// 	if (bitMaskTest & 1)
	// 	{
	// 		const Component& component = engine_.getComponent(i);
	// 		componentLayouts.emplace_back(size);
	// 		size += component.getSize();
	// 	}
	// 	bitMaskTest >>= 1;
	// }

	// archTypes_.emplace_back(index, bitMask, size, componentLayouts);

	// return index;
// }

Handle<Entity>& Scene::addEntity(std::string name)
{
	Handle<Entity>& handle = entityHandles_.alloc();
	handle.data.entity = nullptr;
	handle.data.archType = 0;
	return handle;
}

void Scene::addComponentToEntity(Handle<Entity>& handle, size_t componentIndex)
{
	const ArchTypeIndex oldArchIndex = handle.data.archType;
	ArchType& arch = getArchType(handle.data.archType);

	const Component component = engine_.getComponent(componentIndex);
	const size_t componentBitMask = component.getBitMask();
	const size_t componentSize = component.getSize();
	const size_t newArchBitMask = arch.bitMask | componentBitMask;

	auto it = std::find_if(arch.add.begin(), arch.add.end(), [&](ArchTypeArm& arm) { return getArchType(arm.index).bitMask == newArchBitMask; });

	size_t newArchIndex;
	int16_t splitIndex = 0;

	if (it == arch.add.end()) // arch type does not exists yet!
	{
		// printf("add new arch type from %zu to %zu\n", arch.bitMask, newArchBitMask);

		if (arch.componentOffsets.size() == 0) // from rootArch nothing to copy :D
		{
			newArchIndex = archTypes_.size();
			std::vector<uint16_t> offsets;
			offsets.emplace_back(0);
			arch.add.emplace_back(newArchIndex, 0);
			archTypes_.emplace_back(newArchIndex, newArchBitMask, componentSize, offsets);
		}
		else
		{
			splitIndex = arch.getSplitIndex(newArchBitMask, componentIndex);

			// create offsets for the new arch
			std::vector<uint16_t> offsets;

			for (size_t i = 0, l = arch.componentOffsets.size(); i < l; i++)
			{
				// printf("add offset %zu\n", i);
				if (i == splitIndex)
					offsets.emplace_back(arch.componentOffsets[i]);

				if (i >= splitIndex)
					offsets.emplace_back(componentSize + arch.componentOffsets[i]);
				else
					offsets.emplace_back(arch.componentOffsets[i]);
			}

			newArchIndex = archTypes_.size();
			arch.add.emplace_back(newArchIndex, splitIndex);
			archTypes_.emplace_back(newArchIndex, newArchBitMask, componentSize, offsets);
			
	
		}
		// check sibling combos
		ArchType& newArch = getArchType(newArchIndex);
		newArch.remove.emplace_back(oldArchIndex, splitIndex);
	}
	else
	{
		splitIndex = (*it).offset;
		newArchIndex = (*it).index;
		// printf("found arch %zu and split index %zu\n", newArchIndex, splitIndex);
	}

	ArchType& oldArch = getArchType(oldArchIndex);
	ArchType& newArch = getArchType(newArchIndex);


	handle.data.archType = newArchIndex;
	handle.data.entity = newArch.alloc();

	if (oldArch.componentOffsets.size() != 0)
	{
		size_t splitOffset = oldArch.componentOffsets[splitIndex];
		// printf("copy data %zu - %zu\n", splitOffset, componentSize);
		// printf("copy data %zu - %zu\n", componentSize, splitOffset + componentSize);
		// printf("copy data %zu - %zu\n", splitOffset + componentSize, newArch.size);
		
	}
}

ArchType& Scene::getArchType(size_t index)
{
	return archTypes_[index];
}

ArchType& Scene::getRootArchType()
{
	return archTypes_[0];
}
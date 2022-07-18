#include "Component.hpp"

Component::Component(size_t bitMask, size_t size) : bitMask_(bitMask), size_(size) 
{
	
}

const size_t Component::getSize() const
{ 
	return size_;
}

const size_t Component::getBitMask() const
{ 
	return bitMask_;
}
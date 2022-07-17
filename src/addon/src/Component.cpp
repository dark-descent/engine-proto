#include "Component.hpp"

Component::Component(size_t bitMask, size_t size) : bitMask_(bitMask), size_(size) 
{
	
}

const size_t Component::getSize()
{ 
	return size_;
}

const size_t Component::getBitMask()
{ 
	return bitMask_;
}
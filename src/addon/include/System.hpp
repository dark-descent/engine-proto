#pragma once

#include "framework.hpp"
#include "Arch.hpp"

using ArchGroup = std::vector<Arch*>;

class Engine;

struct Query
{
	size_t required;
	size_t optional;

	Query() : required(0), optional(0) { }
};

class System
{
protected:
	Query query_;

public:
	virtual void run(ArchGroup& archTypes, const size_t count) = 0;
	virtual void initialize(Engine& engine) = 0;

	const Query& query() const { return query_; }
};
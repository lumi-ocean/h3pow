#pragma once

#include "core.h"

struct Package
{
	std::string name;
	std::filesystem::path dir;

	explicit Package(std::string const& name, std::filesystem::path const& dir) : name(name), dir(dir) {}
};

extern std::vector<Package> all_packages;

void InitializePackages();

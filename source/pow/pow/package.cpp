#include "pch.h"
#include "package.h"

std::vector<Package> all_packages;

void InitializePackages()
{
	for (auto const& entry : std::filesystem::directory_iterator(exe_dir / "packages"))
	{
		if (entry.is_directory())
		{
			all_packages.push_back(Package(entry.path().filename().string(), entry.path()));
		}
	}
	all_packages.push_back(Package("local", exe_dir));
}

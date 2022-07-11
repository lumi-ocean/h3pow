#pragma once

#include "framework.h"

enum class ExeType
{
	standalone, main, maped, camped
};

enum class ExeVersion
{
	sod, hota, wog, era, pow
};

extern ExeType exe_type;
extern ExeVersion exe_version;
extern std::string exe_version_str;

extern std::filesystem::path exe_dir;
extern std::filesystem::path rules_dir;

#include "pch.h"
#include "patcher.h"

Patcher* patchers = nullptr;
PatcherInstance* patcher = nullptr;

std::map<s32, MyLoHookFunc> _written_lohookfuncs;

std::map<std::string, char const*> const_chars;
std::mutex const_chars_mutex;

void InitializePatcher()
{
	if (patchers == nullptr)
	{
		patchers = GetPatcher();
	}
	if (patcher == nullptr)
	{
		patcher = patchers->CreateInstance("pow");
	}
}

char const* const_char(std::string const& str)
{
	std::lock_guard lock(const_chars_mutex);

	auto search = const_chars.find(str);
	if (search != const_chars.end()) return search->second;

	char* ch = new char[str.size() + 1];
	strcpy_s(ch, str.size() + 1, str.c_str());
	const_chars[str] = ch;
	return ch;
}

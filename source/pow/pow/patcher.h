#pragma once

#include "core.h"

#include <patcher_x86/patcher_x86.hpp>

#define ARRAY_AT_ADDRESS(Type, address) (reinterpret_cast<Type*>(address))
#define ARRAY_AT_PTR(Type, ptr_address) (*reinterpret_cast<Type**>(ptr_address))

#define LOHOOK(address) Void _hook__##address = AddHook(address, &_func__##address); \
	static void _func__##address(bool& exec, SmartValue eax, SmartValue ebx, SmartValue ecx, SmartValue edx, SmartValue esi, SmartValue edi, HookContext* context)

extern Patcher* patchers;
extern PatcherInstance* patcher;

void InitializePatcher();

char const* const_char(std::string const& str);

#ifdef FLIB_H
inline char const* const_char(fstring const& str) { return const_char(str.std_str()); }
#endif

struct Void
{
};

struct SmartValue
{
private:
	int* m_original;

public:
	explicit SmartValue(int& original) : m_original(&original) {}

	SmartValue& operator =(int value) { *m_original = value; return *this; }
	SmartValue& operator =(std::string const& value) { *m_original = reinterpret_cast<s32>(const_char(value)); return *this; }

	operator int() { return *m_original; }
	operator std::string() { return reinterpret_cast<char const*>(*m_original); }

#ifdef FLIB_H
	SmartValue& operator =(fstring const& value) { *m_original = reinterpret_cast<s32>(const_char(value)); return *this; }
	operator fstring() { return reinterpret_cast<char const*>(*m_original); }
#endif
};

typedef void (*MyLoHookFunc)(bool& exec, SmartValue eax, SmartValue ebx, SmartValue ecx, SmartValue edx, SmartValue esi, SmartValue edi, HookContext* context);

extern std::map<s32, MyLoHookFunc> _written_lohookfuncs;

struct Hooks
{
	virtual ~Hooks() {}
	virtual void Write()
	{
		for (auto& [address, func] : _lohooks)
		{
			patcher->WriteLoHook(address, func);
			_written_lohookfuncs[address] = _lohookfuncs[address];
		}
	}

	std::map<s32, _LoHookFunc_> _lohooks;
	std::map<s32, MyLoHookFunc> _lohookfuncs;

	Void AddHook(s32 address, MyLoHookFunc func)
	{
		_lohookfuncs[address] = func;
		_lohooks[address] = [](LoHook* hook, HookContext* context) {
			SmartValue eax(context->eax); SmartValue ebx(context->ebx);
			SmartValue ecx(context->ecx); SmartValue edx(context->edx);
			SmartValue esi(context->esi); SmartValue edi(context->edi);
			bool exec = true;
			_written_lohookfuncs[hook->GetAddress()](exec, eax, ebx, ecx, edx, esi, edi, context);
			return exec ? EXEC_DEFAULT : SKIP_DEFAULT;
		};
		return Void();
	}
};

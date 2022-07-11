#include "pch.h"

#include "patcher.h"
#include "package.h"
#include "asset.h"
#include "gamehooks.h"

ExeType exe_type = ExeType::standalone;
ExeVersion exe_version = ExeVersion::pow;
std::string exe_version_str = "pow";
std::filesystem::path exe_dir;
std::filesystem::path rules_dir;
std::string exe_filename;

struct GameInitHooks : public Hooks
{
	LOHOOK(0x4F81A8) { InitializeGamehooks(); }
	LOHOOK(0x4EEEAC) { DumpGameRuleset(); }
};

std::string GetExeFullPath()
{
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	return std::string(buffer);
}

/*
LONG WINAPI TopLevelExceptionFilter(_In_ struct _EXCEPTION_POINTERS* ExceptionInfo)
{
	MessageBoxA(0, "Exception", "", 0);

	return EXCEPTION_EXECUTE_HANDLER;
}
*/

BOOL __stdcall Hook_ShowWindow(HiHook* hook, HWND hWnd, int nCmdShow)
{
	if (nCmdShow == 0)
	{
		if (true)
		{
			std::ofstream file_out("crash.txt");
			file_out << "ShowWindow(" << hWnd << ", " << nCmdShow << ");\n\n";
		}
		Sleep(6000);
		exit(1);
		return 0;
	}
	else
	{
		return reinterpret_cast<BOOL (__stdcall *)(HWND hWnd, int nCmdShow)>(hook->GetOriginalFunc())(hWnd, nCmdShow);
	}
}

int __stdcall Hook_MessageBoxA(HiHook* hook, HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
	if (true)
	{
		std::ofstream file_out("crash.txt");
		file_out << "MessageBoxA(" << hWnd << ", " << lpText << ", " << lpCaption << ", " << uType << ");\n\n";
	}
	Sleep(6000);
	exit(1);
	return 0;
}

extern "C" __declspec(dllexport) void Initialize(char const* exe_type_ch, char const* exe_version_ch)
{
	SetProcessDPIAware();

	if (!SetProcessDEPPolicy(0))
	{
		MessageBoxA(0, "Failed to turn DEP off", "Warning", 0);
	}

	//SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
	//SetUnhandledExceptionFilter(TopLevelExceptionFilter);

	std::string exe_type_str = exe_type_ch;
	if (exe_type_str == "standalone") exe_type = ExeType::standalone;
	else if (exe_type_str == "game") exe_type = ExeType::main;
	else if (exe_type_str == "maped") exe_type = ExeType::maped;
	else if (exe_type_str == "camped") exe_type = ExeType::camped;
	else return;

	exe_version_str = exe_version_ch;
	if (exe_version_str == "sod") exe_version = ExeVersion::sod;
	else if (exe_version_str == "hota") exe_version = ExeVersion::hota;
	else if (exe_version_str == "wog") exe_version = ExeVersion::wog;
	else if (exe_version_str == "era") exe_version = ExeVersion::era;
	else if (exe_version_str == "pow") exe_version = ExeVersion::pow;
	else return;

	std::filesystem::path exe_full_path = GetExeFullPath();
	exe_dir = exe_full_path.parent_path();
	exe_filename = exe_full_path.filename().string();

	rules_dir = exe_dir / "rules";

	if (exe_version != ExeVersion::era)
	{
		auto vfs = LoadLibraryA("vfs.dll");
		if (vfs == NULL)
		{
			MessageBoxA(0, "Error: vfs.dll is missing", "Error", 0);
			return;
		}

		using MapModsFromListF = bool __stdcall (char const* GameDir, char const* ModsDir, char const* ModListFilePath, int Flags);
		auto MapModsFromListA = reinterpret_cast<MapModsFromListF*>(GetProcAddress(vfs, "MapModsFromListA"));
		MapModsFromListA(exe_dir.string().c_str(),
			(exe_dir / "mods").string().c_str(),
			(exe_dir / "mods" / (exe_version_str + ".txt")).string().c_str(), 0);

		using RunVfsF = bool __stdcall (int SortType);
		auto RunVfs = reinterpret_cast<RunVfsF*>(GetProcAddress(vfs, "RunVfs"));
		RunVfs(0);

		using RunWatcherF = bool __stdcall (char const* ModsDir, int Interval);
		auto RunWatcherA = reinterpret_cast<RunWatcherF*>(GetProcAddress(vfs, "RunWatcherA"));
		RunWatcherA((exe_dir / "mods").string().c_str(), 250);

		//using GetMappingsReportF = char const* __stdcall ();
		//auto GetMappingsReportA = reinterpret_cast<GetMappingsReportF*>(GetProcAddress(vfs, "GetMappingsReportA"));
		//MessageBoxA(0, GetMappingsReportA(), "", 0);
	}

	InitializePatcher();
	InitializePackages();
	InitializeAssets();

	if (exe_type == ExeType::main)
	{
		GameInitHooks inithooks;
		inithooks.Write();

		auto kernel32 = GetModuleHandleA("kernel32.dll");
		auto user32 = GetModuleHandleA("user32.dll");

		if (kernel32 == nullptr || user32 == nullptr) return;

		patcher->WriteHiHook(_ptr_(GetProcAddress(user32, "ShowWindow")), SPLICE_, EXTENDED_, STDCALL_, Hook_ShowWindow);
		patcher->WriteHiHook(_ptr_(GetProcAddress(user32, "MessageBoxA")), SPLICE_, EXTENDED_, STDCALL_, Hook_MessageBoxA);
	}
}

extern "C" __declspec(dllexport) void StartLauncher()
{
	Initialize("standalone", "pow");

	auto hd_launchernative = LoadLibraryA("hd_launchernative.dll");
	if (hd_launchernative != NULL)
	{
		using StartF = void __stdcall ();
		auto Start = reinterpret_cast<StartF*>(GetProcAddress(hd_launchernative, "Start"));
		Start();
	}
	else
	{
		MessageBoxA(0, "Error: hd_launchernative.dll is missing", "Error", 0);
	}
}

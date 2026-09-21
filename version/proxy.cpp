#include "pch.h"
#include "proxy.h"

#pragma comment(linker,"/export:GetFileVersionInfoA=C:\\Windows\\SysWOW64\\version.GetFileVersionInfoA,@1")
#pragma comment(linker,"/export:GetFileVersionInfoByHandle=C:\\Windows\\SysWOW64\\version.GetFileVersionInfoByHandle,@2")
#pragma comment(linker,"/export:GetFileVersionInfoExA=C:\\Windows\\SysWOW64\\version.GetFileVersionInfoExA,@3")
#pragma comment(linker,"/export:GetFileVersionInfoExW=C:\\Windows\\SysWOW64\\version.GetFileVersionInfoExW,@4")
#pragma comment(linker,"/export:GetFileVersionInfoSizeA=C:\\Windows\\SysWOW64\\version.GetFileVersionInfoSizeA,@5")
#pragma comment(linker,"/export:GetFileVersionInfoSizeExA=C:\\Windows\\SysWOW64\\version.GetFileVersionInfoSizeExA,@6")
#pragma comment(linker,"/export:GetFileVersionInfoSizeExW=C:\\Windows\\SysWOW64\\version.GetFileVersionInfoSizeExW,@7")
#pragma comment(linker,"/export:GetFileVersionInfoSizeW=C:\\Windows\\SysWOW64\\version.GetFileVersionInfoSizeW,@8")
#pragma comment(linker,"/export:GetFileVersionInfoW=C:\\Windows\\SysWOW64\\version.GetFileVersionInfoW,@9")
#pragma comment(linker,"/export:VerFindFileA=C:\\Windows\\SysWOW64\\version.VerFindFileA,@10")
#pragma comment(linker,"/export:VerFindFileW=C:\\Windows\\SysWOW64\\version.VerFindFileW,@11")
#pragma comment(linker,"/export:VerInstallFileA=C:\\Windows\\SysWOW64\\version.VerInstallFileA,@12")
#pragma comment(linker,"/export:VerInstallFileW=C:\\Windows\\SysWOW64\\version.VerInstallFileW,@13")
#pragma comment(linker,"/export:VerLanguageNameA=C:\\Windows\\SysWOW64\\version.VerLanguageNameA,@14")
#pragma comment(linker,"/export:VerLanguageNameW=C:\\Windows\\SysWOW64\\version.VerLanguageNameW,@15")
#pragma comment(linker,"/export:VerQueryValueA=C:\\Windows\\SysWOW64\\version.VerQueryValueA,@16")
#pragma comment(linker,"/export:VerQueryValueW=C:\\Windows\\SysWOW64\\version.VerQueryValueW,@17")

namespace fs = std::filesystem;

namespace version::proxy {

	HMODULE version = nullptr;

	static std::vector<HMODULE> g_plugins;
	static std::wofstream g_logFile;

	static void InitLogFile(const fs::path& logPath) {
		g_logFile.open(logPath, std::ios::out | std::ios::trunc);
	}

	static std::wstring GetTimestamp() {
		std::time_t t = std::time(nullptr);
		std::tm tm{};

		localtime_s(&tm, &t);
		std::wostringstream timeSnapShot;

		timeSnapShot << std::put_time(&tm, L"[%Y-%m-%d %H:%M:%S]");

		return timeSnapShot.str();
	}

	static void Log(const std::wstring& msg) {
		std::wstring line = GetTimestamp() + L" " + msg;

		if (g_logFile.is_open()) {
			g_logFile << line << std::endl;
		}
	}

	static void LoadPlugins() {
		wchar_t exePath[MAX_PATH];

		GetModuleFileName(NULL, exePath, MAX_PATH);

		fs::path baseDir = fs::path(exePath).parent_path();
		fs::path pluginDir = baseDir / L"BioPlugins";
		fs::path logDir = baseDir / L"BioPlugins" / L"Plugins.log";

		if (!fs::exists(pluginDir))
			fs::create_directory(pluginDir);

		InitLogFile(logDir);

		Log(L"---------------------------------------");
		Log(L"BioPlugins loader initialized.");
		Log(L"Plugin directory: " + pluginDir.wstring());

		int pluginCount = 0;

		for (auto& entry : fs::directory_iterator(pluginDir)) {
			if (entry.path().extension() != L".dll")
				continue;

			std::wstring pluginName = entry.path().filename().wstring();

			HMODULE hMod = LoadLibraryW(entry.path().c_str());

			if (hMod) {
				g_plugins.push_back(hMod);
				Log(L"Loaded plugin: " + pluginName);
			}
			else {
				DWORD err = GetLastError();
				std::wostringstream ss;
				ss << L"  >> FAILED to load: " << pluginName << L" (Error: " << err << L")";
				Log(ss.str());
			}

			pluginCount++;
		}

		if (pluginCount == 0) {
			Log(L"No plugins found within the BioPlugins folder.");
		}
		else {
			std::wostringstream ss;
			ss << L"Done. " << g_plugins.size() << L"/" << pluginCount << L" plugin(s) loaded successfully.";
			Log(ss.str());
		}

		Log(L"---------------------------------------");
	}

	static void UnloadPlugins() {
		for (HMODULE hMod : g_plugins) {
			FreeLibrary(hMod);
		}
		g_plugins.clear();
	}

	void init(HMODULE) {

		char sysPath[MAX_PATH];

		UINT wowPath = GetSystemWow64DirectoryA(sysPath, MAX_PATH);

		if (wowPath && wowPath < MAX_PATH) {
			strcat_s(sysPath, "\\version.dll");
			version = LoadLibraryA(sysPath);
		}
		else {
			return;
		}

		if (!version) {
			return;
		}

		LoadPlugins();
	}

	void free() {
		UnloadPlugins();
		if (version != nullptr) {
			FreeLibrary(version);
			version = nullptr;
		}
	}
}
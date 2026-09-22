#include "pch.h"
#include "rebornconfig.h"

namespace adamreborn::rebornconfig {

    int ConsoleKey = 9; // Tab

    std::filesystem::path GetDLLPath() {
        char path[MAX_PATH] = {};
        HMODULE dll = NULL;

        static int dummy = 0;

        if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR)&dummy, &dll)) {
            GetModuleFileNameA(dll, path, sizeof(path));
        }

        return std::filesystem::path(path).parent_path();
    }

    static void CreateRebornConfig() {
        std::filesystem::path configPath = GetDLLPath() / "AdamReborn.ini";

        if (!std::filesystem::exists(configPath)) {
            std::ofstream configFile(configPath);
            if (configFile.is_open()) {
                configFile << "##############################################################################################\n";
                configFile << "# AdamReborn made by HOKTempest\n";
                configFile << "# Set the keybind id required to open the console in-game. Game Default: 9 | AKA Tab\n";
                configFile << "# Accepts key id values ranging from 0 - 254 (Windows virtual-key codes). Some common ones:\n";
                configFile << "# \n";
                configFile << "# Keybind    | Key ID\n";
                configFile << "# ----------------------\n";
                configFile << "# Tab        | 9\n";
                configFile << "# Tilde (`~) | 192\n";
                configFile << "# Backslash  | 220\n";
                configFile << "# Insert     | 45\n";
                configFile << "# Delete     | 46\n";
                configFile << "# Pause      | 19\n";
                configFile << "# Home       | 36\n";
                configFile << "# End        | 35\n";
                configFile << "# PageUp     | 33\n";
                configFile << "# PageDown   | 34\n";
                configFile << "# F1 - F12   | 112 - 123 (F1 = 112, F2 = 113, ... F12 = 123)\n";
                configFile << "# 0 - 9      | 48 - 57\n";
                configFile << "# A - Z      | 65 - 90\n";
                configFile << "# ----------------------\n";
                configFile << "# \n";
                configFile << "# Full list: https://learn.microsoft.com/windows/win32/inputdev/virtual-key-codes\n";
                configFile << "# \n";
                configFile << "# WARNING: 0 means \"no key\", so the console will never open. \n";
                configFile << "# Anything that is not a number (like \"Tilde\" the word itself) also counts as 0. 1 - 6 are mouse buttons.\n";
                configFile << "# Values above 254 are clamped to 254. Avoid keys the game already uses under the [Default] input context.\n";
                configFile << "##############################################################################################\n\n";
                configFile << "[RebornConsoleKey]\n";
                configFile << "ConsoleKey=9\n";
                configFile.close();
            }
        }
    }

    void LoadRebornConfig() {
        CreateRebornConfig();
        std::filesystem::path rebornConfig = GetDLLPath() / "AdamReborn.ini";
        int configConsoleKey = GetPrivateProfileIntA("RebornConsoleKey", "ConsoleKey", static_cast<int>(ConsoleKey), rebornConfig.string().c_str());

        ConsoleKey = std::clamp((int)(configConsoleKey), 0, 254);
    }
}

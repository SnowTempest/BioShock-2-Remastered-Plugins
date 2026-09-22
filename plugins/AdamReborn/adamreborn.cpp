#include "pch.h"
#include "adamreborn.h"
#include "globals.h"
#include "rebornconfig.h"

namespace adamreborn::ar {

	static void* KeyEvent = nullptr;
	static void* Type = nullptr;
	static void* EventKey = nullptr;
	static void* EventAction = nullptr;

	static int ByteScriptOffset = 0x70;
	static int NameOffset = 0x28;

	void SetupKeyEvent() {
		KeyEvent = globals::StaticFindObject(nullptr, nullptr, L"Engine.Console.KeyEvent", false);

		if (!KeyEvent) {
			return;
		}

		Type = globals::StaticFindObject(nullptr, nullptr, L"Engine.Console.Type", false);

		if (!Type) {
			return;
		}

		EventKey = globals::StaticFindObject(nullptr, nullptr, L"Engine.Console.KeyEvent.Key", false);

		if (!EventKey) {
			return;
		}

		EventAction = globals::StaticFindObject(nullptr, nullptr, L"Engine.Console.KeyEvent.Action", false);

		if (!EventAction) {
			return;
		}
	}

	bool PatchConsoleKey() {
		if (!KeyEvent || !Type || !EventKey || !EventAction) {
			return false;
		}

		auto* script = reinterpret_cast<globals::TArray<uint8_t>*>(static_cast<uint8_t*>(KeyEvent) + ByteScriptOffset);

		if (script->Num != 4 || memcmp(script->Data, "\x04\x28\x04\x0B", 4) != 0) {
			return false;
		}

		uint8_t patch[] = {
			0x07, 0x34, 0x00,
			0x9A, 0x39, 0x3A, 0x00, 0, 0, 0, 0, 0, 0, 0, 0, 0x39, 0x3A, 0x24, 0x01, 0x16,
			0x07, 0x34, 0x00,
			0x9A, 0x39, 0x3A, 0x00, 0, 0, 0, 0, 0, 0, 0, 0, 0x39, 0x3A, 0x24, 0x00, 0x16,
			0x1B, 0, 0, 0, 0, 0, 0, 0, 0, 0x16,
			0x04, 0x27,
			0x04, 0x28, 0x04, 0x0B,
		};

		memcpy(&patch[7], &EventAction, sizeof(void*));
		memcpy(&patch[27], &EventKey, sizeof(void*));
		patch[38] = static_cast<uint8_t>(rebornconfig::ConsoleKey);
		memcpy(&patch[41], static_cast<uint8_t*>(Type) + NameOffset, 8);

		const int size = sizeof(patch);
		globals::FArrayAdd(script, size - script->Num, 1);
		if (!script->Data || script->Num != size) {
			return false;
		}

		memcpy(script->Data + 4, patch + 4, size - 4);
		InterlockedExchange(reinterpret_cast<volatile LONG*>(script->Data), *reinterpret_cast<LONG*>(patch));

		return true;
	}

	void InitAdamReborn() {
		if (!globals::InitializeGlobals()) {
			return;
		}

		while (!globals::IsEngineReady()) {
			Sleep(500);
		}

		SetupKeyEvent();
		PatchConsoleKey();
	}
}

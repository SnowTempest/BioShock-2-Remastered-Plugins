#pragma once

#ifndef GLOBALS_H
#define GLOBALS_H

#include <cstdint>

namespace adamreborn::globals {
    inline constexpr uint32_t ExpectedTimeDateStamp = 0x6256F3A5;

    namespace rva {
        inline constexpr uintptr_t StaticFindObject = 0x0B7D4C0;
        inline constexpr uintptr_t GObjects = 0x1A57400;
        inline constexpr uintptr_t GEngine = 0x1A638F0;
        inline constexpr uintptr_t FArrayAdd = 0x2DDE40;
    }

    template <typename T>
    struct TArray {
        T* Data;
        int32_t Num;
        int32_t Max;
    };

    typedef void* (__cdecl* tStaticFindObject)(void* Class, void* Package, const wchar_t* Name, bool ExactClass);
    extern tStaticFindObject StaticFindObject;

    typedef int (__thiscall* tFArrayAdd)(void* array, int count, int elementSize);
    extern tFArrayAdd FArrayAdd;

    extern TArray<void*>* GObjects;
    extern void** GEngine;

    bool InitializeGlobals();
    bool IsEngineReady();
}

#endif
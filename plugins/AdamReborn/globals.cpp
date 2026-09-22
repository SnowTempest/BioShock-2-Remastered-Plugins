#include "pch.h"
#include "globals.h"

namespace adamreborn::globals {

    tStaticFindObject StaticFindObject = nullptr;
    tFArrayAdd FArrayAdd = nullptr;
    TArray<void*>* GObjects = nullptr;
    void** GEngine = nullptr;

    namespace {
        bool IsExpectedBuild(uintptr_t base) {
            auto* dosHeader = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
            if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
                return false;
            }

            auto* ntHeaders = reinterpret_cast<const IMAGE_NT_HEADERS32*>(base + dosHeader->e_lfanew);
            if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) {
                return false;
            }

            return ntHeaders->FileHeader.TimeDateStamp == ExpectedTimeDateStamp;
        }
    }

    bool InitializeGlobals() {
        const auto base = reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr));
        if (!base || !IsExpectedBuild(base)) {
            return false;
        }

        StaticFindObject = reinterpret_cast<tStaticFindObject>(base + rva::StaticFindObject);
        FArrayAdd = reinterpret_cast<tFArrayAdd>(base + rva::FArrayAdd);
        GObjects = reinterpret_cast<TArray<void*>*>(base + rva::GObjects);
        GEngine = reinterpret_cast<void**>(base + rva::GEngine);

        return true;
    }

    bool IsEngineReady() {
        return GEngine && *GEngine && GObjects && GObjects->Data && GObjects->Num > 0;
    }
}

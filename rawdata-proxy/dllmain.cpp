#include <stdint.h>
#define WIN32_LEAN_AND_MEAN
#include "FlashRuntimeExtensions.h"
#include <fstream>
#include <iostream>
#include <string>
#include <windows.h>

#define DLL_EXPORT extern "C" __declspec(dllexport)

enum class SwzType {
    Unknown,
    Dynamic,
    Engine,
    Game,
    Init,
};

static SwzType activeSwz = SwzType::Unknown;
static bool shouldLoadCustomMaps = false;

// pointers to original functions from RawData.dll
FREInitializer ExtInitializer_orig; // calls contextInitializer
FREFinalizer ExtFinalizer_orig; // extension finalizer, not used
FREContextInitializer contextInitializer_orig; // sets function pointers of native extension
FREFunction init_orig; // sets the decryption key
FREFunction setData_orig; // loads byte array of swz file contents
FREFunction getData_orig; // returns decrypted UTF8 data of one file inside an swz

std::string createFileName(const std::string& data) {
    std::string fileName;
    if (data.starts_with("<LevelDesc")) {
        std::string temp =
            data.substr(data.find("LevelName=") + std::string("LevelName=\"").size());
        fileName = temp.substr(0, temp.find('"')) + ".xml";
    } else if (data.starts_with('<')) {
        fileName = data.substr(1, data.find(">") - 1) + ".xml";
    } else {
        fileName = data.substr(0, data.find("\n") - 1) + ".csv";
    }

    return fileName;
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(module);

        FILE* file = nullptr;
        AllocConsole();
        freopen_s(&file, "CONOUT$", "w", stdout);
        SetWindowTextA(GetConsoleWindow(), "Rawdata proxy console");

        auto orig_dll =
            LoadLibraryA("META-INF/AIR/extensions/RawData/META-INF/ANE/Windows-x86/RawData.dll");
        if (!orig_dll) {
            std::cout << "could not load original rawdata dll\n";
        }

        ExtInitializer_orig = (FREInitializer)GetProcAddress(orig_dll, "ExtInitializer");
        if (!ExtInitializer_orig) {
            std::cout << "could not get ExtInitializer address\n";
        }

        ExtFinalizer_orig = (FREFinalizer)GetProcAddress(orig_dll, "ExtFinalizer");
        if (!ExtFinalizer_orig) {
            std::cout << "could not get ExtFinalizer address\n";
        }

        auto args = GetCommandLineA();
        if (strstr(args, "-ptr") && strstr(args, "-noeac")) {
            std::cout << "Loading custom maps enabled" << std::endl;
            shouldLoadCustomMaps = true;
        } else {
            std::cout << "Not loading custom maps\nuse launch options -noeac and -ptr to "
                         "enable\nyou can close this window"
                      << std::endl;
            shouldLoadCustomMaps = false;
            FreeConsole();
        }
    }

    return TRUE;
}

FREObject init(FREContext ctx, void* functionData, uint32_t argc, FREObject argv[]) {
    std::cout << "RawData_Init called" << std::endl;
    uint32_t key = 0;
    FREGetObjectAsUint32(*argv, &key);
    std::cout << "Key: " << key << std::endl;
    return init_orig(ctx, functionData, argc, argv);
}

FREObject setData(FREContext ctx, void* functionData, uint32_t argc, FREObject argv[]) {
    std::cout << "RawData_SetData called" << std::endl;
    return setData_orig(ctx, functionData, argc, argv);
}

/*
*/

FREObject getData(FREContext ctx, void* functionData, uint32_t argc, FREObject argv[]) {
    // check if used as log or not
    std::cout << "argc: " << argc << std::endl;
    if (argc > 0) {
        std::cout << "trying to log" << std::endl;
        const uint8_t* text;
        uint32_t length = 0;
        if (FREGetObjectAsUTF8(argv[0], &length, &text) == FRE_OK && length > 0) {
            std::cout << "[AS]: " << text << std::endl;
        } 
        return nullptr;
    }

    FREObject data = getData_orig(ctx, functionData, argc, argv);
    const char* cData;
    uint32_t length = 0;
    if (FREGetObjectAsUTF8(data, &length, (const uint8_t**)&cData) == FRE_OK) {
        std::string contents(cData);
        std::string filename = createFileName(contents);
        std::cout << filename << std::endl;

        // bad way to detect which swz is being loaded
        if (activeSwz == SwzType::Unknown) {
            if (filename == "MasterFileList.xml") {
                activeSwz = SwzType::Engine;
            } else if (filename == "AnimTypes.xml") {
                activeSwz = SwzType::Init;
            } else if (filename == "BattleHill.xml") {
                activeSwz = SwzType::Dynamic;
            } else if (filename == "LevelSetTypes.xml") {
                activeSwz = SwzType::Game;
            }
        }

        return data;
    }

    switch (activeSwz) {
    case SwzType::Dynamic:
        std::cout << "finished loading Dynamic.swz\n" << std::endl;
        break;
    case SwzType::Engine:
        std::cout << "finished loading Engine.swz\n" << std::endl;
        break;
    case SwzType::Game:
        std::cout << "finished loading Game.swz\n" << std::endl;
        break;
    case SwzType::Init:
        std::cout << "finished loading Init.swz\n" << std::endl;
        break;
    default:
        break;
    }

    activeSwz = SwzType::Unknown;

    return data;
}

void contextInitializer(void* extData, const uint8_t* ctxType, FREContext ctx,
                        uint32_t* numFunctionsToSet, const FRENamedFunction** functionsToSet) {
    std::cout << "Context initializer called" << std::endl;
    contextInitializer_orig(extData, ctxType, ctx, numFunctionsToSet, functionsToSet);

    init_orig = (*functionsToSet)[0].function;
    setData_orig = (*functionsToSet)[1].function;
    getData_orig = (*functionsToSet)[2].function;

    if (shouldLoadCustomMaps) {
        FRENamedFunction* funcs = (FRENamedFunction*)malloc(sizeof(FRENamedFunction) * (*numFunctionsToSet));
        memcpy(funcs, *functionsToSet, sizeof(FRENamedFunction) * (*numFunctionsToSet));
        funcs[0].function = init;
        funcs[1].function = setData;
        funcs[2].function = getData;
        *functionsToSet = funcs;
    }
}

DLL_EXPORT void ExtInitializer(void** extDataToSet, FREContextInitializer* ctxInitializerToSet,
                               FREContextFinalizer* ctxFinalizerToSet) {
    std::cout << "ExtInitializer called" << std::endl;
    ExtInitializer_orig(extDataToSet, ctxInitializerToSet, ctxFinalizerToSet);
    contextInitializer_orig = *ctxInitializerToSet;
    *ctxInitializerToSet = contextInitializer;
}

DLL_EXPORT void ExtFinalizer(void* extData) {
    std::cout << "ExtFinalizer called" << std::endl;
    ExtFinalizer_orig(extData);
    return;
}

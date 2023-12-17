#define WIN32_LEAN_AND_MEAN
#include "tinyxml2.h"
#include <Windows.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tlhelp32.h>

DWORD getProcessByName(const char* processName) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 procEntry = {0};
    procEntry.dwSize = sizeof(PROCESSENTRY32);

    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    if (!Process32First(snapshot, &procEntry)) {
        return 0;
    }

    while (Process32Next(snapshot, &procEntry)) {
        if (!strcmp(procEntry.szExeFile, processName)) {
            CloseHandle(snapshot);
            return procEntry.th32ProcessID;
        }
    }

    CloseHandle(snapshot);
    return 0;
}

void waitForGame() {
    while (!getProcessByName("BrawlhallaGame.exe")) {
        Sleep(100);
    }
}

// matches original file exactly so eac can work
std::string origData;
bool resetRawdataLibrary(const char* xmlPath) {
    std::ofstream out(xmlPath, std::ios::binary);
    if (!out.is_open()) {
        return false;
    }

    out << origData;
    out.close();
    return true;
}

bool setRawdataLibrary(const char* xmlPath, const char* dllName) {
    using namespace tinyxml2;

    std::ifstream file(xmlPath);
    if (!file.is_open()) {
        std::cerr << "could not open rawdata extension.xml" << std::endl;
        return false;
    }

    std::ostringstream str;
    str << file.rdbuf();
    std::string data = str.str();
    file.close();

    XMLDocument doc;
    const errno_t err = doc.Parse(data.c_str());
    if (err) {
        std::cerr << "could not parse extension.xml: " << err << std::endl;
        return false;
    }

    XMLElement* elem = doc.RootElement()
                           ->FirstChildElement("platforms")
                           ->FirstChild()
                           ->FirstChildElement("applicationDeployment")
                           ->FirstChildElement("nativeLibrary");

    if (!elem->Value()) {
        std::cerr << "could not find nativeLibrary element in extension.xml" << std::endl;
        return false;
    }

    if (!strcmp(elem->GetText(), "RawData.dll")) {
        origData = data;
    }

    elem->SetText(dllName);

    std::ofstream out(xmlPath, std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "could not save extension.xml" << std::endl;
        return false;
    }

    XMLPrinter printer;
    doc.Print(&printer);
    out << printer.CStr();
    out.close();

    return true;
}

int main(int argc, char* argv[]) {
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    std::string args = "Brawlhalla.exe -noeac -ptr ";
    for (int i = 1; i < argc; i++) {
        args += argv[i];
        args += " ";
    }

    const char* xmlPath = "META-INF/AIR/extensions/RawData/META-INF/ANE/extension.xml";
    if (!setRawdataLibrary(xmlPath, "RawData-proxy.dll")) {
        std::cerr << "could not enable rawdata proxy" << std::endl;
        ShowWindow(GetConsoleWindow(), SW_SHOW);
        std::cin.get();
        return 1;
    }

    std::cout << "Launching: " + args << std::endl;

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    if (!CreateProcess(NULL, (char*)args.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        std::cerr << "CreateProcess failed: " << GetLastError() << std::endl;
        ShowWindow(GetConsoleWindow(), SW_SHOW);
        std::cin.get();
        return 1;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    waitForGame();
    Sleep(3000);

    if (origData.empty() || !resetRawdataLibrary(xmlPath)) {
        std::cerr << "could not reset rawdata to original\nmanually changing extension.xml file "
                     "might be needed"
                  << std::endl;
        ShowWindow(GetConsoleWindow(), SW_SHOW);
        std::cin.get();
    }

    return 0;
}
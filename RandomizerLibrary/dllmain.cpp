// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

std::string outputString;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

extern "C" __declspec(dllexport) const char* __cdecl GenerateSamplePuzzle()
{
	Generate* generator = new Generate();
	generator->initPanel(0);
	generator->resetConfig();
	generator->setGridSize(4, 4);
	// generator->generate(0, Decoration::Start, 1, Decoration::Exit, 1, Decoration::Dot_Intersection, 25, Decoration::Gap, 4);
	generator->generate(0, Decoration::Start, 2, Decoration::Exit, 2, Decoration::Dot_Intersection, 25, Decoration::Triangle2 | Decoration::Magenta, 4);
	outputString = generator->exportPanel();
	// outputBuffer = new char[result.length() + 1];
	// strcpy_s(outputBuffer, result.length() + 1, generator->exportPanel().c_str());
	delete generator;
	return outputString.c_str();
}
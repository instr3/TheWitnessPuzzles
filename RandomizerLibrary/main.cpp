#include "Generate.h"
#include "pch.h"
#include <iostream>
#include <io.h>
#include <fcntl.h>

int WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR    lpCmdLine,
	int      nCmdShow)
{
	AllocConsole();
	HANDLE stdHandle;
	int hConsole;
	FILE* fp;
	stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	hConsole = _open_osfhandle((long)stdHandle, _O_TEXT);
	fp = _fdopen(hConsole, "w");

	freopen_s(&fp, "CONOUT$", "w", stdout);

	printf("Hello console on\n");
	Generate* generator = new Generate();
	generator->initPanel(0);
	generator->resetConfig();
	generator->setGridSize(4, 4);
	generator->generate(0x0005D, Decoration::Start, 1, Decoration::Exit, 1, Decoration::Dot_Intersection, 25, Decoration::Triangle2 | Decoration::Orange, 4);
	std::cout << generator->exportPanel() << std::endl;
	system("pause");
	return 0;
}
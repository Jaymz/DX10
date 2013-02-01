#include "systemclass.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdShow) {
	SystemClass* systemClass;

	bool result;

	systemClass = new SystemClass;
	if (!systemClass) {
		return 0;
	}

	result = systemClass->Initialize();
	if (result) {
		systemClass->Run();
	}

	systemClass->Shutdown();
	delete systemClass;
	systemClass = 0;

	return 0;
}
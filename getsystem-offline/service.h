#pragma once

BOOL ServiceInstall(const char *path, const char *name);
BOOL ServiceDelete(const char *name);
BOOL ServiceStart(const char *name);
BOOL ServiceStop(const char *name);
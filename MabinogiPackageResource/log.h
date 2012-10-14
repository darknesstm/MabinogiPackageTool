#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LOG_FATAL		(0)
#define LOG_ERROR		(LOG_FATAL + 1)
#define LOG_WARN		(LOG_ERROR + 1)
#define LOG_INFO		(LOG_WARN + 1)
#define LOG_DEBUG		(LOG_INFO + 1)
#define LOG_TRACE		(LOG_DEBUG + 1)

typedef void(CALLBACK *LOG_HANDLE_FUNC)(int level, LPCTSTR message);

void pack_log(int level, LPCTSTR message);

BOOL pack_add_log_handle(LOG_HANDLE_FUNC handle);
BOOL pack_remove_log_handle(LOG_HANDLE_FUNC handle);

#ifdef __cplusplus
}
#endif
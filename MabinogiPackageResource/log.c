#include "log.h"
#define HANDLE_MAX_COUNT 24

LOG_HANDLE_FUNC g_handles[HANDLE_MAX_COUNT] = {0};

void pack_log(int level, LPCTSTR message)
{
	size_t i;
	for (i = 0; i < HANDLE_MAX_COUNT; i++)
	{
		if (g_handles[i] != NULL)
		{
			g_handles[i](level, message);
		}
	}
}

BOOL pack_add_log_handle(LOG_HANDLE_FUNC handle)
{
	size_t i;
	for (i = 0; i < HANDLE_MAX_COUNT; i++)
	{
		if (g_handles[i] == NULL)
		{
			g_handles[i] = handle;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL pack_remove_log_handle(LOG_HANDLE_FUNC handle)
{
	size_t i;
	for (i = 0; i < HANDLE_MAX_COUNT; i++)
	{
		if (g_handles[i] == handle)
		{
			g_handles[i] = NULL;
			return TRUE;
		}
	}
	return FALSE;
}
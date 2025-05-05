#pragma once

#include <Windows.h>
#include <functional>

typedef struct
{
	UINT    msg;
	WPARAM  wParam;
	LPARAM  lParam;
} EventArgs_t;
typedef std::function<void(void* const sender, EventArgs_t info)> EventCallback_t;
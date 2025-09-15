#pragma once

#include <Windows.h>
#include <functional>

namespace mui 
{
	typedef struct
	{
		UINT    msg;
		WPARAM  wParam;
		LPARAM  lParam;
		BOOL	handled;
	} EventArgs_t;
	typedef std::function<void(void* const sender, EventArgs_t* info)> EventCallback_t;
};
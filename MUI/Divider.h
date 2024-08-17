#ifndef DIVIDER_H
#define DIVIDER_H
#include <windows.h>

BOOL Divider_Init(HINSTANCE hInstance, LPCTSTR szClassName, HBRUSH hBackgroundBrush, DWORD dwNotifyMsg);
BOOL Divider_Deinit(HINSTANCE hInstance);
LONG Divider_GetMovement(LPARAM ptr);
LONG Divider_GetState(LPARAM ptr);
POINT Divider_GetPosition(LPARAM ptr);

typedef enum
{
	DIVIDER_START,
	DIVIDER_MOVE,
	DIVIDER_END
} eDividerState;

struct sDividerMsgData
{
	sDividerMsgData()
	{
		bIsDragged = FALSE;
		memset(&capt,0,sizeof(capt));
		memset(&cur,0,sizeof(cur));
		memset(&prev, 0, sizeof(prev));
	}

	BOOL bIsDragged;
	BOOL bIsVeritcal;
	POINT capt;
	POINT prev;
	POINT cur;
	eDividerState state;
};



#endif
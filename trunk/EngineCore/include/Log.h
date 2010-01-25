#pragma once

#include "common.h"
#define LOG_TARGET_CONSOLE			0x00000001
#define LOG_TARGET_FILE				0x00000002
#define LOG_TARGET_WINDOW			0x00000004
#define LOG_TARGET_ALL				(LOG_TARGET_CONSOLE|LOG_TARGET_FILE|LOG_TARGET_WINDOW)
#define LOG_WINDOW_TITLE			"LOGÊä³ö"
#define LOG_WINDOW_CX				300
#define LOG_WINDOW_CY				600

#define LOG(_text) GetLog()->Log(_text);

class DLL_EXPORT CLog
{
public:
	CLog();
	CLog(UINT32 nTarget, LPSTR szFilename = "log.txt");
	~CLog();
public:
	void	SetFilename(LPSTR szFilename);
	void	CreateLogWindow();
	int		Log(LPSTR fmt, ...);

private:
	unsigned int	m_nTarget;
	char			m_szFilename[MAX_PATH];
	HWND			m_hwnd;
	HWND			m_hwndList;
private:
	static	LRESULT CALLBACK
	CLog::WndProcLog(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

//DECLARE ZFLog*		g_pLog;
CLog* GetLog();
#pragma once
#include "windows.h"

class CDataPlugBase
{
public:
	CDataPlugBase(){};
	virtual ~CDataPlugBase(){};

	virtual const char * GetTitle()		= 0;
	virtual const char * GetFormat()	= 0;

	virtual DWORD GetExportDataType() = 0;
	virtual DWORD GetImportDataType() = 0;

	virtual void Release() = 0;
};
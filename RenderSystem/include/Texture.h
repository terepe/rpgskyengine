#pragma once
#include "Common.h"

class DLL_EXPORT CTexture
{
	friend class CTextureMgr;
public:
	enum TEX_TYPE
	{
		TEX_TYPE_UNKNOWN,
		TEX_TYPE_FORM_FILE,
		TEX_TYPE_NORMAL,
		TEX_TYPE_DYNAMIC,
		TEX_TYPE_CUBETEXTURE,
		TEX_TYPE_RENDERTARGET,
		TEX_TYPE_DEPTHSTENCIL,
	};
public:
	CTexture();
	virtual ~CTexture();
public:
	//
	virtual void FillLevel(void* pBuf, int nSize, int nWidth, int nHeight, int nLevel)=0;
	virtual void Filter(int nSrcLevel, int nFilter);										// 生成minmap
	virtual	void SaveToFile(const std::string& strFilename)=0;								// 保存
	//
	void	SetFilename(const std::string& strFilename){m_strFilename = strFilename;}
	const std::string& GetFilename(){return m_strFilename;}
	void	SetLevels(int nLevels)	{m_nLevels = nLevels;}
	int		GetLevels()				{return m_nLevels;}
	int		GetWidth()				{return m_nWidth;}
	int		GetHeight()				{return m_nHeight;}
protected:
	// 创建纹理
	virtual void createTexture(int nWidth, int nHeight, int nLevels, bool bPoolManaged)=0;
	virtual void createDynamicTexture(int nWidth, int nHeight)=0;
	virtual bool createTextureFromFile(const std::string& strFilename, int nLevels);
	virtual void createTextureFromFileInMemory(void* pBuf, int nSize, int nLevels)=0;
	virtual void createTextureFromMemory(void* pBuf, int nSize, int nWidth, int nHeight, int nLevels);
	virtual void createCubeTexture(int nSize, int nLevels, bool bPoolManaged=true)=0;
	virtual void createCubeTextureFromFile(const std::string& strFilename)=0;

	virtual bool createFromBLP(void* pBuf, int nSize, int nLevels);

	virtual void createRenderTarget(int nWidth, int nHeight)=0;
	virtual void createDepthStencil(int nWidth, int nHeight)=0;

	virtual void releaseBuffer()=0;
	void	reset();
	void	load();
	bool	isLoaded()	{return m_bLoaded;}

	void	setTextureMgr(CTextureMgr* pTextureMgr);
protected:
	std::string			m_strFilename;
	bool				m_bLoaded;
	// 数据备份
	bool				m_bPoolManaged;
	bool				m_bUseBuffer;
	char*				m_buffer;
	int					m_nBufferSize;
	int					m_nWidth, m_nHeight;
	int					m_nLevels;
	TEX_TYPE			m_eTexType;
	// texture wrapping
	bool bSwrap, bTwrap;
	CTextureMgr*		m_pTextureMgr;
};
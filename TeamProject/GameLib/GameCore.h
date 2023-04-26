#pragma once
#include "Writer.h"
#include "RenderTarget.h"
#include "DXState.h"
class GameCore : public DXDevice
{
public:
	virtual bool	Init() { return true; }
	virtual bool	Frame() { return true; }
	virtual bool	Render() { return true; }
	virtual bool	Release() { return true; }

	virtual HRESULT CreateDxResource();
	virtual HRESULT DeleteDxResource();

	virtual bool	Run();

public:
	bool		CoreInit();
	bool		CoreFrame();

	bool		CorePreRender();
	bool		CoreRender();
	bool		CorePostRender();

	bool		CoreRelease();

public:
	bool	m_bGameRun;
	bool	m_isRelease;
	//Writer  m_Writer;

	BaseObject		m_BG;
	RenderTarget	m_RT;
};


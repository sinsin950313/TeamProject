#pragma once
#include "BaseObject.h"

class Sprite : public BaseObject
{
public:
	//std::vector<Texture*>	m_pTextureList;
	//std::vector<RECT>		m_UVRectList;

	Shader* m_pSwapVS[2]; // VS, RibbonVS
	Shader* m_pSwapGS[2]; // NULL, RibbonGS
	Shader* m_pSwapPS[3]; // PS, Distortion, COLOR_PS

	Shader* m_pVS;
	Shader* m_pGS;
	Shader* m_pPS;

public:
	virtual bool Init() override;
	virtual bool Frame() override;
	virtual bool PreRender() override;
	virtual bool Render() override;
	virtual bool PostRender() override;
	bool			RenderInstancing(UINT size);
	virtual bool Release() override;


public:
	Sprite();
	virtual ~Sprite();

};


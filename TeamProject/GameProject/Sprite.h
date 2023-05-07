#pragma once
#include "BaseObject.h"
#include "RenderTarget.h"

class Sprite : public BaseObject
{
public:
	//std::vector<CTexture*>	m_pTextureList;
	//std::vector<RECT>		m_UVRectList;

	//CShader* m_pSwapShader[2];
	Shader* m_pSwapVS[2]; // VS, RibbonVS
	Shader* m_pSwapGS[2]; // NULL, RibbonGS
	Shader* m_pSwapPS[4]; // PS, Distortion, COLOR_PS

	Shader* m_pVS;
	Shader* m_pGS;
	Shader* m_pPS;

	RenderTarget* m_EmissionRT;

public:
	virtual bool Init() override;
	virtual bool Frame() override;
	virtual bool PreRender() override;
	virtual bool Render() override;
	virtual bool PostRender() override;
	bool			RenderInstancing(UINT size);
	virtual bool Release() override;

	void			SetBlurData(bool isBlur);

public:
	Sprite();
	virtual ~Sprite();

};

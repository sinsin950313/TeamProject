#pragma once
#include "BaseObject.h"

class Sprite : public BaseObject
{
public:
	//std::vector<Texture*>	m_pTextureList;
	//std::vector<RECT>		m_UVRectList;

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


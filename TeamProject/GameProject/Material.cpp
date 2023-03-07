#include "Material.h"
#include "TextureMgr.h"

namespace SSB
{
	Material::Material() : _materialIndex(0)
	{
		//_textureArray = new Texture * [kTextureTypeCount];
		_textureArray.resize(kTextureTypeCount);
	}

	Material::~Material()
	{
		Release();
	}

	void Material::Initialize_SetMaterialIndex(MaterialIndex materialIndex)
	{
		_materialIndex = materialIndex;
	}

	void Material::Initialize_SetTexture(TextureType textureType, Texture* texture)
	{
		_textureArray[textureType] = texture;
	}

	bool Material::Init()
	{
		return true;
	}

	bool Material::Frame()
	{
		return true;
	}

	bool Material::Render()
	{
		if (_textureArray[kDiffuse])
		{
			auto shaderResourceView = _textureArray[kDiffuse]->m_pTextureSRV;
			m_pImmediateContext->PSSetShaderResources(_materialIndex, 1, &shaderResourceView);
		}
		
		return true;
	}

	bool Material::Release()
	{
		_materialIndex = -1;

		return true;
	}
	void Material::Deserialize(std::string& serialedString)
	{
		serialedString = GetUnitObject(serialedString, 0).str;

		int offset = 0;
		{
			offset = serialedString.find(_materialIndexStr, offset);
			auto data = GetUnitElement(serialedString, offset);
			std::string atomic = data.str;
			offset = data.offset;
			Serializeable::Deserialize(atomic, _materialIndex);
		}

		for (int i = 0; i < kTextureTypeCount; ++i)
		{
			auto data = GetUnitElement(serialedString, offset);
			std::string atomic = data.str;
			offset = data.offset;
			I_Tex.Load(mtw(GetUnitAtomic(data.str, 0).str), &_textureArray[i]);
		}
	}
}

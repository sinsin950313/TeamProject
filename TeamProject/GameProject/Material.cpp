#include "Material.h"
#include "TextureMgr.h"
#include "CommonPath.h"
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
	void Material::Deserialize(const char* buffer, int size, int& offset)
	{
		{
			memcpy(&_materialIndex, buffer + offset, sizeof(int));
			offset += sizeof(int);
		}

		{
			for (int i = 0; i < kTextureTypeCount; ++i)
			{
				int fileNameSize;
				memcpy(&fileNameSize, buffer + offset, sizeof(int));
				offset += sizeof(int);

				char* fileNameBuffer = new char[fileNameSize];
				memcpy(fileNameBuffer, buffer + offset, fileNameSize);
				offset += fileNameSize;
				std::string fileName(fileNameBuffer, fileNameSize);
				if (fileName != "Empty")
				{
					I_Tex.Load(kTextureResourcePath + mtw(fileName), &_textureArray[i]);
				}
				delete[] fileNameBuffer;
			}
		}
	}
}

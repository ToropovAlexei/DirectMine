#pragma once

#include "TextureLoader.h"

class TextureAtlas
{
public:
	struct UVCoords
	{
		float u1;
		float v1;
		float u2;
		float v2;
	};
public:
	TextureAtlas();

	void BuildAtlas(ID3D11Device* device, ID3D11DeviceContext* context);

	Microsoft::WRL::ComPtr<ID3D11Texture2D> GetAtlas();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetAtlasSRV();

	UVCoords GetTextureCoords(std::string texName);

private:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_atlasTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_atlasSRV;

	std::unordered_map<std::string, UVCoords> m_texturesCoords;

	int m_textureSize = 16;
	float m_atlasGridStep = 0.0f;
	int m_atlasSize = 0;
	int m_texturesPerRow = 0;
};


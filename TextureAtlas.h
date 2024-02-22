#pragma once

#include "TextureLoader.h"
#include "UVCoords.h"

class TextureAtlas
{
public:
	TextureAtlas() = delete;
	static void BuildAtlas(ID3D11Device* device, ID3D11DeviceContext* context);

	static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Get3dTextureSRV() noexcept
	{
		return texture3dSRV;
	};

	static inline float GetTextureIdx(std::string& texName) noexcept
	{
		return texturesIndices[texName];
	};

private:
	static Microsoft::WRL::ComPtr<ID3D11Texture2D> atlas3d;
	static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture3dSRV;
	static std::unordered_map<std::string, float> texturesIndices;
};


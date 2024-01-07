#pragma once

#include "TextureLoader.h"
#include "UVCoords.h"

class TextureAtlas
{
public:
	TextureAtlas() = delete;
	static void BuildAtlas(ID3D11Device* device, ID3D11DeviceContext* context);

	static Microsoft::WRL::ComPtr<ID3D11Texture2D> GetAtlas() noexcept;
	static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetAtlasSRV() noexcept;

	static UVCoords GetTextureCoords(std::string texName) noexcept;

private:
	static Microsoft::WRL::ComPtr<ID3D11Texture2D> atlasTexture;
	static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> atlasSRV;
	static std::unordered_map<std::string, UVCoords> texturesCoords;
};


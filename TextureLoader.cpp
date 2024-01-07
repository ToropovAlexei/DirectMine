#include "pch.h"
#include "TextureLoader.h"

std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11Resource>> TextureLoader::LoadTextures(ID3D11Device* device)
{
	std::string path = "textures";
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11Resource>> textures;
	for (const auto& texturePath : std::filesystem::directory_iterator(path))
	{
		std::filesystem::path filePath = texturePath.path();
		textures.insert({ filePath.filename().string(), TextureLoader::LoadTexture(device, filePath)});
	}
	if (textures.empty())
	{
		throw std::runtime_error("Textures not found in " + path + " path");
	}
	return textures;
}

Microsoft::WRL::ComPtr<ID3D11Resource> TextureLoader::LoadTexture(ID3D11Device* device, std::filesystem::path filePath)
{
	Microsoft::WRL::ComPtr<ID3D11Resource> texture = nullptr;
	HRESULT hr = DirectX::CreateWICTextureFromFileEx(device, 
		filePath.c_str(),
		0u, 
		D3D11_USAGE_DEFAULT, 
		D3D11_BIND_SHADER_RESOURCE, 
		0u, 
		0u, 
		DirectX::WIC_LOADER_FORCE_RGBA32, 
		texture.GetAddressOf(),
		nullptr);
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to load texture " + filePath.filename().string());
	}
	return texture;
}

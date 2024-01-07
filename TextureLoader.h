#pragma once
#include <filesystem>

class TextureLoader
{
public:
	static std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11Resource>> LoadTextures(ID3D11Device* device);

private:
	static Microsoft::WRL::ComPtr<ID3D11Resource> LoadTexture(ID3D11Device* device, std::filesystem::path filePath);
};


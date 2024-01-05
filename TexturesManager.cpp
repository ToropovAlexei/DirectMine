#include "pch.h"
#include "TexturesManager.h"

TexturesManager::TexturesManager(ID3D11Device* device)
{
	LoadBlockTextures(device);
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> TexturesManager::GetBlockTexture(CubeId id)
{
	return m_blockTexMap[id];
}

void TexturesManager::LoadBlockTextures(ID3D11Device* device)
{
	LoadBlockTexture(device, CubeId::Debug, L"cube.png");
	LoadBlockTexture(device, CubeId::Dirt, L"dirt.png");
	LoadBlockTexture(device, CubeId::Leaves, L"leaves.png");
}

void TexturesManager::LoadBlockTexture(ID3D11Device* device, CubeId id, const wchar_t* filename)
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV = nullptr;
	DirectX::CreateWICTextureFromFileEx(device, filename, 0u, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0u, 0u, DirectX::WIC_LOADER_FORCE_RGBA32, nullptr, textureSRV.GetAddressOf());
	m_blockTexMap.insert({ id, textureSRV });
}

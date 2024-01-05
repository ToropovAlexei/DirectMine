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
}

void TexturesManager::LoadBlockTexture(ID3D11Device* device, CubeId id, const wchar_t* filename)
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV = nullptr;
	DirectX::CreateWICTextureFromFile(device, filename, nullptr, &textureSRV);
	m_blockTexMap.insert({ id, textureSRV });
}

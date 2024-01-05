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
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV = nullptr;
	DirectX::CreateWICTextureFromFile(device, L"cube.png", nullptr, &textureSRV);
	m_blockTexMap.insert({CubeId::Debug, textureSRV});
}

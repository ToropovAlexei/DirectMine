#pragma once

#include "Cube.h"

class TexturesManager
{
public:
	TexturesManager(ID3D11Device* device);

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetBlockTexture(CubeId id);

private:
	void LoadBlockTextures(ID3D11Device* device);

	std::unordered_map<CubeId, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_blockTexMap;
};


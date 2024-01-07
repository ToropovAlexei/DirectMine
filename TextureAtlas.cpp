#include "pch.h"
#include "TextureAtlas.h"
#include "MathUtils.h"

TextureAtlas::TextureAtlas()
{
}

void TextureAtlas::BuildAtlas(ID3D11Device* device, ID3D11DeviceContext* context)
{
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11Resource>> textures = TextureLoader::LoadTextures(device);
    int squareSize = MathUtils::GetNearestSquareNumber(static_cast<int>(textures.size()));
	m_atlasSize = MathUtils::GetNextPowerOfTwo(squareSize * m_textureSize);
    m_texturesPerRow = m_atlasSize / m_textureSize;
    m_atlasGridStep = static_cast<float>(m_textureSize) / m_atlasSize;

    D3D11_TEXTURE2D_DESC atlasDesc = {};
    atlasDesc.Width = m_atlasSize;
    atlasDesc.Height = m_atlasSize;
    atlasDesc.MipLevels = 1;
    atlasDesc.ArraySize = 1;
    atlasDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    atlasDesc.SampleDesc.Count = 1;
    atlasDesc.Usage = D3D11_USAGE_DEFAULT;
    atlasDesc.CPUAccessFlags = 0;
    atlasDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    DX::ThrowIfFailed(device->CreateTexture2D(&atlasDesc, nullptr, &m_atlasTexture));

    int i = 0;
    for (auto& texturePair : textures)
    {
        int gridX = i % m_texturesPerRow;
        int gridY = i / m_texturesPerRow;
        int x = gridX * m_textureSize;
        int y = gridY * m_textureSize;
        context->CopySubresourceRegion(m_atlasTexture.Get(), 0, x, y, 0, texturePair.second.Get(), 0, nullptr);

        float u1 = gridX * m_atlasGridStep;
        float u2 = u1 + m_atlasGridStep;
        float v1 = gridY * m_atlasGridStep;
        float v2 = v1 + m_atlasGridStep;
        m_texturesCoords.insert({ texturePair.first, UVCoords(u1, v1, u2, v2) });
        i++;
    }
    DirectX::SaveWICTextureToFile(context, m_atlasTexture.Get(), GUID_ContainerFormatPng, L"atlas.png"); // TODO FOR TESTS ONLY

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Формат текстуры
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D; // Размерность шейдерного ресурса
    srvDesc.Texture2D.MipLevels = 1; // Количество уровней мип-карт (mip levels)

    DX::ThrowIfFailed(device->CreateShaderResourceView(m_atlasTexture.Get(), &srvDesc, &m_atlasSRV));
}

Microsoft::WRL::ComPtr<ID3D11Texture2D> TextureAtlas::GetAtlas()
{
    return m_atlasTexture;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> TextureAtlas::GetAtlasSRV()
{
    return m_atlasSRV;
}

TextureAtlas::UVCoords TextureAtlas::GetTextureCoords(std::string texName)
{
    return m_texturesCoords[texName];
}

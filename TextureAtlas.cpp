#include "pch.h"
#include "TextureAtlas.h"
#include "MathUtils.h"

TextureAtlas::TextureAtlas()
{
}

void TextureAtlas::BuildAtlas(ID3D11Device* device, ID3D11DeviceContext* context)
{
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11Resource>> textures = TextureLoader::LoadTextures(device);
    int squareSize = MathUtils::GetNearestSquareNumber(textures.size());
	m_atlasSize = MathUtils::GetNextPowerOfTwo(squareSize * m_textureSize);
    m_texturesPerRow = m_atlasSize / m_textureSize;

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

    size_t i = 0;
    for (auto& texturePair : textures)
    {
        int x = (i % m_texturesPerRow) * m_textureSize;
        int y = (i / m_texturesPerRow) * m_textureSize;
        context->CopySubresourceRegion(m_atlasTexture.Get(), 0, x, y, 0, texturePair.second.Get(), 0, nullptr);
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

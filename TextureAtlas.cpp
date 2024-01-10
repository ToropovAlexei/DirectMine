#include "pch.h"
#include "TextureAtlas.h"
#include "MathUtils.h"

Microsoft::WRL::ComPtr<ID3D11Texture2D> TextureAtlas::atlasTexture;
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> TextureAtlas::atlasSRV;
std::unordered_map<std::string, UVCoords> TextureAtlas::texturesCoords;

void TextureAtlas::BuildAtlas(ID3D11Device* device, ID3D11DeviceContext* context)
{
    const int textureSize = 16;
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11Resource>> textures = TextureLoader::LoadTextures(device);
    int squareSize = MathUtils::GetNearestSquareNumber(static_cast<int>(textures.size()));
	int atlasSize = MathUtils::GetNextPowerOfTwo(squareSize * textureSize);
    int texturesPerRow = atlasSize / textureSize;
    float atlasGridStep = static_cast<float>(textureSize) / atlasSize;
    // Обязательно вычитаем 0.5f чтобы текстура не смешивалась с соседней 
    // https://learn.microsoft.com/en-us/windows/win32/direct3d9/directly-mapping-texels-to-pixels?redirectedfrom=MSDN
    float atlasGridStepWithCorrection = static_cast<float>(textureSize - 0.5f) / atlasSize;

    D3D11_TEXTURE2D_DESC atlasDesc = {};
    atlasDesc.Width = atlasSize;
    atlasDesc.Height = atlasSize;
    atlasDesc.MipLevels = 1;
    atlasDesc.ArraySize = 1;
    atlasDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    atlasDesc.SampleDesc.Count = 1;
    atlasDesc.Usage = D3D11_USAGE_DEFAULT;
    atlasDesc.CPUAccessFlags = 0;
    atlasDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    DX::ThrowIfFailed(device->CreateTexture2D(&atlasDesc, nullptr, &atlasTexture));

    int i = 0;
    for (auto& texturePair : textures)
    {
        int gridX = i % texturesPerRow;
        int gridY = i / texturesPerRow;
        int x = gridX * textureSize;
        int y = gridY * textureSize;
        context->CopySubresourceRegion(atlasTexture.Get(), 0, x, y, 0, texturePair.second.Get(), 0, nullptr);

        float u1 = gridX * atlasGridStep;
        float u2 = u1 + atlasGridStepWithCorrection;
        float v1 = gridY * atlasGridStep;
        float v2 = v1 + atlasGridStepWithCorrection;
        texturesCoords.insert({ texturePair.first, UVCoords(u1, v1, u2, v2) });
        i++;
    }

#ifdef DEBUG
    // TODO FOR TESTS ONLY
    DirectX::SaveWICTextureToFile(context, atlasTexture.Get(), GUID_ContainerFormatPng, L"atlas.png");
#endif // DEBUG

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Формат текстуры
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D; // Размерность шейдерного ресурса
    srvDesc.Texture2D.MipLevels = 1; // Количество уровней мип-карт (mip levels)

    DX::ThrowIfFailed(device->CreateShaderResourceView(atlasTexture.Get(), &srvDesc, &atlasSRV));
}

Microsoft::WRL::ComPtr<ID3D11Texture2D> TextureAtlas::GetAtlas() noexcept
{
    return atlasTexture;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> TextureAtlas::GetAtlasSRV() noexcept
{
    return atlasSRV;
}

UVCoords TextureAtlas::GetTextureCoords(std::string texName) noexcept
{
    return texturesCoords[texName];
}

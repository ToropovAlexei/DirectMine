#include "pch.h"
#include "TextureAtlas.h"
#include "MathUtils.h"

Microsoft::WRL::ComPtr<ID3D11Texture2D> TextureAtlas::atlasTexture;
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> TextureAtlas::atlasSRV;
std::unordered_map<std::string, UVCoords> TextureAtlas::texturesCoords;

void TextureAtlas::BuildAtlas(ID3D11Device* device, ID3D11DeviceContext* context)
{
    const int textureSize = 16;
    const int padding = 32;
    const int textureWithPadding = textureSize + padding;
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11Resource>> textures = TextureLoader::LoadTextures(device);
    int squareSize = MathUtils::GetNearestSquareNumber(static_cast<int>(textures.size() + textures.size() * (textureSize / padding)));
	int atlasSize = MathUtils::GetNextPowerOfTwo(squareSize * textureWithPadding);
    int texturesPerRow = atlasSize / (textureWithPadding);
    float atlasGridStep = static_cast<float>(textureWithPadding) / atlasSize;
    float textureUVSize = static_cast<float>(textureSize) / atlasSize;
    float correction = static_cast<float>(0.5f) / atlasSize;

    D3D11_BOX topLineBox;
    topLineBox.left = 0;
    topLineBox.right = textureSize;
    topLineBox.top = 0;
    topLineBox.bottom = 1;
    topLineBox.front = 0;
    topLineBox.back = 1;

    D3D11_BOX bottomLineBox;
    bottomLineBox.left = 0;
    bottomLineBox.right = textureSize;
    bottomLineBox.top = textureSize - 1;
    bottomLineBox.bottom = textureSize;
    bottomLineBox.front = 0;
    bottomLineBox.back = 1;

    D3D11_BOX leftLineBox;
    leftLineBox.left = 0;
    leftLineBox.right = 1;
    leftLineBox.top = 0;
    leftLineBox.bottom = textureSize;
    leftLineBox.front = 0;
    leftLineBox.back = 1;

    D3D11_BOX rightLineBox;
    rightLineBox.left = textureSize - 1;
    rightLineBox.right = textureSize;
    rightLineBox.top = 0;
    rightLineBox.bottom = textureSize;
    rightLineBox.front = 0;
    rightLineBox.back = 1;

    D3D11_BOX leftTopPixelBox;
    leftTopPixelBox.left = 0;
    leftTopPixelBox.right = 1;
    leftTopPixelBox.top = 0;
    leftTopPixelBox.bottom = 1;
    leftTopPixelBox.front = 0;
    leftTopPixelBox.back = 1;

    D3D11_BOX rightTopPixelBox;
    rightTopPixelBox.left = textureSize - 1;
    rightTopPixelBox.right = textureSize;
    rightTopPixelBox.top = 0;
    rightTopPixelBox.bottom = 1;
    rightTopPixelBox.front = 0;
    rightTopPixelBox.back = 1;

    D3D11_BOX leftBottomPixelBox;
    leftBottomPixelBox.left = 0;
    leftBottomPixelBox.right = 1;
    leftBottomPixelBox.top = textureSize - 1;
    leftBottomPixelBox.bottom = textureSize;
    leftBottomPixelBox.front = 0;
    leftBottomPixelBox.back = 1;

    D3D11_BOX rightBottomPixelBox;
    rightBottomPixelBox.left = textureSize - 1;
    rightBottomPixelBox.right = textureSize;
    rightBottomPixelBox.top = textureSize - 1;
    rightBottomPixelBox.bottom = textureSize;
    rightBottomPixelBox.front = 0;
    rightBottomPixelBox.back = 1;

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
        int x = gridX * textureWithPadding;
        int y = gridY * textureWithPadding;
        context->CopySubresourceRegion(atlasTexture.Get(), 0, x, y, 0, texturePair.second.Get(), 0, nullptr);
        // Копируем пиксели сверху
        if (y > 0)
        {
            context->CopySubresourceRegion(atlasTexture.Get(), 0, x, y - 1, 0, texturePair.second.Get(), 0, &topLineBox);
            //context->CopySubresourceRegion(atlasTexture.Get(), 0, x, y - 2, 0, texturePair.second.Get(), 0, &topLineBox);
            //context->CopySubresourceRegion(atlasTexture.Get(), 0, x, y - 3, 0, texturePair.second.Get(), 0, &topLineBox);
            //context->CopySubresourceRegion(atlasTexture.Get(), 0, x, y - 4, 0, texturePair.second.Get(), 0, &topLineBox);
        }
        // Копируем пиксели снизу
        context->CopySubresourceRegion(atlasTexture.Get(), 0, x, y + textureSize, 0, texturePair.second.Get(), 0, &bottomLineBox);
        //context->CopySubresourceRegion(atlasTexture.Get(), 0, x, y + textureSize + 1, 0, texturePair.second.Get(), 0, &bottomLineBox);
        //context->CopySubresourceRegion(atlasTexture.Get(), 0, x, y + textureSize + 2, 0, texturePair.second.Get(), 0, &bottomLineBox);
        //context->CopySubresourceRegion(atlasTexture.Get(), 0, x, y + textureSize + 3, 0, texturePair.second.Get(), 0, &bottomLineBox);
        // Копируем пиксели слева
        if (x > 0)
        {
            context->CopySubresourceRegion(atlasTexture.Get(), 0, x - 1, y, 0, texturePair.second.Get(), 0, &leftLineBox);
            //context->CopySubresourceRegion(atlasTexture.Get(), 0, x - 2, y, 0, texturePair.second.Get(), 0, &leftLineBox);
            //context->CopySubresourceRegion(atlasTexture.Get(), 0, x - 3, y, 0, texturePair.second.Get(), 0, &leftLineBox);
            //context->CopySubresourceRegion(atlasTexture.Get(), 0, x - 4, y, 0, texturePair.second.Get(), 0, &leftLineBox);
        }
        // Копируем пиксели справа
        context->CopySubresourceRegion(atlasTexture.Get(), 0, x + textureSize, y, 0, texturePair.second.Get(), 0, &rightLineBox);
        //context->CopySubresourceRegion(atlasTexture.Get(), 0, x + textureSize + 1, y, 0, texturePair.second.Get(), 0, &rightLineBox);
        //context->CopySubresourceRegion(atlasTexture.Get(), 0, x + textureSize + 2, y, 0, texturePair.second.Get(), 0, &rightLineBox);
        //context->CopySubresourceRegion(atlasTexture.Get(), 0, x + textureSize + 3, y, 0, texturePair.second.Get(), 0, &rightLineBox);
        // Копируем пиксель слева сверху
        if (y > 0 && x > 0)
        {
            context->CopySubresourceRegion(atlasTexture.Get(), 0, x - 1, y - 1, 0, texturePair.second.Get(), 0, &leftTopPixelBox);
        }
        // Копируем пиксель справа сверху
        if (y > 0)
        {
            context->CopySubresourceRegion(atlasTexture.Get(), 0, x + textureSize, y - 1, 0, texturePair.second.Get(), 0, &rightTopPixelBox);
        }
        // Копируем пиксель слева снизу
        if (x > 0)
        {
            context->CopySubresourceRegion(atlasTexture.Get(), 0, x - 1, y + textureSize, 0, texturePair.second.Get(), 0, &leftBottomPixelBox);
        }
        // Копируем пиксель справа снизу
        context->CopySubresourceRegion(atlasTexture.Get(), 0, x + textureSize, y + textureSize, 0, texturePair.second.Get(), 0, &rightBottomPixelBox);

        float u1 = gridX * atlasGridStep;
        float u2 = u1 + textureUVSize;
        float v1 = gridY * atlasGridStep;
        float v2 = v1 + textureUVSize;
        texturesCoords.insert({ texturePair.first, UVCoords(u1, v1, u2, v2) });
        i++;
    }

    DirectX::SaveWICTextureToFile(context, atlasTexture.Get(), GUID_ContainerFormatPng, L"atlas.png");

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

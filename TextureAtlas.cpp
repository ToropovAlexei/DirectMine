#include "pch.h"
#include "TextureAtlas.h"
#include "MathUtils.h"

Microsoft::WRL::ComPtr<ID3D11Texture2D> TextureAtlas::atlasTexture;
Microsoft::WRL::ComPtr<ID3D11Texture3D> TextureAtlas::atlas3d;
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> TextureAtlas::atlasSRV;
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> TextureAtlas::texture3dSRV;
std::unordered_map<std::string, UVCoords> TextureAtlas::texturesCoords;
std::unordered_map<std::string, float> TextureAtlas::texturesIndices;

void TextureAtlas::BuildAtlas(ID3D11Device* device, ID3D11DeviceContext* context)
{
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11Resource>> textures = TextureLoader::LoadTextures(device);
    CD3D11_TEXTURE3D_DESC textureDesc(DXGI_FORMAT_R8G8B8A8_UNORM, 16, 16, static_cast<UINT>(textures.size()), 1);
    Microsoft::WRL::ComPtr<ID3D11Texture3D> texture3D;
    device->CreateTexture3D(&textureDesc, nullptr, texture3D.GetAddressOf());

    UINT slice = 0;
    for (const auto& texturePair : textures)
    {
        context->CopySubresourceRegion(texture3D.Get(), 0, 0, 0, slice, texturePair.second.Get(), 0, nullptr);
        texturesIndices[texturePair.first] = static_cast<float>(slice) / textures.size();
        slice++;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
    srvDesc.Texture3D.MipLevels = 1;
    srvDesc.Texture3D.MostDetailedMip = 0;

    DX::ThrowIfFailed(device->CreateShaderResourceView(texture3D.Get(), &srvDesc, &texture3dSRV));
}

Microsoft::WRL::ComPtr<ID3D11Texture2D> TextureAtlas::GetAtlas() noexcept
{
    return atlasTexture;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> TextureAtlas::GetAtlasSRV() noexcept
{
    return atlasSRV;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> TextureAtlas::Get3dTextureSRV() noexcept
{
    return texture3dSRV;
}

UVCoords TextureAtlas::GetTextureCoords(std::string& texName) noexcept
{
    return texturesCoords[texName];
}

float TextureAtlas::GetTextureIdx(std::string& texName) noexcept
{
    return texturesIndices[texName];
}

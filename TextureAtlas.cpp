#include "pch.h"
#include "TextureAtlas.h"
#include "MathUtils.h"

Microsoft::WRL::ComPtr<ID3D11Texture2D> TextureAtlas::atlas3d;
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> TextureAtlas::texture3dSRV;
std::unordered_map<std::string, float, TextureAtlas::string_hash, std::equal_to<>> TextureAtlas::texturesIndices;

void TextureAtlas::BuildAtlas(ID3D11Device* device, ID3D11DeviceContext* context)
{
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11Resource>> textures = TextureLoader::LoadTextures(device);
    CD3D11_TEXTURE2D_DESC textureDesc(DXGI_FORMAT_R8G8B8A8_UNORM, 16, 16, static_cast<UINT>(textures.size()), 1);
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture3D;
    DX::ThrowIfFailed(device->CreateTexture2D(&textureDesc, nullptr, texture3D.GetAddressOf()));

    UINT slice = 0;
    for (const auto& texturePair : textures)
    {
        context->CopySubresourceRegion(texture3D.Get(), slice, 0, 0, 0, texturePair.second.Get(), 0, nullptr);
        texturesIndices[texturePair.first] = static_cast<float>(slice);
        slice++;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    srvDesc.Texture2DArray.ArraySize = static_cast<UINT>(textures.size());
    srvDesc.Texture2DArray.FirstArraySlice = 0u;
    srvDesc.Texture2DArray.MipLevels = 1;
    srvDesc.Texture2DArray.MostDetailedMip = 0;

    DX::ThrowIfFailed(device->CreateShaderResourceView(texture3D.Get(), &srvDesc, &texture3dSRV));
}

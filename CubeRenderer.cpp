#include "pch.h"
#include "CubeRenderer.h"
#include "ShadersLoader.h"

CubeRenderer::CubeRenderer(ID3D11Device* device)
{
    BuildInputLayout(device);
    BuildSampler(device);
    BuildBlendState(device);
    m_vertexShader = ShadersLoader::LoadVertexShader(device, L"VertexShader.cso");
    m_pixelShader = ShadersLoader::LoadPixelShader(device, L"PixelShader.cso");
}

void CubeRenderer::DrawChunk(ID3D11DeviceContext1* context, Chunk* chunk)
{
    context->IASetVertexBuffers(0u, 1u, chunk->GetVertexBuffer().GetAddressOf(), &m_stride, &m_offset);
    context->IASetIndexBuffer(chunk->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0u);
    context->PSSetShader(m_pixelShader.Get(), nullptr, 0u);
    context->VSSetShader(m_vertexShader.Get(), nullptr, 0u);
    context->IASetInputLayout(m_inputLayout.Get());
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->PSSetSamplers(0u, 1u, m_sampler.GetAddressOf());
    context->OMSetBlendState(m_blendState.Get(), nullptr, 0xFFFFFFFF);
    context->PSSetShaderResources(0u, 1u, TextureAtlas::GetAtlasSRV().GetAddressOf());
    context->DrawIndexed(chunk->GetIndices().size(), 0u, 0u);
}

void CubeRenderer::BuildInputLayout(ID3D11Device* device)
{
    D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    auto vertexShaderBlob = ShadersLoader::LoadBlob(L"VertexShader.cso");
    device->CreateInputLayout(inputLayoutDesc,
        ARRAYSIZE(inputLayoutDesc),
        vertexShaderBlob->GetBufferPointer(),
        vertexShaderBlob->GetBufferSize(),
        &m_inputLayout);
}

void CubeRenderer::BuildSampler(ID3D11Device* device)
{
    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    device->CreateSamplerState(&samplerDesc, &m_sampler);
}

void CubeRenderer::BuildBlendState(ID3D11Device* device)
{
    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.AlphaToCoverageEnable = FALSE;
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    device->CreateBlendState(&blendDesc, &m_blendState);
}

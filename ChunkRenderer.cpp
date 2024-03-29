#include "pch.h"
#include "ChunkRenderer.h"
#include "ShadersLoader.h"

ChunkRenderer::ChunkRenderer(std::unique_ptr<DX::DeviceResources>& deviceResources) :
    m_deviceResources(deviceResources)
{
    BuildInputLayout();
    BuildSampler();
    BuildBlendState();
    BuildRasterState();
    m_vertexShader = ShadersLoader::LoadVertexShader(deviceResources->GetD3DDevice(), L"VertexShader.cso");
    m_pixelShader = ShadersLoader::LoadPixelShader(deviceResources->GetD3DDevice(), L"PixelShader.cso");
}

void ChunkRenderer::RenderChunks(std::vector<std::shared_ptr<Chunk>>& chunks)
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    context->GSSetShader(nullptr, nullptr, 0u);
    context->PSSetShader(m_pixelShader.Get(), nullptr, 0u);
    context->VSSetShader(m_vertexShader.Get(), nullptr, 0u);
    context->IASetInputLayout(m_inputLayout.Get());
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->PSSetSamplers(0u, 1u, m_sampler.GetAddressOf());
    context->OMSetBlendState(m_blendState.Get(), nullptr, 0xFFFFFFFF);
    context->PSSetShaderResources(0u, 1u, TextureAtlas::Get3dTextureSRV().GetAddressOf());
    context->RSSetState(m_rasterState.Get());

    for (int drawGroup = 0; drawGroup < 4; drawGroup++)
    {
        for (auto& chunk : chunks)
        {
            if (!chunk || !chunk->ShouldRender() || chunk->GetIndicesCount(drawGroup) == 0)
            {
                continue;
            }
            context->IASetVertexBuffers(0u, 1u, chunk->GetVertexBuffer(drawGroup).GetAddressOf(), &m_stride, &m_offset);
            context->IASetIndexBuffer(chunk->GetIndexBuffer(drawGroup).Get(), DXGI_FORMAT_R32_UINT, 0u);
            context->DrawIndexed(static_cast<UINT>(chunk->GetIndicesCount(drawGroup)), 0u, 0u);
        }
    }
}

void ChunkRenderer::BuildInputLayout()
{
    D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "LIGHT", 0, DXGI_FORMAT_R16_UINT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    auto vertexShaderBlob = ShadersLoader::LoadBlob(L"VertexShader.cso");
    m_deviceResources->GetD3DDevice()->CreateInputLayout(inputLayoutDesc,
        ARRAYSIZE(inputLayoutDesc),
        vertexShaderBlob->GetBufferPointer(),
        vertexShaderBlob->GetBufferSize(),
        &m_inputLayout);
}

void ChunkRenderer::BuildSampler()
{
    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    m_deviceResources->GetD3DDevice()->CreateSamplerState(&samplerDesc, &m_sampler);
}

void ChunkRenderer::BuildBlendState()
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

    m_deviceResources->GetD3DDevice()->CreateBlendState(&blendDesc, &m_blendState);
}

void ChunkRenderer::BuildRasterState()
{
    D3D11_RASTERIZER_DESC rasterizerDesc;
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_BACK;
    rasterizerDesc.FrontCounterClockwise = false;
    rasterizerDesc.DepthBias = 0;
    rasterizerDesc.DepthBiasClamp = 0.0f;
    rasterizerDesc.SlopeScaledDepthBias = 0.0f;
    rasterizerDesc.DepthClipEnable = true;
    rasterizerDesc.ScissorEnable = false;
    rasterizerDesc.MultisampleEnable = true;
    rasterizerDesc.AntialiasedLineEnable = true; // ��������� �������

    m_deviceResources->GetD3DDevice()->CreateRasterizerState(&rasterizerDesc, &m_rasterState);
}

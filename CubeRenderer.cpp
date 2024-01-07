#include "pch.h"
#include "CubeRenderer.h"
#include "ShadersLoader.h"

CubeRenderer::CubeRenderer(ID3D11Device* device)
{
    BuildInputLayout(device);
    BuildVerticesAndIndices();
    BuildVertexBuffer(device);
    BuildIndexBuffer(device);
    BuildSampler(device);
    BuildConstantBuffer(device);
    BuildBlendState(device);
    m_texManager = std::make_unique<TexturesManager>(device);
    m_vertexShader = ShadersLoader::LoadVertexShader(device, L"VertexShader.cso");
    m_pixelShader = ShadersLoader::LoadPixelShader(device, L"PixelShader.cso");
}

void CubeRenderer::DrawCubes(ID3D11DeviceContext1* context, std::vector<std::unique_ptr<Cube>>& cubes)
{
    context->IASetVertexBuffers(0u, 1u, m_vertexBuffer.GetAddressOf(), &m_stride, &m_offset);
    context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
    context->PSSetShader(m_pixelShader.Get(), nullptr, 0u);
    context->VSSetShader(m_vertexShader.Get(), nullptr, 0u);
    context->IASetInputLayout(m_inputLayout.Get());
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->PSSetSamplers(0u, 1u, m_sampler.GetAddressOf());
    context->OMSetBlendState(m_blendState.Get(), nullptr, 0xFFFFFFFF);

    for (auto& cube : cubes)
    {
        m_activeCB = cube->Pos();
        UpdateWorldConstantBuffer(context);
        context->VSSetConstantBuffers(0u, 1u, m_constantBuffer.GetAddressOf());
        context->PSSetShaderResources(0u, 1u, m_texManager->GetBlockTexture(cube->Id()).GetAddressOf());
        context->DrawIndexed(36u, 0u, 0u);
    }
}

void CubeRenderer::DrawChunk(ID3D11DeviceContext1* context, Chunk* chunk, TextureAtlas textureAtlas)
{
    context->IASetVertexBuffers(0u, 1u, m_vertexBuffer.GetAddressOf(), &m_stride, &m_offset);
    context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
    context->PSSetShader(m_pixelShader.Get(), nullptr, 0u);
    context->VSSetShader(m_vertexShader.Get(), nullptr, 0u);
    context->IASetInputLayout(m_inputLayout.Get());
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->PSSetSamplers(0u, 1u, m_sampler.GetAddressOf());
    context->OMSetBlendState(m_blendState.Get(), nullptr, 0xFFFFFFFF);

    auto& blocks = chunk->GetBlocks();

    for (auto& block : blocks)
    {
        m_activeCB = block->Pos();
        UpdateWorldConstantBuffer(context);
        context->VSSetConstantBuffers(0u, 1u, m_constantBuffer.GetAddressOf());
        context->PSSetShaderResources(0u, 1u, textureAtlas.GetAtlasSRV().GetAddressOf());
        context->DrawIndexed(36u, 0u, 0u);
    }
}

void CubeRenderer::BuildInputLayout(ID3D11Device* device)
{
    D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    auto vertexShaderBlob = ShadersLoader::LoadBlob(L"VertexShader.cso");
    device->CreateInputLayout(inputLayoutDesc,
        ARRAYSIZE(inputLayoutDesc),
        vertexShaderBlob->GetBufferPointer(),
        vertexShaderBlob->GetBufferSize(),
        &m_inputLayout);
}

void CubeRenderer::BuildVerticesAndIndices()
{
    DirectX::GeometricPrimitive::CreateCube(vertices, indices, 1.0f, false);

    // Back
    vertices[0].textureCoordinate = { 0.75f, 1.0f / 3.0f };
    vertices[1].textureCoordinate = { 0.75f, 2.0f / 3.0f };
    vertices[2].textureCoordinate = { 1.0f, 2.0f / 3.0f };
    vertices[3].textureCoordinate = { 1.0f, 1.0f / 3.0f };
    // Front
    vertices[4].textureCoordinate = { 0.25f, 1.0f / 3.0f };
    vertices[5].textureCoordinate = { 0.25f, 2.0f / 3.0f };
    vertices[6].textureCoordinate = { 0.5f, 2.0f / 3.0f };
    vertices[7].textureCoordinate = { 0.5f, 1.0f / 3.0f };
    // Right
    vertices[8].textureCoordinate = { 0.5f, 1.0f / 3.0f };
    vertices[9].textureCoordinate = { 0.5f, 2.0f / 3.0f };
    vertices[10].textureCoordinate = { 0.75f, 2.0f / 3.0f };
    vertices[11].textureCoordinate = { 0.75f, 1.0f / 3.0f };
    // Left
    vertices[12].textureCoordinate = { 0.0f, 1.0f / 3.0f };
    vertices[13].textureCoordinate = { 0.0f, 2.0f / 3.0f };
    vertices[14].textureCoordinate = { 0.25f, 2.0f / 3.0f };
    vertices[15].textureCoordinate = { 0.25f, 1.0f / 3.0f };
    // Top
    vertices[16].textureCoordinate = { 0.25f, 0.0f };
    vertices[17].textureCoordinate = { 0.25f, 1.0f / 3.0f };
    vertices[18].textureCoordinate = { 0.5f, 1.0f / 3.0f };
    vertices[19].textureCoordinate = { 0.5f, 0.0f };
    // Bottom
    vertices[20].textureCoordinate = { 0.25f, 2.0f / 3.0f };
    vertices[21].textureCoordinate = { 0.25f, 1.0f };
    vertices[22].textureCoordinate = { 0.5f, 1.0f };
    vertices[23].textureCoordinate = { 0.5f, 2.0f / 3.0f };
}

void CubeRenderer::BuildVertexBuffer(ID3D11Device* device)
{
    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.CPUAccessFlags = 0u;
    bd.MiscFlags = 0u;
    bd.ByteWidth = static_cast<UINT>(sizeof(DirectX::GeometricPrimitive::VertexType) * vertices.size());
    bd.StructureByteStride = sizeof(DirectX::GeometricPrimitive::VertexType);
    D3D11_SUBRESOURCE_DATA sd = {};
    sd.pSysMem = vertices.data();
    device->CreateBuffer(&bd, &sd, &m_vertexBuffer);
}

void CubeRenderer::BuildIndexBuffer(ID3D11Device* device)
{
    D3D11_BUFFER_DESC ibd = {};
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.CPUAccessFlags = 0u;
    ibd.MiscFlags = 0u;
    ibd.ByteWidth = static_cast<UINT>(sizeof(uint16_t) * indices.size());
    ibd.StructureByteStride = sizeof(uint16_t);
    D3D11_SUBRESOURCE_DATA isd = {};
    isd.pSysMem = indices.data();
    device->CreateBuffer(&ibd, &isd, &m_indexBuffer);
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

void CubeRenderer::BuildConstantBuffer(ID3D11Device* device)
{
    D3D11_BUFFER_DESC cbd;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbd.MiscFlags = 0u;
    cbd.ByteWidth = m_cbSize;
    cbd.StructureByteStride = 0u;
    D3D11_SUBRESOURCE_DATA csd = {};
    csd.pSysMem = &m_activeCB;
    device->CreateBuffer(&cbd, &csd, &m_constantBuffer);
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

void CubeRenderer::UpdateWorldConstantBuffer(ID3D11DeviceContext1* context)
{
    D3D11_MAPPED_SUBRESOURCE msr;
    context->Map(
        m_constantBuffer.Get(), 0u,
        D3D11_MAP_WRITE_DISCARD, 0u,
        &msr
    );
    memcpy(msr.pData, &m_activeCB, m_cbSize);
    context->Unmap(m_constantBuffer.Get(), 0u);
}

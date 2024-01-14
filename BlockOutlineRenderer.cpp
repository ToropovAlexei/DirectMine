#include "pch.h"
#include "BlockOutlineRenderer.h"
#include "ShadersLoader.h"

BlockOutlineRenderer::BlockOutlineRenderer(std::unique_ptr<DX::DeviceResources>& deviceResources) :
	m_deviceResources(deviceResources),
    m_worldPos(std::nullopt)
{
    BuildInputLayout();
    BuildIndices();
    m_vertexShader = ShadersLoader::LoadVertexShader(deviceResources->GetD3DDevice(), L"BlockOutlineVS.cso");
    m_pixelShader = ShadersLoader::LoadPixelShader(deviceResources->GetD3DDevice(), L"BlockOutlinePS.cso");
    m_geometryShader = ShadersLoader::LoadGeometryShader(deviceResources->GetD3DDevice(), L"BlockOutlineGS.cso");
}

void BlockOutlineRenderer::RenderCubeOutline()
{
    if (m_worldPos == std::nullopt || m_vertices.empty())
    {
        return;
    }
    assert(m_vertices.size());
    auto context = m_deviceResources->GetD3DDeviceContext();
    context->IASetVertexBuffers(0u, 1u, m_vertexBuffer.GetAddressOf(), &m_stride, &m_offset);
    context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0u);
    context->PSSetShader(m_pixelShader.Get(), nullptr, 0u);
    context->VSSetShader(m_vertexShader.Get(), nullptr, 0u);
    context->IASetInputLayout(m_inputLayout.Get());
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    context->DrawIndexed(static_cast<UINT>(m_indices.size()), 0u, 0u);
}

void BlockOutlineRenderer::UpdateOutlinedCube(std::optional<WorldPos>& worldPos)
{
    m_worldPos = worldPos;
    if (worldPos.has_value())
    {
        UpdateVertices();
    }
}

void BlockOutlineRenderer::BuildIndices()
{
    m_indices = {
        0, 1,
        0, 2,
        0, 4,
        1, 3,
        1, 5,
        2, 6,
        3, 2,
        3, 7,
        4, 5,
        4, 6,
        5, 7,
        6, 7,
    };

    UpdateIndexBuffer();
}

void BlockOutlineRenderer::UpdateIndexBuffer()
{
    D3D11_BUFFER_DESC ibd = {};
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.CPUAccessFlags = 0u;
    ibd.MiscFlags = 0u;
    ibd.ByteWidth = static_cast<UINT>(sizeof(UINT) * m_indices.size());
    ibd.StructureByteStride = sizeof(UINT);
    D3D11_SUBRESOURCE_DATA isd = {};
    isd.pSysMem = m_indices.data();
    m_deviceResources->GetD3DDevice()->CreateBuffer(&ibd, &isd, &m_indexBuffer);
}

void BlockOutlineRenderer::UpdateVertices()
{
    if (m_worldPos == std::nullopt)
    {
        return;
    }
    auto& worldPos = m_worldPos.value();
    m_vertices = {
        OutlineVertex({ -0.005f + worldPos.x, -0.005f + worldPos.y, -0.005f + worldPos.z }),
        OutlineVertex({ -0.005f + worldPos.x, 1.005f + worldPos.y, -0.005f + worldPos.z }),
        OutlineVertex({ -0.005f + worldPos.x, -0.005f + worldPos.y, 1.005f + worldPos.z }),
        OutlineVertex({ -0.005f + worldPos.x, 1.005f + worldPos.y, 1.005f + worldPos.z }),
        OutlineVertex({ 1.005f + worldPos.x, -0.005f + worldPos.y, -0.005f + worldPos.z }),
        OutlineVertex({ 1.005f + worldPos.x, 1.005f + worldPos.y, -0.005f + worldPos.z }),
        OutlineVertex({ 1.005f + worldPos.x, -0.005f + worldPos.y, 1.005f + worldPos.z }),
        OutlineVertex({ 1.005f + worldPos.x, 1.005f + worldPos.y, 1.005f + worldPos.z }),
    };
    UpdateVertexBuffer();
}

void BlockOutlineRenderer::UpdateVertexBuffer()
{
    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.CPUAccessFlags = 0u;
    bd.MiscFlags = 0u;
    bd.ByteWidth = static_cast<UINT>(sizeof(OutlineVertex) * m_vertices.size());
    bd.StructureByteStride = sizeof(OutlineVertex);
    D3D11_SUBRESOURCE_DATA sd = {};
    sd.pSysMem = m_vertices.data();
    m_deviceResources->GetD3DDevice()->CreateBuffer(&bd, &sd, &m_vertexBuffer);
}

void BlockOutlineRenderer::BuildInputLayout()
{
    D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    auto vertexShaderBlob = ShadersLoader::LoadBlob(L"BlockOutlineVS.cso");
    m_deviceResources->GetD3DDevice()->CreateInputLayout(inputLayoutDesc,
        ARRAYSIZE(inputLayoutDesc),
        vertexShaderBlob->GetBufferPointer(),
        vertexShaderBlob->GetBufferSize(),
        &m_inputLayout);
}

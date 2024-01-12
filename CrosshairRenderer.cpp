#include "pch.h"
#include "CrosshairRenderer.h"
#include "ShadersLoader.h"

CrosshairRenderer::CrosshairRenderer(std::unique_ptr<DX::DeviceResources>& deviceResources, float aspectRatio) :
	m_deviceResources(deviceResources),
    m_aspectRatio(aspectRatio)
{
    m_vertexShader = ShadersLoader::LoadVertexShader(deviceResources->GetD3DDevice(), L"CrosshairVS.cso");
    m_pixelShader = ShadersLoader::LoadPixelShader(deviceResources->GetD3DDevice(), L"CrosshairPS.cso");
    BuildVertices();
    BuildVertexBuffer();
    BuildInputLayout();
}

void CrosshairRenderer::Render()
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    context->IASetInputLayout(m_inputLayout.Get());
    context->GSSetShader(nullptr, nullptr, 0u);
    context->VSSetShader(m_vertexShader.Get(), nullptr, 0u);
    context->PSSetShader(m_pixelShader.Get(), nullptr, 0u);
    context->RSSetState(nullptr);
    context->IASetVertexBuffers(0u, 1u, m_vertexBuffer.GetAddressOf(), &stride, &offset);
    context->Draw(static_cast<UINT>(m_vertices.size()), 0u);
}

void CrosshairRenderer::UpdateAspectRatio(float aspectRatio)
{
    m_aspectRatio = aspectRatio;
    BuildVertices();
    BuildVertexBuffer();
}

void CrosshairRenderer::BuildVertices()
{
    m_vertices = {
        CrosshairVertex({ -scale, 0.0f, 0.0f }),
        CrosshairVertex({ scale, 0.0f, 0.0f }),
        CrosshairVertex({ 0.0f, -scale * m_aspectRatio, 0.0f }),
        CrosshairVertex({ 0.0f, scale * m_aspectRatio, 0.0f }),
    };
}

void CrosshairRenderer::BuildInputLayout()
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

void CrosshairRenderer::BuildVertexBuffer()
{
    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.CPUAccessFlags = 0u;
    bd.MiscFlags = 0u;
    bd.ByteWidth = static_cast<UINT>(sizeof(CrosshairVertex) * m_vertices.size());
    bd.StructureByteStride = sizeof(CrosshairVertex);
    D3D11_SUBRESOURCE_DATA sd = {};
    sd.pSysMem = m_vertices.data();
    m_deviceResources->GetD3DDevice()->CreateBuffer(&bd, &sd, &m_vertexBuffer);
}

#pragma once

#include "DeviceResources.h"

struct CrosshairVertex
{
	DirectX::XMFLOAT3 pos;
};

class CrosshairRenderer
{
public:
	CrosshairRenderer(std::unique_ptr<DX::DeviceResources>& deviceResources, float aspectRatio);

	void Render();
	void UpdateAspectRatio(float aspectRatio);

private:
	void BuildVertices();
	void BuildInputLayout();
	void BuildVertexBuffer();

private:
	std::unique_ptr<DX::DeviceResources>& m_deviceResources;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
	std::vector<CrosshairVertex> m_vertices;
	float m_aspectRatio = 1.0f;
	static constexpr float scale = 0.01f;

	UINT stride = sizeof(CrosshairVertex);
	UINT offset = 0;
};


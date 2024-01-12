#pragma once

#include "DeviceResources.h"
#include "WorldPos.hpp"

struct OutlineVertex
{
	DirectX::XMFLOAT3 pos;
};

class BlockOutlineRenderer
{
public:
	BlockOutlineRenderer(std::unique_ptr<DX::DeviceResources>& deviceResources);

	void RenderCubeOutline();
	void UpdateOutlinedCube(std::optional<WorldPos>& worldPos);

private:
	void BuildIndices();
	void UpdateIndexBuffer();

	void UpdateVertices();

	void UpdateVertexBuffer();
	void BuildInputLayout();

private:
	std::optional<WorldPos> m_worldPos;
	std::unique_ptr<DX::DeviceResources>& m_deviceResources;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_geometryShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
	std::vector<OutlineVertex> m_vertices;
	std::vector<UINT> m_indices;
	UINT m_stride = sizeof(OutlineVertex);
	UINT m_offset = 0u;
};


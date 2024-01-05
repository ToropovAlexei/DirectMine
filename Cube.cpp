#include "pch.h"
#include "Cube.h"

Cube::Cube(DirectX::XMFLOAT3 pos) :
	m_pos(pos)
{
}

DirectX::XMFLOAT3 Cube::Pos() const noexcept
{
	return m_pos;
}

CubeId Cube::Id() const noexcept
{
	return CubeId::Air;
}

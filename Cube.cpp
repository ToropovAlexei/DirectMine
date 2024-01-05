#include "pch.h"
#include "Cube.h"

Cube::Cube(DirectX::XMMATRIX world) :
	m_world(world)
{
}

DirectX::XMMATRIX Cube::World() const noexcept
{
	return m_world;
}

CubeId Cube::Id() const noexcept
{
	throw std::runtime_error("Not implemented");
}

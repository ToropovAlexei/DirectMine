#pragma once

enum class CubeId
{
	Debug,
	Air,
	Dirt,
	Grass,
	Leaves,
	Badrock,
	Cobblestone,
	Stone
};

class Cube
{
public:
	Cube(DirectX::XMMATRIX world);

	DirectX::XMMATRIX World() const noexcept;

	virtual CubeId Id() const noexcept;

protected:
	DirectX::XMMATRIX m_world;
};


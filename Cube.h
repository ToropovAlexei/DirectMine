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
	Cube(DirectX::XMFLOAT3 pos);

	DirectX::XMFLOAT3 Pos() const noexcept;

	virtual CubeId Id() const noexcept;

protected:
	DirectX::XMFLOAT3 m_pos;
};


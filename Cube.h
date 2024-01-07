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
	Cube();

	virtual CubeId Id() const noexcept;
};


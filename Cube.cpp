#include "pch.h"
#include "Cube.h"

Cube::Cube()
{
}

CubeId Cube::Id() const noexcept
{
	return CubeId::Air;
}

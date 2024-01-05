#include "pch.h"
#include "GrassBlock.h"

GrassBlock::GrassBlock(DirectX::XMMATRIX world) :
	Cube(world)
{
}

CubeId GrassBlock::Id() const noexcept
{
	return CubeId::Grass;
}

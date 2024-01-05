#include "pch.h"
#include "GrassBlock.h"

GrassBlock::GrassBlock(DirectX::XMFLOAT3 pos) :
	Cube(pos)
{
}

CubeId GrassBlock::Id() const noexcept
{
	return CubeId::Grass;
}

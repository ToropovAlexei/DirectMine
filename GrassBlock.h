#pragma once
#include "Cube.h"

class GrassBlock :
    public Cube
{
public:
    GrassBlock(DirectX::XMFLOAT3 pos);

    CubeId Id() const noexcept;
};


#pragma once
#include "Cube.h"

class GrassBlock :
    public Cube
{
public:
    GrassBlock(DirectX::XMMATRIX world);

    CubeId Id() const noexcept;
};


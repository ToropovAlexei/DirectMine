#pragma once
#include "Cube.h"

class LeavesBlock :
    public Cube
{
public:
    LeavesBlock(DirectX::XMMATRIX world);

    CubeId Id() const noexcept;
};


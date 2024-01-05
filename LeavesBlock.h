#pragma once
#include "Cube.h"

class LeavesBlock :
    public Cube
{
public:
    LeavesBlock(DirectX::XMFLOAT3 pos);

    CubeId Id() const noexcept;
};


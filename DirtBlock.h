#pragma once
#include "Cube.h"

class DirtBlock :
    public Cube
{
public:
    DirtBlock(DirectX::XMFLOAT3 pos);

    CubeId Id() const noexcept;
};


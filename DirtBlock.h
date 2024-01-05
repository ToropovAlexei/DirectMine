#pragma once
#include "Cube.h"

class DirtBlock :
    public Cube
{
public:
    DirtBlock(DirectX::XMMATRIX world);

    CubeId Id() const noexcept;
};


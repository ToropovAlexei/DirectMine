#pragma once
#include "Cube.h"

class DebugBlock :
    public Cube
{
public:
    DebugBlock(DirectX::XMFLOAT3 pos);
    
    CubeId Id() const noexcept;
};


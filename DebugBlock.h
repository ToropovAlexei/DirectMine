#pragma once
#include "Cube.h"

class DebugBlock :
    public Cube
{
public:
    DebugBlock(DirectX::XMMATRIX world);
    
    CubeId Id() const noexcept;
};


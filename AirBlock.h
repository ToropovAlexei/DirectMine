#pragma once
#include "Cube.h"

class AirBlock :
    public Cube
{
protected:
    static const CubeId id = CubeId::Air;
};


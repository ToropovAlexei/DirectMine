#include "pch.h"
#include "MathUtils.h"

int MathUtils::GetNearestSquareNumber(int number)
{
    int squareRoot = std::sqrt(number);
    int nearestSquareNumber = squareRoot * squareRoot;
    if (nearestSquareNumber < number)
    {
        ++squareRoot;
    }
    return squareRoot;
}

unsigned int MathUtils::GetNextPowerOfTwo(unsigned int number)
{
    --number;
    number |= number >> 1;
    number |= number >> 2;
    number |= number >> 4;
    number |= number >> 8;
    number |= number >> 16;
    ++number;
    return number;
}

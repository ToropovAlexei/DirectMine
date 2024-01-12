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

int MathUtils::RoundUp(int numToRound, int multiple)
{
    int remainder = abs(numToRound) % multiple;
    if (remainder == 0)
    {
        return numToRound;
    }
    if (numToRound < 0)
    {
        return -(abs(numToRound) - remainder);
    }

    return numToRound + multiple - remainder;
}

int MathUtils::RoundDown(int numToRound, int multiple)
{
    int remainder = abs(numToRound) % multiple;
    if (remainder == 0)
    {
        return numToRound;
    }
    if (numToRound < 0)
    {
        return -(abs(numToRound) + remainder);
    }

    return numToRound - remainder;
}

#pragma once
class MathUtils
{
public:
    static int GetNearestSquareNumber(int number);

    static unsigned int GetNextPowerOfTwo(unsigned int number);

    static int RoundUp(int numToRound, int multiple);

    static int RoundDown(int numToRound, int multiple);
};


#pragma once

#include <cmath>

class MathUtils
{
public:
    static inline int GetNearestSquareNumber(int number) noexcept
    {
        int squareRoot = static_cast<int>(std::sqrt(number));
        int nearestSquareNumber = squareRoot * squareRoot;
        if (nearestSquareNumber < number)
        {
            ++squareRoot;
        }
        return squareRoot;
    }

    static inline unsigned int GetNextPowerOfTwo(unsigned int number) noexcept
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

    static inline int RoundUp(int numToRound, int multiple) noexcept
    {
        int remainder = std::abs(numToRound) % multiple;
        if (remainder == 0)
        {
            return numToRound;
        }
        if (numToRound < 0)
        {
            return -(std::abs(numToRound) - remainder);
        }

        return numToRound + multiple - remainder;
    }

    static inline int RoundDown(int numToRound, int multiple) noexcept
    {
        int remainder = std::abs(numToRound) % multiple;
        if (remainder == 0)
        {
            return numToRound;
        }
        if (numToRound < 0)
        {
            return -(std::abs(numToRound) + multiple - remainder);
        }

        return numToRound - remainder;
    }
};


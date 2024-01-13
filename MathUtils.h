#pragma once
class MathUtils
{
public:
    static inline const int GetNearestSquareNumber(int number) noexcept
    {
        int squareRoot = static_cast<int>(std::sqrt(number));
        int nearestSquareNumber = squareRoot * squareRoot;
        if (nearestSquareNumber < number)
        {
            ++squareRoot;
        }
        return squareRoot;
    }

    static inline const unsigned int GetNextPowerOfTwo(unsigned int number) noexcept
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

    static inline const int RoundUp(int numToRound, int multiple) noexcept
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

    static inline const int RoundDown(int numToRound, int multiple) noexcept
    {
        int remainder = abs(numToRound) % multiple;
        if (remainder == 0)
        {
            return numToRound;
        }
        if (numToRound < 0)
        {
            return -(abs(numToRound) + multiple - remainder);
        }

        return numToRound - remainder;
    }
};



#pragma once

#include <iomanip>
#include <array>

template <size_t N>
class myClass
{
private:
    std::array<std::uint8_t, N>& localArray;

public:
    myClass(std::array<std::uint8_t, N>& p) : localArray(p)
    {
    }

    void myFunc()
    {
        this->localArray[0] = 9;
        std::cout << "array contains " << this->localArray.size() << "elements " << +this->localArray[0];
        std::cout << std::endl;
    }
};
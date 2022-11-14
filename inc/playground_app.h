
#pragma once

#include <iostream>
#include <iomanip>
#include <array>
#include <cstddef>

template <std::size_t N>
class myClass
{
private:
    //std::array<std::uint8_t, N>& localArray;

public:
    myClass();

    template <typename T>
    void myFunc();
};

template <std::size_t N>
class myDeClass : public myClass<N>
{
private:
    //std::array<std::uint8_t, N>& localArray;

public:
    myDeClass(std::array<std::uint8_t, N>& p);

    template <typename T>
    void myDeFunc();
};

#include "playground_app.tpp"
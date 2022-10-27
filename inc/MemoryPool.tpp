/**
 * @file MemoryPool.tpp
 * @author Umberto Cancedda ()
 * @brief Implementation of memory pool library functions
 * @version 0.1
 * @date 2022-10-27
 *
 * @copyright Copyright (c) 2022
 *
 */

// Std-Includes
#include <iostream>
#include <limits>

// Own Includes

std::uint8_t* getRefPageStartAddress()
{
    if (refMemPool == nullptr)
    {
        return nullptr;
    }
    else
    {
        return refMemPool->getPoolStartAddress();
    }
}

std::uint8_t* getWrkPageStartAddress()
{
    if (wrkMemPool == nullptr)
    {
        return nullptr;
    }
    else
    {
        return wrkMemPool->getPoolStartAddress();
    }
}

std::uint8_t* getMeasVarStartAddress()
{
    if (measMemPool == nullptr)
    {
        return nullptr;
    }
    else
    {
        return measMemPool->getPoolStartAddress();
    }
}

// implementation of MemoryPool
MemoryPool::MemoryPool(MemoryPoolType type) : currentMemSize(0), poolMemory(nullptr)
{
    switch (type)
    {
        case MemoryPoolType::referencePage:
        {
            this->poolMemory = refPageMem;
            break;
        }
        case MemoryPoolType::workingPage:
        {
            this->poolMemory = wrkPageMem;
            break;
        }
        case MemoryPoolType::measurementMemory:
        {
            this->poolMemory = measMem;
        }
    }
}

std::uint8_t* MemoryPool::getPoolStartAddress()
{
    return this->poolMemory;
}

template <class T>
T* MemoryPool::addVariable(T testVar)
{
    //TODO: Check for available Memory !!!!

    T* ptrToPoolMemory = reinterpret_cast<T*>(this->poolMemory + this->currentMemSize);
    this->currentMemSize = this->currentMemSize + sizeof(testVar);

    std::cout << "Allocated variable of size " << sizeof(testVar) << " @0x" << static_cast<void*>(ptrToPoolMemory) << std::endl;

    return ptrToPoolMemory;
}

template <class T>
void memPoolVariable<T>::set(T value)
{
    *(this->valPtr) = value;
}

template <class T>
T memPoolVariable<T>::get()
{
    return *(this->valPtr);
}

template <class T>
std::uint32_t calibratable<T>::numVars(0);

// implementation of calibratable
template <class T>
calibratable<T>::calibratable()
{
    if (refMemPool == nullptr)
    {
        refMemPool = new MemoryPool(MemoryPoolType::referencePage);
    }

    std::cout << "Var-Type: " << typeid(T).name() << std::endl;

    T temp(0);

    this->valPtr = refMemPool->addVariable(temp);

    *(this->valPtr) = 0;
}

template <class T>
calibratable<T>::~calibratable()
{
}

// // initialization of measurable static members
// template <class T>
// MemoryPool* measurable<T>::measMemPool = nullptr;

// implementation of measurable
template <class T>
measurable<T>::measurable()
{
    if (measMemPool == nullptr)
    {
        measMemPool = new MemoryPool(MemoryPoolType::measurementMemory);
    }

    std::cout << "Var-Type: " << typeid(this->valPtr).name() << std::endl;
    T temp;

    this->valPtr = measMemPool->addVariable(temp);

    this->valPtr = 0;
}

template <class T>
measurable<T>::~measurable()
{
    this->valPtr = std::numeric_limits<T>::max();
}

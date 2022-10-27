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
    if (refMemoryPool == nullptr)
    {
        return nullptr;
    }
    else
    {
        return refMemoryPool->getPoolStartAddress();
    }
}

std::uint32_t getRefPageNumVariables()
{
    if (refMemoryPool == nullptr)
    {
        return 0;
    }
    else
    {
        return refMemoryPool->getPoolNumVariables();
    }
}

std::uint32_t getRefPageTotalSize()
{
    if (refMemoryPool == nullptr)
    {
        return 0;
    }
    else
    {
        return refMemoryPool->getPoolTotalSize();
    }
}

std::uint32_t getRefPageCurrentSize()
{
    if (refMemoryPool == nullptr)
    {
        return 0;
    }
    else
    {
        return refMemoryPool->getPoolCurrentSize();
    }
}

std::uint8_t* getWrkPageStartAddress()
{
    if (wrkMemoryPool == nullptr)
    {
        return nullptr;
    }
    else
    {
        return wrkMemoryPool->getPoolStartAddress();
    }
}

std::uint32_t getWrkPageNumVariables()
{
    if (wrkMemoryPool == nullptr)
    {
        return 0;
    }
    else
    {
        return wrkMemoryPool->getPoolNumVariables();
    }
}

std::uint32_t getWrkPageTotalSize()
{
    if (wrkMemoryPool == nullptr)
    {
        return 0;
    }
    else
    {
        return wrkMemoryPool->getPoolTotalSize();
    }
}

std::uint32_t getWrkPageCurrentSize()
{
    if (wrkMemoryPool == nullptr)
    {
        return 0;
    }
    else
    {
        return wrkMemoryPool->getPoolCurrentSize();
    }
}

std::uint8_t* getMeasPageStartAddress()
{
    if (measMemoryPool == nullptr)
    {
        return nullptr;
    }
    else
    {
        return measMemoryPool->getPoolStartAddress();
    }
}

std::uint32_t getMeasPageNumVariables()
{
    if (measMemoryPool == nullptr)
    {
        return 0;
    }
    else
    {
        return measMemoryPool->getPoolNumVariables();
    }
}

std::uint32_t getMeasPageTotalSize()
{
    if (measMemoryPool == nullptr)
    {
        return 0;
    }
    else
    {
        return measMemoryPool->getPoolTotalSize();
    }
}

std::uint32_t getMeasPageCurrentSize()
{
    if (measMemoryPool == nullptr)
    {
        return 0;
    }
    else
    {
        return measMemoryPool->getPoolCurrentSize();
    }
}

// implementation of MemoryPool
MemoryPool::MemoryPool(MemoryPoolType type) : usedMemPoolSize(0), numVariablesInMemPool(0), poolMemory(nullptr)
{
    switch (type)
    {
        case MemoryPoolType::referencePage:
        {
            this->poolMemory = refPageMemory;
            this->totalMemPoolSize = CAL_MEM_POOL_SIZE;
            break;
        }
        case MemoryPoolType::workingPage:
        {
            this->poolMemory = wrkPageMemory;
            this->totalMemPoolSize = CAL_MEM_POOL_SIZE;
            break;
        }
        case MemoryPoolType::measurementPage:
        {
            this->poolMemory = measPageMemory;
            this->totalMemPoolSize = MEAS_MEM_POOL_SIZE;
        }
    }
}

std::uint8_t* MemoryPool::getPoolStartAddress()
{
    return this->poolMemory;
}

std::uint32_t MemoryPool::getPoolNumVariables()
{
    return this->numVariablesInMemPool;
}

std::uint32_t MemoryPool::getPoolTotalSize()
{
    return this->totalMemPoolSize;
}

std::uint32_t MemoryPool::getPoolCurrentSize()
{
    return this->usedMemPoolSize;
}

template <class T>
T* MemoryPool::addVariable(T testVar)
{
    if ((this->usedMemPoolSize + sizeof(testVar)) > this->totalMemPoolSize)
    {
        return nullptr;
    }
    else
    {
        T* ptrToPoolMemory = reinterpret_cast<T*>(this->poolMemory + this->usedMemPoolSize);

        std::string varName = applicationName + "." + __FUNCTION__ + "." + std::to_string(this->usedMemPoolSize);
        VarIdentifier varId;
        varId.type = typeid(T).name();
        varId.size = sizeof(testVar);
        varId.relativeAddressOffset = this->usedMemPoolSize;

        this->variableList.insert(std::pair<std::string, VarIdentifier>(varName, varId));

        this->usedMemPoolSize = this->usedMemPoolSize + sizeof(testVar);
        this->numVariablesInMemPool++;

        std::cout << "Allocated variable of size " << sizeof(testVar) << " @0x" << static_cast<void*>(ptrToPoolMemory) << std::endl;

        return ptrToPoolMemory;
    }
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
    if (refMemoryPool == nullptr)
    {
        refMemoryPool = new MemoryPool(MemoryPoolType::referencePage);
    }

    std::cout << "Var-Type: " << typeid(T).name() << std::endl;

    T temp(0);

    this->valPtr = refMemoryPool->addVariable(temp);

    *(this->valPtr) = 0;
}

template <class T>
calibratable<T>::~calibratable()
{
}

// implementation of measurable
template <class T>
measurable<T>::measurable()
{
    if (measMemoryPool == nullptr)
    {
        measMemoryPool = new MemoryPool(MemoryPoolType::measurementPage);
    }

    std::cout << "Var-Type: " << typeid(this->valPtr).name() << std::endl;
    T temp;

    this->valPtr = measMemoryPool->addVariable(temp);

    this->valPtr = 0;
}

template <class T>
measurable<T>::~measurable()
{
    this->valPtr = std::numeric_limits<T>::max();
}

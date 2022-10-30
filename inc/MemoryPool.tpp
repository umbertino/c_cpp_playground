/**
 * @file MemoryPool.tpp
 * @author Umberto Cancedda
 * @brief Implementation of memory pool library functions
 * @version 0.1
 * @date 2022-10-27
 *
 * @copyright Copyright (c)2022
 *
 */

// Std-Includes
#include <iostream>
#include <iomanip>
#include <limits>

// Own Includes

// implementation of MemoryPool
MemoryPool::MemoryPool(MemoryPoolType type) : usedMemPoolSize(0), numVariablesInMemPool(0), poolMemoryStartAddress(nullptr)
{
    switch (type)
    {
        case MemoryPoolType::referencePage:
        {
            this->poolMemoryStartAddress = refPageMemory;
            this->totalMemPoolSize = CAL_MEM_POOL_SIZE;
            break;
        }
        case MemoryPoolType::workingPage:
        {
            this->poolMemoryStartAddress = wrkPageMemory;
            this->totalMemPoolSize = CAL_MEM_POOL_SIZE;
            break;
        }
        case MemoryPoolType::measurementPage:
        {
            this->poolMemoryStartAddress = measPageMemory;
            this->totalMemPoolSize = MEAS_MEM_POOL_SIZE;
        }
    }
}

std::uint8_t* MemoryPool::getPoolStartAddress()
{
    return this->poolMemoryStartAddress;
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

bool MemoryPool::dumpListOfvariables()
{
    if (this->variableList.empty())
    {
        return false;
    }
    else
    {
        for (auto const var : this->variableList)
        {
            std::string category = (var.second.category == VariableCategory::calibration) ? "calibration" : "measurement";
            std::cout << std::setfill(' ') << std::setw(32) << std::left << var.first << "\t" << std::setw(18) << var.second.type << "\t" << +var.second.size << " Byte"
                      << "\t"
                      << "Offset: " << std::right << std::setw(8) << var.second.relativeAddressOffset
                      << "\t" << category << std::endl;
        }

        return true;
    }
}

bool MemoryPool::dumpPoolMemory()
{
    if (this->variableList.empty())
    {
        return false;
    }
    else
    {
        std::uint32_t lineCounter = 0;
        const std::uint8_t elementsPerLine = 8;
        const std::uint8_t spaceAfter = 4;

        // iterate over the number of used bytes in the memory pool
        for (std::uint32_t i = 0; i < this->usedMemPoolSize; i++)
        {
            if (i % elementsPerLine == 0)
            {
                // print address at beginning of each line
                std::cout << std::hex << std::setfill('0') << std::setw(16) << i << "\t";
                lineCounter++;
            }

            if (i % spaceAfter == 0)
            {
                // add an extra space after number of elements defined by 'paceAfter'
                std::cout << " ";
            }

            std::cout << std::setw(2) << +poolMemoryStartAddress[i] << " ";

            if (i == (lineCounter * elementsPerLine - 1))
            {
                // add a carriage return after number of elements defined by 'elementsPerLine'
                std::cout << std::endl;
            }
        }

        std::cout << std::endl;

        return true;
    }
}

template <class T>
T* MemoryPool::addVariable(T testVar, std::string label, VariableCategory category)
{
    if ((this->usedMemPoolSize + sizeof(testVar)) > this->totalMemPoolSize)
    {
        return nullptr;
    }
    else
    {
        T* ptrToPoolMemory = reinterpret_cast<T*>(this->poolMemoryStartAddress + this->usedMemPoolSize);

        std::string varName = applicationName + "." + label;
        VariableIdentifier varId;
        varId.type = typeid(T).name();
        varId.size = sizeof(testVar);
        varId.relativeAddressOffset = this->usedMemPoolSize;
        varId.category = category;

        this->variableList.insert(std::pair<std::string, VariableIdentifier>(varName, varId));

        this->usedMemPoolSize = this->usedMemPoolSize + sizeof(testVar);
        this->numVariablesInMemPool++;

        // std::cout << "Allocated variable of size " << sizeof(testVar) << " @0x" << static_cast<void*>(ptrToPoolMemory) << std::endl;

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

// implementation of calibratable
template <class T>
calibratable<T>::calibratable(std::string label)
{
    if (RefMemoryPool == nullptr)
    {
        RefMemoryPool = new MemoryPool(MemoryPoolType::referencePage);
    }

    // std::cout << "Var-Type: " << typeid(T).name() << std::endl;

    T temp(0);

    this->valPtr = RefMemoryPool->addVariable(temp, label, VariableCategory::calibration);

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
    if (MeasMemoryPool == nullptr)
    {
        MeasMemoryPool = new MemoryPool(MemoryPoolType::measurementPage);
    }

    std::cout << "Var-Type: " << typeid(this->valPtr).name() << std::endl;
    T temp;

    this->valPtr = MeasMemoryPool->addVariable(temp, label, VariableCategory::measurement);

    this->valPtr = 0;
}

template <class T>
measurable<T>::~measurable()
{
    this->valPtr = std::numeric_limits<T>::max();
}

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
MemoryPool::MemoryPool() : usedMemPoolSize(0), numVariablesInMemPool(0), poolMemoryStartAddress(nullptr)
{
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
            std::string category = (var.second.category == MemoryPoolType::calibration) ? "calibration" : "measurement";
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
T* MemoryPool::addVariable(T testVar, std::string label, MemoryPoolType category)
{
    if ((this->usedMemPoolSize + sizeof(testVar)) > this->totalMemPoolSize)
    {
        return nullptr;
    }
    else
    {
        std::string varName = applicationName + "." + label;
        std::map<std::string, VariableIdentifier>::iterator it = this->variableList.find(varName);
        T* ptrToPoolMemory;

        // check if variable is already registered in this memory pool
        if (it == this->variableList.end())
        {
            VariableIdentifier varId;
            varId.type = typeid(T).name();
            varId.size = sizeof(testVar);
            varId.relativeAddressOffset = this->usedMemPoolSize;
            varId.category = category;

            this->variableList.insert(std::pair<std::string, VariableIdentifier>(varName, varId));
            ptrToPoolMemory = reinterpret_cast<T*>(this->poolMemoryStartAddress + this->usedMemPoolSize);
            this->usedMemPoolSize = this->usedMemPoolSize + sizeof(testVar);
            this->numVariablesInMemPool++;
        }
        else
        {
            ptrToPoolMemory = reinterpret_cast<T*>(this->getPoolStartAddress() + it->second.relativeAddressOffset);
        }

        return ptrToPoolMemory;
    }
}

// implementation of CalibrationMemoryPool
CalibrationMemoryPool::CalibrationMemoryPool()
{
    this->poolMemoryStartAddress = referencePageMemory;
    this->totalMemPoolSize = CAL_MEM_POOL_SIZE;
    this->activePage = CalibrationPageType::reference;
}

bool CalibrationMemoryPool::switchPage(CalibrationPageType page)
{
    switch (page)
    {
        case CalibrationPageType::reference:
        {
            this->poolMemoryStartAddress = referencePageMemory;
            this->activePage = CalibrationPageType::reference;

            return true;
        }
        case CalibrationPageType::working:
        {
            this->poolMemoryStartAddress = workingPageMemory;
            this->activePage = CalibrationPageType::working;

            return true;
        }
        default:
        {
            return false;
        }
    }
}

// implementation of MeasurementMemoryPool
MeasurementMemoryPool::MeasurementMemoryPool()
{
    this->poolMemoryStartAddress = measurementPageMemory;
    this->totalMemPoolSize = MEAS_MEM_POOL_SIZE;
}

// implementation of memPoolVariable
template <class T>
memPoolVariable<T>::memPoolVariable()
{
}

template <class T>
T memPoolVariable<T>::get()
{
    return *(this->valPtr);
}

template <class T>
std::string memPoolVariable<T>::getLabel()
{
    return this->label;
}

// implementation of calibratable
template <class T>
calibratable<T>::calibratable(T value, std::string label)
{
    if (CalibrationObject == nullptr)
    {
        CalibrationObject = new CalibrationMemoryPool();
    }

    this->label = label;
    this->valPtr = CalibrationObject->addVariable(value, label, MemoryPoolType::calibration);
    *(this->valPtr) = value;
}

template <class T>
calibratable<T>::~calibratable()
{
}

// implementation of measurable
template <class T>
measurable<T>::measurable(std::string label)
{
    if (MeasurementObject == nullptr)
    {
        MeasurementObject = new MeasurementMemoryPool();
    }

    this->label = label;
    T temp(0);
    this->valPtr = MeasurementObject->addVariable(temp, label, MemoryPoolType::measurement);
    *(this->valPtr) = 0;
}

template <class T>
measurable<T>::~measurable()
{
    *(this->valPtr) = std::numeric_limits<T>::max();
}

template <class T>
void measurable<T>::set(T value)
{
    *(this->valPtr) = value;
}
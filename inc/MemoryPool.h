/**
 * @file MemoryPool.h
 * @author Umberto Cancedda ()
 * @brief A memory management library that provides measurement and calibration functionality
 *        on application level. In contrast to classic approaches measurement and calibration
 *        variables do not need to be defined globally any more.
 * @version 0.1
 * @date 2022-10-27
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

namespace MemPoolLib
{
extern const std::string applicationName;

/**
 * @brief
 *
 */
static constexpr std::uint16_t CAL_MEM_POOL_SIZE = 8192;

/**
 * @brief
 *
 */
static std::uint8_t refPageMem[CAL_MEM_POOL_SIZE];

/**
 * @brief
 *
 */
static std::uint8_t wrkPageMem[CAL_MEM_POOL_SIZE];

/**
 * @brief
 *
 */
static constexpr std::uint16_t MEAS_MEM_POOL_SIZE = 8192;

/**
 * @brief
 *
 */
static std::uint8_t measMem[MEAS_MEM_POOL_SIZE];

/**
 * @brief Get the Ref Page Start Address object
 *
 * @return char*
 */
std::uint8_t* getRefPageStartAddress();

/**
 * @brief Get the Wrk Page Start Address object
 *
 * @return char*
 */
std::uint8_t* getWrkPageStartAddress();

/**
 * @brief Get the Meas Var Start Address object
 *
 * @return char*
 */
std::uint8_t* getMeasVarStartAddress();

/**
 * @brief
 *
 */
enum class MemoryPoolType
{
    referencePage = 0,
    workingPage = 1,
    measurementMemory = 2,
};

/**
 * @brief
 *
 * @tparam T
 */
class MemoryPool
{
private:
    /**
     * @brief
     *
     */
    std::uint32_t totalMemSize;

    /**
     * @brief
     *
     */
    std::uint32_t currentMemSize;

    /**
     * @brief
     *
     */
    std::uint8_t* poolMemory;

public:
    /**
     * @brief Construct a new memory Pool object
     *
     * @param size
     */
    MemoryPool(MemoryPoolType type);

    /**
     * @brief Get the Pool Start Address object
     *
     * @return char*
     */
    std::uint8_t* getPoolStartAddress();

    /**
     * @brief
     *
     * @tparam T
     * @param type
     * @return T&
     */
    template <typename T>
    T* addVariable(T testVar);
};

/**
 * @brief
 *
 */
static MemoryPool* refMemPool = nullptr;

/**
 * @brief
 *
 */
static MemoryPool* wrkMemPool = nullptr;

/**
 * @brief
 *
 */
static MemoryPool* measMemPool = nullptr;

/**
 * @brief
 *
 * @tparam T
 */
template <class T>
class memPoolVariable
{
    static_assert(std::is_arithmetic<T>::value, "Type must be numeric");

private:
protected:
    /**
     * @brief
     *
     */
    T* valPtr;

public:
    /**
     * @brief Construct a new set object
     *
     * @param value
     */
    void set(T value);

    /**
     * @brief
     *
     * @return T
     */
    T get();
};

/**
 * @brief
 *
 * @tparam T
 */
template <class T>
class calibratable : public memPoolVariable<T>
{
private:
    static std::uint32_t numVars;

public:
    /**
     * @brief Construct a new calibratable object
     *
     */
    calibratable();

    /**
     * @brief Construct a new calibratable object
     *
     * @param val
     */
    //calibratable(T val);

    /**
     * @brief Destroy the calibratable object
     *
     */
    ~calibratable();
};

/**
 * @brief
 *
 * @tparam T
 */
template <class T>
class measurable : public memPoolVariable<T>
{
private:
public:
    /**
     * @brief Construct a new measurable object
     *
     */
    measurable();

    /**
     * @brief Construct a new measurable object
     *
     * @param val
     */
    //measurable(T val);

    /**
     * @brief Destroy the measurable object
     *
     */
    ~measurable();
};

class VarIdentifier
{
private:
public:
    std::string fullName;
};

#include "MemoryPool.tpp"
}

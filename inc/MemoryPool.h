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

#include <typeinfo>
#include <map>

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
static std::uint8_t refPageMemory[CAL_MEM_POOL_SIZE];

/**
 * @brief
 *
 */
static std::uint8_t wrkPageMemory[CAL_MEM_POOL_SIZE];

/**
 * @brief
 *
 */
static constexpr std::uint16_t MEAS_MEM_POOL_SIZE = 8192;

/**
 * @brief
 *
 */
static std::uint8_t measPageMemory[MEAS_MEM_POOL_SIZE];

/**
 * @brief Get the Ref Page Start Address object
 *
 * @return char*
 */
std::uint8_t* getRefPageStartAddress();

/**
 * @brief Get the Ref Page Num Variable object
 *
 * @return std::uint32_t
 */
std::uint32_t getRefPageNumVariables();

/**
 * @brief Get the Ref Page Total Size object
 *
 * @return std::uint32_t
 */
std::uint32_t getRefPageTotalSize();

/**
 * @brief Get the Ref Page Current Size object
 *
 * @return std::uint32_t
 */
std::uint32_t getRefPageCurrentSize();

/**
 * @brief Get the Wrk Page Start Address object
 *
 * @return char*
 */
std::uint8_t* getWrkPageStartAddress();

/**
 * @brief Get the Wrk Page Num Variable object
 *
 * @return std::uint32_t
 */
std::uint32_t getWrkPageNumVariables();

/**
 * @brief Get the Wrk Page Total Size object
 *
 * @return std::uint32_t
 */
std::uint32_t getWrkPageTotalSize();

/**
 * @brief Get the Wrk Page Current Size object
 *
 * @return std::uint32_t
 */
std::uint32_t getWrkPageCurrentSize();

/**
 * @brief Get the Meas Var Start Address object
 *
 * @return char*
 */
std::uint8_t* getMeasPageStartAddress();

/**
 * @brief Get the Meas Page Num Variable object
 *
 * @return std::uint32_t
 */
std::uint32_t getMeasPageNumVariables();

/**
 * @brief Get the Meas Page Total Size object
 *
 * @return std::uint32_t
 */
std::uint32_t getMeasPageTotalSize();

/**
 * @brief Get the Meas Page Current Size object
 *
 * @return std::uint32_t
 */
std::uint32_t getMeasPageCurrentSize();

/**
 * @brief
 *
 */
enum class MemoryPoolType
{
    referencePage = 0,
    workingPage = 1,
    measurementPage = 2,
};

/**
 * @brief
 *
 */
class VarIdentifier
{
private:
public:
    //std::string fullName;
    std::string type;
    std::uint8_t size;
    std::uint32_t relativeAddressOffset;
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
    std::uint32_t totalMemPoolSize;

    /**
     * @brief
     *
     */
    std::uint32_t usedMemPoolSize;

    /**
     * @brief
     *
     */
    std::uint32_t numVariablesInMemPool;

    /**
     * @brief
     *
     */
    std::uint8_t* poolMemory;

    /**
     * @brief
     *
     */
    std::map<std::string, VarIdentifier> variableList;

public:
    /**
     * @brief Construct a new memory Pool object
     *
     * @param type
     */
    MemoryPool(MemoryPoolType type);

    /**
     * @brief Get the Pool Start Address object
     *
     * @return std::uint8_t*
     */
    std::uint8_t* getPoolStartAddress();

    /**
     * @brief Get the Pool Num Vars object
     *
     * @return std::uint32_t
     */
    std::uint32_t getPoolNumVariables();

    /**
     * @brief Get the Pool Tital Size object
     *
     * @return std::uint32_t
     */
    std::uint32_t getPoolTotalSize();

    /**
     * @brief Get the Pool Current Size object
     *
     * @return std::uint32_t
     */
    std::uint32_t getPoolCurrentSize();

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
static MemoryPool* refMemoryPool = nullptr;

/**
 * @brief
 *
 */
static MemoryPool* wrkMemoryPool = nullptr;

/**
 * @brief
 *
 */
static MemoryPool* measMemoryPool = nullptr;

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
    // calibratable(T val);

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
    // measurable(T val);

    /**
     * @brief Destroy the measurable object
     *
     */
    ~measurable();
};

#include "MemoryPool.tpp"
}

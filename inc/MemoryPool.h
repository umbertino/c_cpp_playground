#pragma once

namespace MemPoolLib
{
/**
 * @brief
 *
 */
static constexpr std::uint16_t CAL_MEM_POOL_SIZE = 8192;

/**
 * @brief
 *
 */
static constexpr std::uint16_t MEAS_MEM_POOL_SIZE = 8192;

/**
 * @brief Get the Ref Page Start Address object
 *
 * @return char*
 */
char* getRefPageStartAddress();

/**
 * @brief Get the Wrk Page Start Address object
 *
 * @return char*
 */
char* getWrkPageStartAddress();

/**
 * @brief Get the Meas Var Start Address object
 *
 * @return char*
 */
char* getMeasVarStartAddress();

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
    std::allocator<char> pool;

    /**
     * @brief
     *
     */
    char* poolMemory;

public:
    /**
     * @brief Construct a new memory Pool object
     *
     * @param size
     */
    MemoryPool(std::uint32_t size);

    /**
     * @brief Get the Pool Start Address object
     *
     * @return char*
     */
    char* getPoolStartAddress();

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

#include "MemoryPool.tpp"
}

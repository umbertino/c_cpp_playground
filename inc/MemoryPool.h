/**
 * @file MemoryPool.h
 * @author Umberto Cancedda
 * @brief A memory management library that provides measurement and calibration functionality
 *        on application level. In contrast to classic approaches measurement and calibration
 *        variables do not need to be defined globally any more.
 * @version 0.1
 * @date 2022-10-27
 *
 * @copyright Copyright (c)2022
 *
 */

#pragma once

#include <typeinfo>
#include <cstdint>
#include <string>
#include <map>

namespace MemPoolLib
{
    /**
 * @brief The name of the application. Needs to be defined by user
 *
 */
    extern const std::string applicationName;

    /**
 * @brief The size of the calibration memory in Bytes
 *
 */
    static constexpr std::uint16_t CAL_MEM_POOL_SIZE = 8192;

    /**
 * @brief The rteference page memory
 *
 */
    static std::uint8_t refPageMemory[CAL_MEM_POOL_SIZE];

    /**
 * @brief The working page memory
 *
 */
    static std::uint8_t wrkPageMemory[CAL_MEM_POOL_SIZE];

    /**
 * @brief The size of the memory foe measurement data in Bytes
 *
 */
    static constexpr std::uint16_t MEAS_MEM_POOL_SIZE = 8192;

    /**
 * @brief The memory for the measurement data
 *
 */
    static std::uint8_t measPageMemory[MEAS_MEM_POOL_SIZE];

    /**
 * @brief Defines the types of memory a pool is associated to
 *
 */
    enum class MemoryPoolType
    {
        referencePage = 0,
        workingPage = 1,
        measurementPage = 2,
    };

    /**
 * @brief Defines the category a pool variable belongs to
 *
 */
    enum class VariableCategory
    {
        calibration = 0,
        measurement = 1,
    };

    /**
 * @brief
 *
 */
    class VariableIdentifier
    {
    private:
    public:
        std::string type;
        std::uint8_t size;
        std::uint32_t relativeAddressOffset;
        VariableCategory category;
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
        std::uint8_t* poolMemoryStartAddress;

        /**
     * @brief
     *
     */
        std::map<std::string, VariableIdentifier> variableList;

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
     */
        bool dumpListOfvariables();

        /**
     * @brief
     *
     * @return true
     * @return false
     */
        bool dumpPoolMemory();

        /**
     * @brief
     *
     * @tparam T
     * @param type
     * @return T&
     */
        template<typename T>
        T* addVariable(T testVar, std::string label, VariableCategory category);
    };

    /**
 * @brief
 *
 */
    static MemoryPool* RefMemoryPool = nullptr;

    /**
 * @brief
 *
 */
    static MemoryPool* WrkMemoryPool = nullptr;

    /**
 * @brief
 *
 */
    static MemoryPool* MeasMemoryPool = nullptr;

    /**
 * @brief
 *
 * @tparam T
 */
    template<class T>
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
    template<class T>
    class calibratable : public memPoolVariable<T>
    {
    private:
        // static std::uint32_t numVars;

    public:
        /**
     * @brief Construct a new calibratable object
     *
     */
        calibratable(std::string label);

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
    template<class T>
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

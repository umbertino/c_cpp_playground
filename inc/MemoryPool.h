/**
 * @file MemoryPool.h
 * @author Umberto Cancedda
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
#include <cstdint>
#include <csignal>
#include <string>
#include <map>
#include <iomanip>

// Own Includes
#include "MemoryPoolUserConfig.h"

namespace MemPoolLib
{

/**
 * @brief The rteference page memory
 *
 */
static std::uint8_t referencePageMemory[CAL_MEM_POOL_SIZE];

/**
 * @brief The working page memory
 *
 */
static std::uint8_t workingPageMemory[CAL_MEM_POOL_SIZE];

/**
 * @brief The memory for the measurement data
 *
 */
static std::uint8_t measurementPageMemory[MEAS_MEM_POOL_SIZE];

/**
 * @brief Defines the category an item is associated to
 *
 */
enum class CategoryType
{
    /**
     * @brief Identifies a calibration item
     *
     */
    calibration = 0,

    /**
     * @brief Identifies a measurement item
     *
     */
    measurement = 1,
};

/**
 * @brief A type that helps to distinguish between reference and working data
 *
 */
enum class CalibrationPageType
{
    /**
     * @brief Identifies the read-only reference calibration data
     *
     */
    reference = 0,

    /**
     * @brief Identifies the read-write working calibration data
     *
     */
    working = 2
};

/**
 * @brief Meta data to clearly identify a variable
 *
 */
class VariableIdentifier
{
private:
public:
    /**
     * @brief The type of the variable
     *
     */
    std::string type;

    /**
     * @brief The size of the variable in Bytes
     *
     */
    std::uint8_t size;

    /**
     * @brief The address offset relative to the base address
     *
     */
    std::uint32_t relativeAddressOffset;

    /**
     * @brief Identifies the category the variable belongs to
     *
     */
    CategoryType category;
};

/**
 * @brief A memory pool base class
 *
 * @details This is the base class for memory pools. It cannot be instatiated and thus must be inherited.
 *          It monitors its used size comaring against total size and maintains a list of all
 *          used variables' meta data. On construction of a derived object a static memory area
 *          needs to be assigned to it.
 *
 */
class MemoryPool
{
protected:
    /**
     * @brief The size of the memory pool in Bytes
     *
     */
    std::uint32_t totalMemPoolSize;

    /**
     * @brief The amount of memory currently used in Bytes
     *
     */
    std::uint32_t usedMemPoolSize;

    /**
     * @brief The number of variables maintained in this pool
     *
     */
    std::uint32_t numVariablesInMemPool;

    /**
     * @brief A pointer to the memory associated to thi pool
     *
     */
    std::uint8_t* poolMemoryStartAddress;

    /**
     * @brief The list of all variables handled by this memory pool
     *
     */
    std::map<std::string, VariableIdentifier> variableList;

    /**
     * @brief The default constructor
     *
     */
    MemoryPool();

    /**
     * @brief The default destructor
     *
     */
    ~MemoryPool();

public:
    /**
     * @brief Get the pool's memory start address
     *
     * @return The start address
     */
    std::uint8_t* getPoolStartAddress();

    /**
     * @brief Get the pool's number of variables handles by this pool
     *
     * @return The number of variables handled by this pool
     */
    std::uint32_t getPoolNumVariables();

    /**
     * @brief Get the total size of the memory associated to this pool
     *
     * @return The total size of this pool's memory in Bytes
     */
    std::uint32_t getPoolTotalSize();

    /**
     * @brief Get the size of the used memory of this pool
     *
     * @return The size of this pool's memory in use in Bytes
     */
    std::uint32_t getPoolCurrentSize();

    /**
     * @brief Gets all pool variables
     *
     * @return A pointer to the map containing the variables infotmation
     */
    const std::map<std::string, VariableIdentifier>* getPoolVariables();

    /**
     * @brief Dump a list of all variables handled by this memory pool
     *
     * @return true The list could be dumped successfully
     * @return false The list is empty
     */
    bool dumpPoolListOfvariables();

    /**
     * @brief Dump a hex-output of the used pool memory
     *
     * @return true Memory could be dumped successfully
     * @return false Memory is not used, i.e. it is empty
     */
    bool dumpPoolMemory();

    /**
     * @brief Adds a variable to the memory pool.
     *
     * @details In case a variable with the specified label already exists,
     *          it is not added any further. Instead the address of the
     *          existing one is returned.
     *
     * @tparam T The type of the variable
     * @param testVar the variable to be added
     * @param label A unique name
     * @param category The category type of the variable to be added
     * @return T* The address of the variable in the pool's memory, a nullptr on failure
     */
    template <typename T>
    T* addPoolVariable(T testVar, std::string label, CategoryType category);

    /**
     * @brief Retrieves the variable identification object assosiated to a label
     *
     * @param label The name of the variable to be queried
     * @return A pointer to a VariableIdentifier object in case the variable exists
     * @return nullptr in case the name/label is not existent in the memory pool
     */
    VariableIdentifier* getPoolVariable(const std::string label);
};

/**
 * @brief A calibration memory pool class
 *
 * @details This is a concrete class that can be instatiated and may contain
 *          specifics related to calibration
 */
class CalibrationMemoryPool : public MemoryPool
{
private:
    /**
     * @brief Contains the current active page information
     *
     */
    CalibrationPageType activePage;

public:
    /**
     * @brief The default constructor
     *
     */
    CalibrationMemoryPool();

    /**
     * @brief Switches to another memory with alternative data content
     *
     * @param page The type of page to switch to
     * @return true Switching was successful
     * @return false Switching failed
     */
    bool switchPage(CalibrationPageType page);
};

/**
 * @brief A memory pool class for measurement data
 *
 * @details This is a concrete class that can be instatiated and may contain
 *          specifics related to measurement
 *
 */
class MeasurementMemoryPool : public MemoryPool
{
private:
public:
    /**
     * @brief The default constructor
     *
     */
    MeasurementMemoryPool();
};

/**
 * @brief The pointer to the memory pool object related to calibration
 *
 * @details This object is created as soon as the first variable is added to it.
 *
 */
static MemoryPool* CalibrationObject = nullptr;

/**
 * @brief The pointer to the memory pool object related to measurement
 *
 * @details This object is created as soon as the first variable is added to it.
 *
 */
static MemoryPool* MeasurementObject = nullptr;

/**
 * @brief A base class representing a variable located in a memory pool.
 *
 * @details The type of allowed variables (template type) is restricted to arithmetic types. This class
 *          cannot be instatiated and thus needs to be derived
 *
 * @tparam T The type of the variable (template type)
 */
template <class T>
class memPoolVariable
{
    /**
     * @brief Ensure that only numeric template types are accepted!
     *
     */
    static_assert(std::is_arithmetic<T>::value, "Type must be numeric");

private:
protected:
    /**
     * @brief The address of the variable in its memory pool
     *
     */
    T* valPtr;

    /**
     * @brief A human readible identifier / name for the variable
     *
     */
    std::string label;

    /**
     * @brief Keeps the signal state
     *
     */
    volatile static std::sig_atomic_t gSignalStatus;

    /**
     * @brief The default constructor
     *
     */
    memPoolVariable();

    /**
     * @brief The default destructor
     *
     */
    ~memPoolVariable();

    /**
     * @brief A signal handler
     *
     * @param signal The signel to be handled
     */
    void static signalHandler(int signal);

public:
    /**
     * @brief Return the current value of a memory pool variable
     *
     * @return The value of the memory pool variable
     */
    T get();

    /**
     * @brief Retrieves the label of the variable
     *
     * @return std::string the label
     */
    std::string getLabel();
};

/**
 * @brief A class that represents a calibration variable within a memory pool
 *
 * @tparam The varaible type (template type)
 */
template <class T>
class calibratable : public memPoolVariable<T>
{
private:
public:
    /**
     * @brief Constructs a new calibration variable
     *
     * @param value The value of the calibration variable object, cannot be altered any more
     * @param label A human readible identifier / name for the variable
     */
    calibratable(T value, std::string label);

    /**
     * @brief The default destructor
     *
     */
    ~calibratable();
};

/**
 * @brief A class that represents a measurement variable within a memory pool
 *
 * @tparam The varaible type (template type)
 */
template <class T>
class measurable : public memPoolVariable<T>
{
private:
    /**
     * @brief The default value the measurable object gets when not set or when destructed
     *
     */

public:
    /**
     * @brief Constructs a new measurement variable
     *
     * @param label A human readible identifier / name for the variable
     */
    measurable(std::string label);

    /**
     * @brief The default destructor
     *
     */
    ~measurable();

    /**
     * @brief Set the value of a measurement pool variable
     *
     * @param value The value to be set
     */
    void set(T value);
};

#include "MemoryPool.tpp"
}

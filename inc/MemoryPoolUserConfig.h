/**
 * @file MemoryPoolUserConfig.h
 * @author Umberto Cancedda ()
 * @brief This header file is for user configuration purposes
 * @version 0.1
 * @date 2022-11-06
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include <string>
#include <cstdint>

namespace MemPoolLib
{
/**
 * @brief The name of the application.
 *
 */
static const std::string applicationName = "SuperApplication";

/**
 * @brief The size of the calibration memory in Bytes
 *
 */
static constexpr std::uint16_t CAL_MEM_POOL_SIZE = 1024;

/**
 * @brief The size of the memory foe measurement data in Bytes
 *
 */
static constexpr std::uint16_t MEAS_MEM_POOL_SIZE = 8192;
}
#pragma once

#include <system_error>

enum class FlightsErrc
{
    // no 0
    NonexistentLocations = 10, // requested airport doesn't exist
    DatesInThePast, // booking flight for yesterday
    InvertedDates, // returning before departure
    NoFlightsFound = 20, // did not find any combination
    ProtocolViolation = 30, // e.g., bad XML
    ConnectionError, // could not connect to server
    ResourceError, // service run short of resources
    Timeout, // did not respond in time
};

namespace Flights
{
class FlightsErrCategory : public std::error_category
{
    virtual const char* name() const noexcept override;
    virtual std::string message(int ev) const override;
};
}

namespace std
{
template <>
struct is_error_code_enum<FlightsErrc> : true_type
{
};
}

std::error_code make_error_code(FlightsErrc);
#pragma once

#include <ostream>
#include <sstream>
#include <filesystem>

#include <boost/log/trivial.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/container/map.hpp>


// some convenience macros

// make __FILE__ a string literal
#define S(x) #x
#define S_(x) S(x)
#define __SLINE__ S_(__LINE__)

#define __BASENAME__ (std::filesystem::path(__FILE__).filename().string())
#define __FILE_EXT__ (std::filesystem::path(__FILE__).filename().extension().string())
// #define __LOCATION_B__ BOOST_PP_CAT(__BASENAME__, BOOST_PP_CAT(<<, ""))
// #define __LOCATION_B_F__ BOOST_PP_CAT(BOOST_PP_CAT(__LOCATION_B__, BOOST_PP_CAT(<<, " ")), __FUNCTION__)
// #define __LOCATION_B_F_L__ BOOST_PP_CAT(BOOST_PP_CAT(__LOCATION_B_F__, BOOST_PP_CAT(<<, " ")), __SLINE__)
// #define __LOCATION__ __LOCATION_B_F_L__

class Logger
{
public:
    typedef enum
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERR,
        FATAL
    } logLevel;

    Logger(std::ostream& strm);
    Logger(std::ofstream& strm);
    ~Logger();
    void log(Logger::logLevel level, const std::ostream& messageStream);
    void trace(const std::ostream& messageStream);
    void info(const std::ostream& messageStream);
    void debug(const std::ostream& messageStream);
    void warn(const std::ostream& messageStream);
    void error(const std::ostream& messageStream);
    void fatal(const std::ostream& messageStream);

    void start();
    void stop();
    void resume();
    void suppress();

private:
    bool loggingSuppressed;
    std::string message;
    boost::scoped_ptr<boost::log::sources::severity_logger<boost::log::trivial::severity_level>> boostLogger;
};
#pragma once

#include <ostream>
#include <sstream>

#include <boost/scoped_ptr.hpp>
#include <boost/container/map.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/log/trivial.hpp>


// some convenience macros
#define __BASENAME__ (boost::filesystem::path(__FILE__).filename().string())
#define __FILE_EXT__ (boost::filesystem::path(__FILE__).filename().extension().string())
#define __LOCATION__ __BASENAME__ << ":" <<__FUNCTION__ <<":" << __LINE__ << ":"


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
    //Logger(std::ofstream& strm);
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
    std::ostream& channel;
    std::stringstream message;
    boost::scoped_ptr<boost::log::sources::severity_logger<boost::log::trivial::severity_level>> boostLogger;
};
#pragma once

#include <string>

#include <boost/log/trivial.hpp>

#include <boost/scoped_ptr.hpp>
#include <boost/container/map.hpp>

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
    Logger::Logger(std::ofstream& strm);
    ~Logger();
    void log(Logger::logLevel level, const std::string& message);

private:
    //static const boost::container::map<Logger::logLevel, boost::log::trivial::severity_level> levelTranslator;
    boost::scoped_ptr<boost::log::sources::severity_logger<boost::log::trivial::severity_level>> boostLogger;
};
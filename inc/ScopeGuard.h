#pragma once

#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/scoped_ptr.hpp>

class ScopeGuard
{
private:
    static boost::mutex scopeGuardMutex;

public:
    ScopeGuard(bool error, bool warning, bool info);
    ~ScopeGuard();
};

// pass parameters
#define SUPPRESS_LOGGING(error, warning, info, ...) \
    { \
        const boost::scoped_ptr<ScopeGuard> scopeGuard(new ScopeGuard(error, warning, info)); \
        __VA_ARGS__ \
    }


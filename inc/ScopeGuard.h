#pragma once

#include <memory>

class ScopeGuard
{
private:
    /* data */
public:
    ScopeGuard(bool error, bool warning, bool info);
    ~ScopeGuard();
};

// pass parameters
#define SUPPRESS_LOGGING(error, warning, info, ...) \
    { \
        const std::unique_ptr<ScopeGuard> scope(new ScopeGuard(error, warning, info)); \
        __VA_ARGS__ \
    }


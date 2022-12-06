#include "ScopeGuard.h"
#include <iostream>

boost::mutex ScopeGuard::scopeGuardMutex;

ScopeGuard::ScopeGuard(bool error, bool warning, bool info)
{
    // maybe useful to be thread-safe
    boost::lock_guard<boost::mutex> lockMtx(ScopeGuard::scopeGuardMutex);

    // do whatever you need to do to set up this scope
    std::cout << "ScopeGuard created" << std::endl;
    std::cout << "Errors: " << error << std::endl;
    std::cout << "Warnings: " << warning << std::endl;
    std::cout << "Info: " << info << std::endl;
}

ScopeGuard::~ScopeGuard()
{
    // maybe useful to be thread-safe
    boost::lock_guard<boost::mutex> lockMtx(ScopeGuard::scopeGuardMutex);

    // revert everything back before leaving this scope
    std::cout << "ScopeGuard destroyed" << std::endl;
}

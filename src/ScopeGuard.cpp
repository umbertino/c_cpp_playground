#include "ScopeGuard.h"
#include <iostream>

ScopeGuard::ScopeGuard(bool error, bool warning, bool info)
{
    // do whatever you need to do to set up this scope
    std::cout << "ScopeGuard created" << std::endl;
    std::cout << "Errors: " << error << std::endl;
    std::cout << "Warnings: " << warning << std::endl;
    std::cout << "Info: " << info << std::endl;
}

ScopeGuard::~ScopeGuard()
{
    // revert everything back before leaving this scope
    std::cout << "ScopeGuard destroyed" << std::endl;
}

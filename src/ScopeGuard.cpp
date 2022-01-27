#include "ScopeGuard.h"
#include <iostream>


ScopeGuard::ScopeGuard()
{
    // do whatever you need to do to set up this scope
    std::cout << "ScopeGuard created" << std::endl;
}

ScopeGuard::~ScopeGuard()
{
    // revert everything back before leaving this scope
    std::cout << "ScopeGuard destroyed" << std::endl;
}

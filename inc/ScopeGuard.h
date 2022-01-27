#include <memory>

class ScopeGuard
{
private:
    /* data */
public:
    ScopeGuard();
    ~ScopeGuard();
};

#define SUPPRESS_LOGGING(...) \
    { \
        const std::unique_ptr<ScopeGuard> scope = std::make_unique<ScopeGuard>(); \
        __VA_ARGS__ \
    }


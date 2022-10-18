// Std-Includes

// Own Includes
#include "MemoryPool.h"

// implementation of calibratable
template <typename T>
calibratable<T>::calibratable()
{
    this->var = 0;
}

template <typename T>
calibratable<T>::calibratable(T val)
{
    this->var = val;
}

// implementation of measurable
template <typename T>
measurable<T>::measurable()
{
    this->var = 0;
}

template <typename T>
measurable<T>::measurable(T val)
{
    this->var = val;
}

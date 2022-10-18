

/**
 * @brief
 *
 * @tparam T
 */
template <typename T>
class calibratable
{
private:
    T var;

public:
    calibratable();
    calibratable(T val);
};

/**
 * @brief
 *
 * @tparam T
 */
template <typename T>
class measurable
{
private:
    T var;

public:
    measurable();
    measurable(T val);
};

class MemoryPool
{
private:
public:
    MemoryPool();
    ~MemoryPool();
};

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

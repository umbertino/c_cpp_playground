

template <std::size_t N>
myClass<N>::myClass()
{
    //this->localArray.fill(0);
}

template <std::size_t N>
template <typename T>
void myClass<N>::myFunc()
{
    //this->localArray[0] = 9;
    //std::cout << "array contains " << this->localArray.size() << "elements " << +this->localArray[0];
    std::cout << "myFunc " << sizeof(T) <<std::endl;
}

template <std::size_t N>
myDeClass<N>::myDeClass(std::array<std::uint8_t, N>& p)// : localArray(p)
{
    //this->localArray.fill(0);
}

template <size_t N>
template <typename T>
void myDeClass<N>::myDeFunc()
{
    this->template myFunc<T>();
};

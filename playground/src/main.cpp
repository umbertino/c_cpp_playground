// Boost-Includes
#include <boost/chrono.hpp>
#include <boost/thread/thread.hpp>
#include <boost/container/vector.hpp>
#include <boost/format.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/core/typeinfo.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

// Std-Includes
#include <algorithm>
#include <iostream>
#include <ostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <chrono>
#include <ctime>
#include <cstddef>
#include <iomanip>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <future>
#include <array>
#include <condition_variable>
#include <thread>

// Own Includes
#include <ScopeGuard.h>

// switches to activate / deactivate examples
#define SCRATCH_PAD 0
#define SCOPE_GUARD_EXAMPLE 0
#define FUTURE_PROMISE_EXAMPLE 1
#define CONDVAR_EXAMPLE 0

std::ostream* logOutChannel;

std::ostream& print()
{
    logOutChannel = &std::cout;

    return *logOutChannel;
}

template<std::size_t N>
void myFunc(std::array<std::uint8_t, N>& p)
{
    std::array<std::uint8_t, N>& localArray = p;

    p[0] = 9;
    std::cout << "array contains " << p.size() << "elements " << +p[0];
    std::cout << std::endl;
    std::cout << "array contains " << localArray.size() << "elements " << +localArray[0];
    std::cout << std::endl;
}

template<typename T, std::size_t X>
using vector = std::array<T, X>;

template<typename T, std::size_t X, std::size_t Y>
using matrix = std::array<std::array<T, Y>, X>;

template<typename T, std::size_t X, std::size_t Y, std::size_t Z>
using cube = std::array<std::array<std::array<T, Z>, Y>, X>;

std::array<std::uint8_t, 8> myArray;

std::mutex mtx;
std::condition_variable cv;
std::atomic<bool> ready1 = false;
std::atomic<bool> ready2 = false;

bool getPermission(void)
{
    return ready1 && ready2;
}

// a non-optimized way of checking for prime numbers:
void is_prime(std::promise<bool>&& boolPromise, std::uint64_t x)
{
    bool isPrime = true;

    for (std::uint64_t i = 2; i < x; ++i)
    {
        if (x % i == 0)
        {
            isPrime = false;
            break;
        }
    }

    boolPromise.set_value(isPrime);
}

void slaveThread1()
{
    while (true)
    {
        {
            std::unique_lock<std::mutex> lck(mtx);
            cv.wait(lck, getPermission);
            std::cout << "1" << std::flush;
        }

        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}

void slaveThread2()
{
    while (true)
    {
        {
            std::unique_lock<std::mutex> lck(mtx);
            cv.wait(lck, getPermission);
            std::cout << "2" << std::flush;
        }

        std::this_thread::sleep_for(std::chrono::seconds(4));
    }
}

void masterThread()
{
    while (true)
    {
        {
            std::scoped_lock<std::mutex> lck(mtx);
            ready1 = true;
            ready2 = true;
        }

        std::cout << " M" << std::flush;
        cv.notify_all();

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main(void)
{
    std::cout << "Hello, this is a C++ playground" << std::endl << std::endl;

#if SCRATCH_PAD

    std::cout << std::endl;

    const std::uint8_t X = 2;
    const std::uint8_t Y = 3;
    const std::uint8_t Z = 4;

    vector<std::uint16_t, X> vec{1, 2};
    matrix<std::uint16_t, X, Y> mat{{{1, 2, 3}, {11, 12, 13}}};

    std::array<std::uint8_t, Z> zInner{1, 2, 3, 4};

    std::array<std::array<std::uint8_t, Z>, Y> yInner{zInner, zInner, zInner};

    cube<std::uint8_t, X, Y, Z> cub{
        {{{{111, 112, 113, 114}, {121, 122, 123, 124}, {131, 132, 133, 134}}},
         {{{211, 212, 213, 214}, {221, 222, 223, 224}, {231, 232, 233, 234}}}}};

    //std::uint8_t cub{
    // {{1, 2, 3}, {11, 12, 13}},  {{21, 22, 23},  {31, 32, 33}}, {{41, 42, 43}, {51, 52, 53}}, {{61, 62, 63}, {71, 72, 73}}
    // };

    std::cout << std::endl;

    // for (int j = 0; j < Y; j++)
    // {
    //     for (int i = 0; i < X; i++)
    //     {
    //         mat[i][j] = 1 + j + i * 10;
    //         //std::cout << "mat[" << +i << "][" << +j << "] = " << +mat[i][j] << std::endl;
    //     }

    //     std::cout << std::endl;
    // }

    // for (int j = 0; j < Y; j++)
    // {
    //     for (int i = 0; i < X; i++)
    //     {
    //         std::cout << "mat[" << +i << "][" << +j << "] = " << +mat[i][j] << std::endl;
    //     }

    //     std::cout << std::endl;
    // }

    std::cout << std::endl;

    // for (int k = 0; k < Z; k++)
    // {
    //     for (int j = 0; j < Y; j++)
    //     {
    //         for (int i = 0; i < X; i++)
    //         {
    //             std::uint16_t val = i + j * 10 + k * 100;

    //             cub[i][j][k] = val;
    //         }
    //     }
    // }

    for (int k = 0; k < Z; k++)
    {
        for (int j = 0; j < Y; j++)
        {
            for (int i = 0; i < X; i++)
            {
                std::cout << "cub[" << +i << "][" << +j << "][" << +k << "] = " << +cub[i][j][k] << std::endl;
            }

            std::cout << std::endl;
        }

        std::cout << std::endl;
    }

    // std::cout << std::endl;

#endif

#if SCOPE_GUARD_EXAMPLE
    std::cout << "RAII example, exception safe scope guard" << std::endl;

    try
    {
        // usage with helper macro (scope is created by macro)
        SUPPRESS_LOGGING(true, false, true, {
            std::cout << "Scoped output V1" << std::endl;
            throw std::invalid_argument("received negative value");
        })
    }
    catch (const std::invalid_argument& e)
    {
        std::cerr << "Std-Exception caught: " << e.what() << std::endl;
    }

    std::cout << std::endl;

    try
    {
        // usage with scoped pointer and curly block braces
        { // this is the start of the scope
            const boost::scoped_ptr<ScopeGuard> scopeGuard(new ScopeGuard(true, false, true));

            boost::container::vector<int> myvector(10);

            myvector.at(20) = 100; // vector::at throws an out-of-range

        } // this is the end of the scope
    }
    catch (const boost::container::out_of_range& e)
    {
        std::cerr << "Boost-Exception caught: " << e.what() << std::endl;
    }

    std::cout << std::endl;
#endif

#if FUTURE_PROMISE_EXAMPLE
    std::uint64_t testNum = 1234575371;

    std::promise<bool> primePromise;
    std::future<bool> primeResult = primePromise.get_future();

    std::thread primeThread(is_prime, std::move(primePromise), testNum);

    // do something while waiting for function to set future:
    std::cout << "checking, please wait " << std::flush;
    std::chrono::milliseconds span(250);

    while (primeResult.wait_for(span) == std::future_status::timeout)
    {
        std::cout << "." << std::flush;
    }

    bool x = primeResult.get(); // retrieve return value

    std::cout << "\n" << testNum << (x ? " is" : " is not") << " prime.\n";

#endif

#if CONDVAR_EXAMPLE
    std::cout << std::endl;
    std::thread t2(slaveThread1);
    std::thread t3(slaveThread2);

    //std::this_thread::sleep_for(std::chrono::seconds(5));
    std::thread t1(masterThread);

    t1.join();
    t2.join();
    t3.join();

    std::cout << std::endl;

#endif

    return 0;
}

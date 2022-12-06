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
#include <queue>
#include <thread>
#include <future>
#include <array>
#include <condition_variable>
#include <thread>

// Own Includes
#include <ScopeGuard.h>

// switches to activate / deactivate examples
#define SCRATCH_PAD 1
#define SCOPE_GUARD_EXAMPLE 0
#define FUTURE_PROMISE_EXAMPLE 0
#define CONDVAR_EXAMPLE 0

std::ostream* logOutChannel;

std::ostream& print()
{
    logOutChannel = &std::cout;

    return *logOutChannel;
}

template <std::size_t N>
void myFunc(std::array<std::uint8_t, N>& p)
{
    std::array<std::uint8_t, N>& localArray = p;

    p[0] = 9;
    std::cout << "array contains " << p.size() << "elements " << +p[0];
    std::cout << std::endl;
    std::cout << "array contains " << localArray.size() << "elements " << +localArray[0];
    std::cout << std::endl;
}

std::array<std::uint8_t, 8> myArray;

boost::mutex mtx;
boost::condition_variable cv;
bool ready = false;

void slaveThread1()
{
    while (true)
    {
        {
            boost::unique_lock<boost::mutex> lck(mtx);
            cv.wait(lck);
            std::cout << "1" << std::flush;
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

void slaveThread2()
{
    while (true)
    {
        {
            boost::unique_lock<boost::mutex> lck(mtx);
            cv.wait(lck);
            std::cout << "2" << std::flush;
        }

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

void masterThread()
{
    while (true)
    {
        {
            boost::unique_lock<boost::mutex> lck(mtx);
            std::cout << " M" << std::flush;
            cv.notify_all();
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main(void)
{
    std::cout << "Hello, this is a C++ playground" << std::endl
              << std::endl;

#if SCRATCH_PAD

    std::cout << std::endl;
#endif


#if SCOPE_GUARD_EXAMPLE
    std::cout << "RAII example, exception safe scope guard" << std::endl;

    try
    {
        // usage with helper macro (scope is created by macro)
        SUPPRESS_LOGGING(true, false, true,
                         {
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
    std::atomic<bool> magic1Ready(false);
    std::atomic<bool> magic2Ready(false);
    std::promise<int> prom;
    std::future<int> fut = prom.get_future();

    auto magic1 = std::async(std::launch::async, [&]()
                             {
                                 std::this_thread::sleep_for(std::chrono::seconds(5));
                                 magic1Ready.store(true);
                                 return 42; });

    auto magic2 = std::async(std::launch::async, [&]()
                             {
                                 std::this_thread::sleep_for(std::chrono::seconds(8));
                                 prom.set_value(10);
                                 magic2Ready.store(true); });

    std::cout << "Waiting";

    while (!magic1Ready.load() || !magic2Ready.load())
    {
        std::cout << ".";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << std::endl;

    try
    {
        std::cout << magic1.get() << std::endl;
        std::cout << fut.get() << std::endl;

        if (fut.valid() && magic1.valid())
        { // cannot consume any more, raises exception
            std::cout << magic1.get() << std::endl;
            std::cout << fut.get() << std::endl;
        }
    }
    catch (std::exception& ex)
    {
        std::cout << "Exception caught: " << ex.what();
    }

#endif

#if CONDVAR_EXAMPLE
    std::cout << std::endl;

    std::thread t2(slaveThread1);
    std::thread t3(slaveThread2);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::thread t1(masterThread);

    t1.join();
    t2.join();
    t3.join();

    std::cout << std::endl;

#endif

    return 0;
}

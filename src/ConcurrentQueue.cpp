#include "ConcurrentQueue.h"


void ConcurrentQueue::push(Data const& data)
{
    boost::mutex::scoped_lock lock(the_mutex);
    the_queue.push(data);
    lock.unlock();
    the_condition_variable.notify_one();
}

bool ConcurrentQueue::empty() const
{
    boost::mutex::scoped_lock lock(the_mutex);
    return the_queue.empty();
}

bool ConcurrentQueue::try_pop(Data& popped_value)
{
    boost::mutex::scoped_lock lock(the_mutex);
    if (the_queue.empty())
    {
        return false;
    }

    popped_value = the_queue.front();
    the_queue.pop();
    return true;
}

void ConcurrentQueue::wait_and_pop(Data& popped_value)
{
    boost::mutex::scoped_lock lock(the_mutex);
    while (the_queue.empty())
    {
        the_condition_variable.wait(lock);
    }

    popped_value = the_queue.front();
    the_queue.pop();
}

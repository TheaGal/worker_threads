#pragma once

#include <cstdint>
#include <thread>


namespace THEA
{
namespace worker_threads
{

/// Helper func for getting ideal number of threads.
static uint32_t calc_ideal_num_threads();

/// Group of worker threads. Only one should be created in an application.
class Worker_thread_group
{
public:
    Worker_thread_group(uint32_t num_threads = calc_ideal_num_threads());

    Worker_thread_group(Worker_thread_group const&)             = delete;
    Worker_thread_group& operator=(Worker_thread_group const&)  = delete;
    Worker_thread_group(Worker_thread_group const&&)            = delete;
    Worker_thread_group& operator=(Worker_thread_group const&&) = delete;

    ~Worker_thread_group();

    uint32_t get_num_threads() const;
};

} // namespace worker_threads
} // namespace THEA

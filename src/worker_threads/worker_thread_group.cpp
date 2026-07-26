#include "worker_threads/worker_thread_group.h"

#include <cstdint>


namespace THEA
{

static uint32_t worker_threads::calc_ideal_num_threads()
{
    uint32_t ideal_threads{ std::thread::hardware_concurrency() };
    if (ideal_threads > 1)
        ideal_threads--;
    return ideal_threads;
}

} // namespace THEA

#include "worker_threads_public.h"

#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>


int32_t some_job()
{
    std::cout << "started " << __func__ << "()\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "finished " << __func__ << "()\n";
    return 123;
}

// void some_job2()
// {
//     TIMESLICER_SCOPED(TWOS, 0);
//     std::cout << __func__ << "()\n";
// }

// void some_job3()
// {
//     TIMESLICER_SCOPED(TWOS, 1);
//     std::cout << __func__ << "()\n";
// }

// void some_job4()
// {
//     TIMESLICER_SCOPED(EIGHTS, 3);
//     std::cout << __func__ << "()\n";
// }


int32_t main()
{
    THEA::worker_threads::Worker_thread_group worker_thread_group;
    THEA::worker_threads::set_submission_thread_group(worker_thread_group);

    // Submit single job and wait.
    int32_t result = 0;
    auto jb_key = THEA::worker_threads::submit_job([&]() { result = some_job(); });

    do
    {
        assert(result == 0);
    }
    while (!THEA::worker_threads::has_job_batch_finished(jb_key));
    assert(result == 123);

    // for (size_t i = 0; i < 500; i++)
    // {
    //     TIMESLICER_TICK;
    //     std::cout << "--------------- Tick " << i << " ---------------\n";

    //     some_func();
    //     some_func2();
    //     some_func3();
    //     some_func4();
    // }

    // std::cout << "-=-=-=-=-=-=-=- FINISH -=-=-=-=-=-=-=-\n";

    // TIMESLICER_PRINT_PERFORMANCE_REPORT;
    // TIMESLICER_PRINT_TIMESLICE_PATTERN_REPORT;

    return 0;
}
